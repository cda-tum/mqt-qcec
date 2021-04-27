/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "SimulationBasedEquivalenceChecker.hpp"

namespace ec {

    bool SimulationBasedEquivalenceChecker::simulateWithStimulus(const dd::Edge& stimulus, EquivalenceCheckingResults& results, const Configuration& config) {
        line.fill(qc::LINE_DEFAULT);
        auto map = initial1;
        auto e   = stimulus;
        dd->incRef(e);
        it1 = qc1.begin();

        while (it1 != end1) {
            applyGate(it1, e, map, end1);
            ++it1;
        }
        // correct permutation if necessary
        qc::QuantumComputation::changePermutation(e, map, output1, line, dd);
        e = dd->reduceGarbage(e, garbage1);
        e = dd->reduceAncillae(e, ancillary1);

        map    = initial2;
        auto f = stimulus;
        dd->incRef(f);
        it2 = qc2.begin();

        while (it2 != end2) {
            applyGate(it2, f, map, end2);
            ++it2;
        }

        // correct permutation if necessary
        qc::QuantumComputation::changePermutation(f, map, output2, line, dd);
        f = dd->reduceGarbage(f, garbage2);
        f = dd->reduceAncillae(f, ancillary2);

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

            dd->garbageCollect(true);
            return true;
        } else if (results.nsims == static_cast<unsigned long long>(std::pow(2.L, nqubits_for_stimuli))) {
            results.equivalence = ec::Equivalence::Equivalent;
            dd->decRef(e);
            dd->decRef(f);
            dd->garbageCollect(true);
            return true;
        } else {
            results.equivalence = ec::Equivalence::ProbablyEquivalent;
            dd->decRef(e);
            dd->decRef(f);
            dd->garbageCollect(true);
            return false;
        }
    }

    dd::Edge SimulationBasedEquivalenceChecker::generateRandomClassicalStimulus() {
        // generate distinct stimulus
        auto new_stimulus = stimuli.insert(stimuliGenerator());
        while (!new_stimulus.second) {
            new_stimulus = stimuli.insert(stimuliGenerator());
        }
        std::bitset<dd::MAXN> stimulusBits(*new_stimulus.first);

        auto in = dd->makeBasisState(nqubits, stimulusBits);
        return in;
    }

    dd::Edge SimulationBasedEquivalenceChecker::generateRandomLocalQuantumStimulus() {
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

    dd::Edge SimulationBasedEquivalenceChecker::generateRandomGlobalQuantumStimulus() {
        auto rcs        = qc::RandomCliffordCircuit(nqubits_for_stimuli, static_cast<unsigned int>(std::round(std::log2(nqubits_for_stimuli))), stimuliGenerator());
        auto stabilizer = rcs.simulate(dd->makeZeroState(nqubits_for_stimuli), dd);
        dd->decRef(stabilizer); // decrease right after so the reference count stays correct later on

        dd::Edge edges[4];
        edges[1] = edges[2] = edges[3] = dd->DDzero;

        auto initial = stabilizer;
        for (short p = 0; p < static_cast<short>(nqubits - nqubits_for_stimuli); p++) {
            edges[0] = initial;
            initial  = dd->makeNonterminal(static_cast<short>(p + nqubits_for_stimuli), edges);
        }
        return initial;
    }

    dd::Edge SimulationBasedEquivalenceChecker::generateRandomStimulus(StimuliType type) {
        switch (type) {
            case ec::StimuliType::Classical:
                return generateRandomClassicalStimulus();
            case ec::StimuliType::LocalQuantum:
                return generateRandomLocalQuantumStimulus();
            case ec::StimuliType::GlobalQuantum:
                return generateRandomGlobalQuantumStimulus();
        }
        return dd::Package::DDzero;
    }

    EquivalenceCheckingResults SimulationBasedEquivalenceChecker::check(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.stimuliType = config.stimuliType;

        if (!validInstance()) {
            return results;
        }

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
        results.maxActive                               = std::max(results.maxActive, dd->maxActive);

        return results;
    }

    void SimulationBasedEquivalenceChecker::checkWithStimulus(const dd::Edge& stimulus, EquivalenceCheckingResults& results, const Configuration& config) {
        if (!validInstance()) {
            return;
        }

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
        results.maxActive = std::max(results.maxActive, dd->maxActive);
    }

    EquivalenceCheckingResults SimulationBasedEquivalenceChecker::checkZeroState(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.stimuliType = ec::StimuliType::Classical;

        std::bitset<qc::MAX_QUBITS> stimulusBits(0ULL);
        auto                        stimulus = dd->makeBasisState(nqubits, stimulusBits);
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
