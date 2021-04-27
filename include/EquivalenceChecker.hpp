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
        fp           tolerance = CN::TOLERANCE;

        // configuration options for optimizations
        bool fuseSingleQubitGates             = true;
        bool reconstructSWAPs                 = true;
        bool removeDiagonalGatesBeforeMeasure = false;

        // configuration options for PowerOfSimulation equivalence checker
        double             fidelity_limit = 0.999;
        unsigned long long max_sims       = 16;
        StimuliType        stimuliType    = ec::StimuliType::Classical;
        bool               storeCEXinput  = false;
        bool               storeCEXoutput = false;

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

        unsigned short nqubits = 0;

        std::bitset<qc::MAX_QUBITS> ancillary1{};
        std::bitset<qc::MAX_QUBITS> ancillary2{};
        std::bitset<qc::MAX_QUBITS> garbage1{};
        std::bitset<qc::MAX_QUBITS> garbage2{};

        qc::permutationMap initial1;
        qc::permutationMap initial2;
        qc::permutationMap output1;
        qc::permutationMap output2;

        decltype(qc1.begin()) it1;
        decltype(qc2.begin()) it2;
        decltype(qc1.cend())  end1;
        decltype(qc1.cend())  end2;

        std::array<short, qc::MAX_QUBITS> line{};

        /// Given that one circuit has more qubits than the other, the difference is assumed to arise from ancillary qubits.
        /// This function adjusts both circuits accordingly
        void setupAncillariesAndGarbage(qc::QuantumComputation& smaller_circuit, qc::QuantumComputation& larger_circuit);

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
        void applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, Direction dir = LEFT);
        void applyGate(decltype(qc1.begin())& opIt, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, const decltype(qc1.cend())& end, Direction dir = LEFT);

        void setupResults(EquivalenceCheckingResults& results);
        bool validInstance();

    public:
        EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2);

        virtual ~EquivalenceChecker() = default;

        // TODO: also allow equivalence by relative phase or up to a permutation of the outputs
        static Equivalence equals(const dd::Edge& e, const dd::Edge& f);

        virtual EquivalenceCheckingResults check() { return check(Configuration{}); };
        virtual EquivalenceCheckingResults check(const Configuration& config);

        static void setTolerance(fp tol) { dd::Package::setComplexNumberTolerance(tol); }
        Method      method = ec::Method::Reference;
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
