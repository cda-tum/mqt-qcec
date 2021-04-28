/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "SimulationBasedEquivalenceChecker.hpp"

namespace ec {

    bool SimulationBasedEquivalenceChecker::simulateWithStimulus(const qc::VectorDD& stimulus, EquivalenceCheckingResults& results, const Configuration& config) {
        auto map = initial1;
        auto e   = stimulus;
        dd->incRef(e);
        it1 = qc1.begin();

        while (it1 != end1) {
            applyGate(qc1, it1, e, map);
            ++it1;
        }
        // correct permutation if necessary
        qc::QuantumComputation::changePermutation(e, map, output1, dd);
        e = dd->reduceGarbage(e, garbage1);

        map    = initial2;
        auto f = stimulus;
        dd->incRef(f);
        it2 = qc2.begin();

        while (it2 != end2) {
            applyGate(qc2, it2, f, map);
            ++it2;
        }

        // correct permutation if necessary
        qc::QuantumComputation::changePermutation(f, map, output2, dd);
        f = dd->reduceGarbage(f, garbage2);

        results.fidelity = dd->fidelity(e, f);

        results.nsims++;

        if (results.fidelity < config.fidelity_limit) {
            results.equivalence = ec::Equivalence::NotEquivalent;
            dd->decRef(e);
            dd->decRef(f);

            if (config.storeCEXinput) {
                results.cexInput = dd->getVector(stimulus);
            }
            if (config.storeCEXoutput) {
                results.circuit1.cexOutput = dd->getVector(e);
                results.circuit2.cexOutput = dd->getVector(f);
            }

            dd->garbageCollect();
            return true;
        } else if (results.nsims == static_cast<std::size_t>(std::pow(2.L, nqubits_for_stimuli))) {
            results.equivalence = ec::Equivalence::Equivalent;
            dd->decRef(e);
            dd->decRef(f);
            dd->garbageCollect();
            return true;
        } else {
            results.equivalence = ec::Equivalence::ProbablyEquivalent;
            dd->decRef(e);
            dd->decRef(f);
            dd->garbageCollect();
            return false;
        }
    }

    qc::VectorDD SimulationBasedEquivalenceChecker::generateRandomClassicalStimulus() {
        // generate distinct stimulus
        auto [new_stimulus, success] = stimuli.insert(stimuliGenerator());
        while (!success) {
            std::tie(new_stimulus, success) = stimuli.insert(stimuliGenerator());
        }
        std::vector<bool> stimulusBits(nqubits_for_stimuli);
        for (int i = 0; i < nqubits_for_stimuli; ++i) {
            if (*new_stimulus & (1 << i)) {
                stimulusBits[i] = true;
            }
        }
        auto in = dd->makeBasisState(nqubits, stimulusBits);
        return in;
    }

    qc::VectorDD SimulationBasedEquivalenceChecker::generateRandomLocalQuantumStimulus() {
        auto stimulus = std::vector<dd::BasisStates>(nqubits, dd::BasisStates::zero);
        for (int i = 0; i < nqubits_for_stimuli; ++i) {
            switch (basisStateGenerator()) {
                case 0:
                    stimulus.at(i) = dd::BasisStates::zero;
                    break;
                case 1:
                    stimulus.at(i) = dd::BasisStates::one;
                    break;
                case 2:
                    stimulus.at(i) = dd::BasisStates::plus;
                    break;
                case 3:
                    stimulus.at(i) = dd::BasisStates::minus;
                    break;
                case 4:
                    stimulus.at(i) = dd::BasisStates::right;
                    break;
                case 5:
                    stimulus.at(i) = dd::BasisStates::left;
                    break;
                default:
                    stimulus.at(i) = dd::BasisStates::zero;
            }
        }
        auto in = dd->makeBasisState(nqubits, stimulus);
        return in;
    }

    qc::VectorDD SimulationBasedEquivalenceChecker::generateRandomGlobalQuantumStimulus() {
        auto rcs        = qc::RandomCliffordCircuit(nqubits_for_stimuli, static_cast<unsigned int>(std::round(std::log2(nqubits_for_stimuli))), stimuliGenerator());
        auto stabilizer = rcs.simulate(dd->makeZeroState(nqubits_for_stimuli), dd);
        dd->decRef(stabilizer); // decrease right after so the reference count stays correct later on

        std::array<qc::VectorDD, 2> edges{};
        edges[1] = qc::VectorDD::zero;

        auto initial = stabilizer;
        for (std::size_t p = 0; p < static_cast<std::size_t>(nqubits - nqubits_for_stimuli); p++) {
            edges[0] = initial;
            initial  = dd->makeDDNode(static_cast<dd::Qubit>(p + nqubits_for_stimuli), edges);
        }
        return initial;
    }

    qc::VectorDD SimulationBasedEquivalenceChecker::generateRandomStimulus(StimuliType type) {
        switch (type) {
            case ec::StimuliType::Classical:
                return generateRandomClassicalStimulus();
            case ec::StimuliType::LocalQuantum:
                return generateRandomLocalQuantumStimulus();
            case ec::StimuliType::GlobalQuantum:
                return generateRandomGlobalQuantumStimulus();
        }
        return qc::VectorDD::zero;
    }

    EquivalenceCheckingResults SimulationBasedEquivalenceChecker::check(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.stimuliType = config.stimuliType;

        auto start = std::chrono::steady_clock::now();
        runPreCheckPasses(config);
        auto endPreprocessing = std::chrono::steady_clock::now();

        bool done = false;
        while (!done && results.nsims < config.max_sims) {
            auto stimulus = generateRandomStimulus(config.stimuliType);
            dd->incRef(stimulus);
            done = simulateWithStimulus(stimulus, results, config);
            dd->decRef(stimulus);
        }

        auto                          endVerification   = std::chrono::steady_clock::now();
        std::chrono::duration<double> preprocessingTime = endPreprocessing - start;
        std::chrono::duration<double> verificationTime  = endVerification - endPreprocessing;
        results.preprocessingTime                       = preprocessingTime.count();
        results.verificationTime                        = verificationTime.count();
        results.maxActive                               = std::max(results.maxActive, dd->vUniqueTable.getMaxActiveNodes());

        return results;
    }

    void SimulationBasedEquivalenceChecker::checkWithStimulus(const qc::VectorDD& stimulus, EquivalenceCheckingResults& results, const Configuration& config) {
        auto start = std::chrono::steady_clock::now();
        runPreCheckPasses(config);
        auto endPreprocessing = std::chrono::steady_clock::now();

        dd->incRef(stimulus);
        simulateWithStimulus(stimulus, results, config);
        dd->decRef(stimulus);

        auto                          endVerification   = std::chrono::steady_clock::now();
        std::chrono::duration<double> preprocessingTime = endPreprocessing - start;
        std::chrono::duration<double> verificationTime  = endVerification - endPreprocessing;
        results.preprocessingTime += preprocessingTime.count();
        results.verificationTime += verificationTime.count();
        results.maxActive = std::max(results.maxActive, dd->vUniqueTable.getMaxActiveNodes());
    }

    EquivalenceCheckingResults SimulationBasedEquivalenceChecker::checkZeroState(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.stimuliType = ec::StimuliType::Classical;

        auto stimulus = dd->makeZeroState(nqubits);
        checkWithStimulus(stimulus, results, config);
        return results;
    }

    EquivalenceCheckingResults SimulationBasedEquivalenceChecker::checkPlusState(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.stimuliType = ec::StimuliType::LocalQuantum;

        auto state = std::vector<dd::BasisStates>(nqubits, dd::BasisStates::zero);
        for (int i = 0; i < nqubits_for_stimuli; ++i) {
            state.at(i) = dd::BasisStates::plus;
        }
        auto stimulus = dd->makeBasisState(nqubits, state);
        checkWithStimulus(stimulus, results, config);
        return results;
    }
} // namespace ec
