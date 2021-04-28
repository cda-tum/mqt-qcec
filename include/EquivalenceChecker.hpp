/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include "CircuitOptimizer.hpp"
#include "EquivalenceCheckingResults.hpp"
#include "QuantumComputation.hpp"

#include <chrono>
#include <memory>
#include <string>

namespace ec {
    enum Direction : bool { LEFT  = true,
                            RIGHT = false };

    struct Configuration {
        ec::Method   method    = ec::Method::G_I_Gp;
        ec::Strategy strategy  = ec::Strategy::Proportional;
        dd::fp       tolerance = dd::ComplexTable<>::tolerance();

        // configuration options for optimizations
        bool fuseSingleQubitGates             = true;
        bool reconstructSWAPs                 = true;
        bool removeDiagonalGatesBeforeMeasure = false;

        // configuration options for PowerOfSimulation equivalence checker
        double      fidelity_limit = 0.999;
        std::size_t max_sims       = 16;
        StimuliType stimuliType    = ec::StimuliType::Classical;
        bool        storeCEXinput  = false;
        bool        storeCEXoutput = false;

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json config{};
            config["method"] = ec::toString(method);
            if (method == ec::Method::G_I_Gp) {
                config["strategy"] = ec::toString(strategy);
            }
            config["tolerance"]                                   = tolerance;
            config["optimizations"]                               = {};
            auto& optimizations                                   = config["optimizations"];
            optimizations["fuse consecutive single qubit gates"]  = fuseSingleQubitGates;
            optimizations["reconstruct swaps"]                    = reconstructSWAPs;
            optimizations["remove diagonal gates before measure"] = removeDiagonalGatesBeforeMeasure;
            if (method == ec::Method::Simulation) {
                config["simulation config"]               = {};
                auto& simulation                          = config["simulation config"];
                simulation["fidelity limit"]              = fidelity_limit;
                simulation["max sims"]                    = max_sims;
                simulation["stimuli type"]                = ec::toString(stimuliType);
                simulation["store counterexample input"]  = storeCEXinput;
                simulation["store counterexample output"] = storeCEXoutput;
            }
            return config;
        }
        [[nodiscard]] std::string toString() const {
            return json().dump(2);
        }
    };

    class EquivalenceChecker {
    protected:
        qc::QuantumComputation& qc1;
        qc::QuantumComputation& qc2;

        std::unique_ptr<dd::Package> dd;

        dd::QubitCount nqubits = 0;

        std::vector<bool> ancillary1{};
        std::vector<bool> ancillary2{};
        std::vector<bool> garbage1{};
        std::vector<bool> garbage2{};

        qc::Permutation initial1;
        qc::Permutation initial2;
        qc::Permutation output1;
        qc::Permutation output2;

        decltype(qc1.begin()) it1;
        decltype(qc2.begin()) it2;
        decltype(qc1.cend())  end1;
        decltype(qc1.cend())  end2;

        /// Given that one circuit has more qubits than the other, the difference is assumed to arise from ancillary qubits.
        /// This function adjusts both circuits accordingly
        static void setupAncillariesAndGarbage(qc::QuantumComputation& smaller_circuit, qc::QuantumComputation& larger_circuit);

        /// In some cases both circuits calculate the same function, but on different qubits.
        /// This function tries to correct such mismatches.
        /// Note that this is still highly experimental!
        void fixOutputPermutationMismatch(qc::QuantumComputation& circuit);

        /// Run any configured optimization passes
        virtual void runPreCheckPasses(const Configuration& config);

        /// Take operation and apply it either from the left or (inverted) from the right
        /// \param op operation to apply
        /// \param to DD to apply the operation to
        /// \param dir LEFT or RIGHT
        template<class DDType>
        void applyGate(std::unique_ptr<qc::Operation>& op, DDType& to, qc::Permutation& permutation, Direction dir = LEFT) {
            // set appropriate qubit count to generate correct DD
            auto nq = op->getNqubits();
            op->setNqubits(nqubits);

            auto saved = to;
            if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                // direction has no effect on state vector DDs
                to = dd->multiply(op->getDD(dd, permutation), to);
            } else {
                if (dir == LEFT) {
                    to = dd->multiply(op->getDD(dd, permutation), to);
                } else {
                    to = dd->multiply(to, op->getInverseDD(dd, permutation));
                }
            }
            dd->incRef(to);
            dd->decRef(saved);
            dd->garbageCollect();

            // reset qubit count
            op->setNqubits(nq);
        }
        template<class DDType>
        void applyGate(qc::QuantumComputation& qc, decltype(qc1.begin())& opIt, DDType& to, qc::Permutation& permutation, Direction dir = LEFT) {
            // Measurements at the end of the circuit are considered NOPs.
            if ((*opIt)->getType() == qc::Measure) {
                if (!qc.isLastOperationOnQubit(opIt, qc.cend())) {
                    throw std::invalid_argument("Intermediate measurements currently not supported. Defer your measurements to the end.");
                }
                return;
            }
            applyGate(*opIt, to, permutation, dir);
        }

        void setupResults(EquivalenceCheckingResults& results);

    public:
        EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2);

        virtual ~EquivalenceChecker() = default;

        // TODO: also allow equivalence by relative phase or up to a permutation of the outputs
        template<class DDType>
        static Equivalence equals(const DDType& e, const DDType& f) {
            if (e.p != f.p) return Equivalence::NotEquivalent;

            if (!e.w.approximatelyEquals(f.w)) return Equivalence::EquivalentUpToGlobalPhase;

            return Equivalence::Equivalent;
        }

        virtual EquivalenceCheckingResults check() { return check(Configuration{}); };
        virtual EquivalenceCheckingResults check(const Configuration& config);

        static void setTolerance(dd::fp tol) { decltype(dd->cn.complexTable)::setTolerance(tol); }
        Method      method = ec::Method::Reference;
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
