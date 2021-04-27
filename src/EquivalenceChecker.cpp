/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceChecker.hpp"

#include <chrono>
namespace ec {

    EquivalenceChecker::EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2):
        qc1(qc1), qc2(qc2) {
        dd = std::make_unique<dd::Package>();
        dd->setMode(dd::Matrix);

        line.fill(qc::LINE_DEFAULT);

        // currently this modifies the underlying quantum circuits
        // in the future this might want to be avoided
        qc1.stripIdleQubits();
        qc2.stripIdleQubits();

        auto& larger_circuit  = qc1.getNqubits() > qc2.getNqubits() ? qc1 : qc2;
        auto& smaller_circuit = qc1.getNqubits() > qc2.getNqubits() ? qc2 : qc1;

        setupAncillariesAndGarbage(smaller_circuit, larger_circuit);

        initial1   = qc1.initialLayout;
        initial2   = qc2.initialLayout;
        output1    = qc1.outputPermutation;
        output2    = qc2.outputPermutation;
        ancillary1 = qc1.ancillary;
        ancillary2 = qc2.ancillary;
        garbage1   = qc1.garbage;
        garbage2   = qc2.garbage;

        if (qc1.getNqubitsWithoutAncillae() != qc2.getNqubitsWithoutAncillae()) {
            std::cerr << "[QCEC] Warning: circuits have different number of primary inputs! Proceed with caution!" << std::endl;
        }
        nqubits = qc1.getNqubitsWithoutAncillae() + std::max(qc1.getNancillae(), qc2.getNancillae());

        fixOutputPermutationMismatch(smaller_circuit);
        method = Method::Reference;
    }

    void EquivalenceChecker::setupAncillariesAndGarbage(qc::QuantumComputation& smaller_circuit, qc::QuantumComputation& larger_circuit) {
        if (std::max(qc1.getNqubits(), qc2.getNqubits()) <= qc::MAX_QUBITS) {
            unsigned short                                nqubits_to_remove = larger_circuit.getNqubits() - smaller_circuit.getNqubits();
            std::vector<std::pair<unsigned short, short>> removed{};
            std::bitset<qc::MAX_QUBITS>                   garbage{};
            for (unsigned short i = 0; i < nqubits_to_remove; ++i) {
                auto logical_qubit_index = qc::QuantumComputation::getHighestLogicalQubitIndex(larger_circuit.initialLayout);
                smaller_circuit.setLogicalQubitAncillary(logical_qubit_index);

                // store whether qubit was garbage
                garbage[logical_qubit_index] = larger_circuit.logicalQubitIsGarbage(logical_qubit_index);
                removed.push_back(larger_circuit.removeQubit(logical_qubit_index));
            }

            for (auto it = removed.rbegin(); it != removed.rend(); ++it) {
                larger_circuit.addAncillaryQubit(it->first, it->second);
                smaller_circuit.setLogicalQubitGarbage(larger_circuit.getNqubits() - 1);
                // restore garbage
                if (garbage[larger_circuit.getNqubits() - 1]) {
                    larger_circuit.setLogicalQubitGarbage(larger_circuit.getNqubits() - 1);
                }
            }
        }
    }

    void EquivalenceChecker::fixOutputPermutationMismatch(qc::QuantumComputation& circuit) {
        if (std::max(qc1.getNqubits(), qc2.getNqubits()) <= qc::MAX_QUBITS) {
            // Try to fix potential mismatches in output permutations
            auto& smaller_initial   = qc1.getNqubits() > qc2.getNqubits() ? initial2 : initial1;
            auto& larger_output     = qc1.getNqubits() > qc2.getNqubits() ? output1 : output2;
            auto& smaller_output    = qc1.getNqubits() > qc2.getNqubits() ? output2 : output1;
            auto& smaller_ancillary = qc1.getNqubits() > qc2.getNqubits() ? ancillary2 : ancillary1;
            auto& larger_garbage    = qc1.getNqubits() > qc2.getNqubits() ? garbage1 : garbage2;
            auto& smaller_garbage   = qc1.getNqubits() > qc2.getNqubits() ? garbage2 : garbage1;

            for (const auto& o: larger_output) {
                unsigned short output_qubit_in_larger_circuit = o.second;
                //				unsigned short physical_qubit_in_larger_circuit = o.first;
                //			std::cout << "Output logical qubit " << output_qubit_in_larger_circuit << " at physical qubit " << physical_qubit_in_larger_circuit;
                unsigned short nout = 1;
                for (int i = 0; i < output_qubit_in_larger_circuit; ++i) {
                    if (!larger_garbage.test(i))
                        ++nout;
                }
                //			std::cout << " which is logical output qubit number " << nout << std::endl;

                unsigned short outcount                        = nout;
                unsigned short output_qubit_in_smaller_circuit = 0;
                bool           exists_in_smaller               = false;
                for (int i = 0; i < circuit.getNqubits(); ++i) {
                    if (!smaller_garbage.test(i)) {
                        --outcount;
                    }
                    if (outcount == 0) {
                        output_qubit_in_smaller_circuit = i;
                        exists_in_smaller               = true;
                        break;
                    }
                }
                // algorithm has logical qubit that does not exist in the smaller circuit
                if (!exists_in_smaller)
                    continue;

                //			std::cout << "This is logical qubit " << output_qubit_in_smaller_circuit << " in the smaller circuit";
                unsigned short physical_qubit_in_smaller_circuit = 0;
                for (const auto& out: smaller_output) {
                    if (out.second == output_qubit_in_smaller_circuit) {
                        physical_qubit_in_smaller_circuit = out.first;
                        break;
                    }
                }
                //			std::cout << " which is assigned to physical qubit " << physical_qubit_in_smaller_circuit << " at the end of the circuit" << std::endl;

                if (output_qubit_in_larger_circuit != output_qubit_in_smaller_circuit) {
                    //				std::cout << "Mismatch in the logical output qubits" << std::endl;
                    if (smaller_ancillary.test(output_qubit_in_larger_circuit) && smaller_garbage.test(output_qubit_in_larger_circuit)) {
                        bool           found                               = false;
                        unsigned short physical_index_of_larger_in_smaller = 0;
                        for (const auto& in: smaller_initial) {
                            if (in.second == output_qubit_in_larger_circuit) {
                                found                               = true;
                                physical_index_of_larger_in_smaller = in.first;
                                break;
                            }
                        }
                        //					if (found) {
                        //						std::cout << "Found logical qubit " << output_qubit_in_larger_circuit << " in smaller circuit at physical qubit " << physical_index_of_larger_in_smaller << std::endl;
                        //						std::cout << "This qubit is idle: " << smaller_circuit.isIdleQubit(physical_index_of_larger_in_smaller) << std::endl;
                        //					}
                        if (!found || circuit.isIdleQubit(physical_index_of_larger_in_smaller)) {
                            //						std::cout << "Logical qubit " << output_qubit_in_smaller_circuit << " can be moved to logical qubit " << output_qubit_in_larger_circuit << std::endl;
                            for (auto& in: smaller_initial) {
                                if (in.second == output_qubit_in_smaller_circuit) {
                                    in.second = output_qubit_in_larger_circuit;
                                    //								std::cout << "Physical qubit " << in.first << " has been assigned logical qubit " << in.second << " as input" << std::endl;
                                    break;
                                }
                            }
                            smaller_output[physical_qubit_in_smaller_circuit] = output_qubit_in_larger_circuit;
                            //						std::cout << "Physical qubit " << physical_qubit_in_smaller_circuit << " has been assigned logical qubit " << output_qubit_in_larger_circuit << " as output" << std::endl;
                            smaller_ancillary[output_qubit_in_larger_circuit] = smaller_ancillary.test(output_qubit_in_smaller_circuit);
                            smaller_ancillary.set(output_qubit_in_smaller_circuit);
                            //						std::cout << "Logical qubit " << output_qubit_in_larger_circuit << " was assigned the ancillary status of qubit " << output_qubit_in_smaller_circuit << " (i.e., " << smaller_ancillary.test(output_qubit_in_larger_circuit) << ")" << std::endl;
                            smaller_garbage.reset(output_qubit_in_larger_circuit);
                            smaller_garbage.set(output_qubit_in_smaller_circuit);
                            //						std::cout << "Logical qubit " << output_qubit_in_larger_circuit << " has been removed from the garbage outputs" << std::endl;
                        }
                    } else {
                        std::cerr << "Uncorrected mismatch in output qubits!" << std::endl;
                    }
                }
            }
        }
    }

    /// Take operation and apply it either from the left or (inverted) from the right
    /// \param op operation to apply
    /// \param to DD to apply the operation to
    /// \param dir LEFT or RIGHT
    void EquivalenceChecker::applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, Direction dir) {
        // set appropriate qubit count to generate correct DD
        auto nq = op->getNqubits();
        op->setNqubits(nqubits);

        auto saved = to;
        if (dir == LEFT) {
            to = dd->multiply(op->getDD(dd, line, permutation), to);
        } else {
            to = dd->multiply(to, op->getInverseDD(dd, line, permutation));
        }
        dd->incRef(to);
        dd->decRef(saved);
        dd->garbageCollect();

        // reset qubit count
        op->setNqubits(nq);
    }

    void EquivalenceChecker::applyGate(decltype(qc1.begin())& opIt, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, const decltype(qc1.cend())& end, Direction dir) {
        // Measurements at the end of the circuit are considered NOPs.
        if ((*opIt)->getType() == qc::Measure) {
            if (!qc::QuantumComputation::isLastOperationOnQubit(opIt, end)) {
                throw std::invalid_argument("Intermediate measurements currently not supported. Defer your measurements to the end.");
            }
            return;
        }
        applyGate(*opIt, to, permutation, dir);
    }

    void EquivalenceChecker::setupResults(EquivalenceCheckingResults& results) {
        results.circuit1.name = qc1.getName();
        results.circuit2.name = qc2.getName();
        results.name          = results.circuit1.name + " and " + results.circuit2.name;

        results.nqubits          = nqubits;
        results.circuit1.nqubits = qc1.getNqubits();
        results.circuit2.nqubits = qc2.getNqubits();

        results.circuit1.ngates = qc1.getNops();
        results.circuit2.ngates = qc2.getNops();

        results.method = method;
    }

    EquivalenceCheckingResults EquivalenceChecker::check(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);

        if (!validInstance()) {
            return results;
        }

        auto start = std::chrono::steady_clock::now();
        runPreCheckPasses(config);
        auto endPreprocessing = std::chrono::steady_clock::now();

        qc::permutationMap perm1 = initial1;
        dd::Edge           e     = dd->makeIdent(nqubits);
        dd->incRef(e);
        e = dd->reduceAncillae(e, ancillary1);

        while (it1 != end1) {
            applyGate(it1, e, perm1, end1);
            ++it1;
        }
        qc::QuantumComputation::changePermutation(e, perm1, output1, line, dd);
        e = dd->reduceAncillae(e, ancillary1);
        e = dd->reduceGarbage(e, garbage1);

        qc::permutationMap perm2 = initial2;
        dd::Edge           f     = dd->makeIdent(nqubits);
        dd->incRef(f);
        f = dd->reduceAncillae(f, ancillary2);

        while (it2 != end2) {
            applyGate(it2, f, perm2, end2);
            ++it2;
        }
        qc::QuantumComputation::changePermutation(f, perm2, output2, line, dd);
        f = dd->reduceAncillae(f, ancillary2);
        f = dd->reduceGarbage(f, garbage2);

        results.maxActive = dd->maxActive;

        results.equivalence = equals(e, f);
        if (results.equivalence == Equivalence::NotEquivalent) {
            results.result = dd->multiply(e, dd->conjugateTranspose(f));
            dd->decRef(e);
            dd->decRef(f);
            dd->incRef(results.result);
        } else {
            results.result = e;
            dd->decRef(f);
        }

        auto                          endVerification   = std::chrono::steady_clock::now();
        std::chrono::duration<double> preprocessingTime = endPreprocessing - start;
        std::chrono::duration<double> verificationTime  = endVerification - endPreprocessing;
        results.preprocessingTime                       = preprocessingTime.count();
        results.verificationTime                        = verificationTime.count();

        return results;
    }

    bool EquivalenceChecker::validInstance() {
        if (qc1.getNqubits() > qc::MAX_QUBITS || qc2.getNqubits() > qc::MAX_QUBITS) {
            std::cerr << "Circuit contains too many qubits to be manageable by QFR library" << std::endl;
            return false;
        }
        return true;
    }

    Equivalence EquivalenceChecker::equals(const dd::Edge& e, const dd::Edge& f) {
        if (e.p != f.p) return Equivalence::NotEquivalent;

        if (!dd::ComplexNumbers::equals(e.w, f.w)) return Equivalence::EquivalentUpToGlobalPhase;

        return Equivalence::Equivalent;
    }

    void EquivalenceChecker::runPreCheckPasses(const Configuration& config) {
        setTolerance(config.tolerance);

        if (config.removeDiagonalGatesBeforeMeasure) {
            qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc1);
            qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc2);
        }

        if (config.reconstructSWAPs) {
            qc::CircuitOptimizer::swapReconstruction(qc1);
            qc::CircuitOptimizer::swapReconstruction(qc2);
        }

        if (config.fuseSingleQubitGates) {
            qc::CircuitOptimizer::singleQubitGateFusion(qc1);
            qc::CircuitOptimizer::singleQubitGateFusion(qc2);
        }

        it1  = qc1.begin();
        it2  = qc2.begin();
        end1 = qc1.cend();
        end2 = qc2.cend();
    }
} // namespace ec
