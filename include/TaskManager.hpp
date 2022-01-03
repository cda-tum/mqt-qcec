/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_TASKMANAGER_HPP
#define QCEC_TASKMANAGER_HPP

#include "QuantumComputation.hpp"
#include "dd/Package.hpp"

namespace ec {
    enum Direction : bool { LEFT  = true,
                            RIGHT = false };

    template<class DDType>
    class TaskManager {
    protected:
        const qc::QuantumComputation* qc = nullptr;
        qc::Permutation               permutation{};
        decltype(qc->begin())         iterator;
        decltype(qc->end())           end;
        std::unique_ptr<dd::Package>& package;
        DDType                        internalState{};
        ec::Direction                 direction = LEFT;

    public:
        explicit TaskManager(const qc::QuantumComputation& qc, std::unique_ptr<dd::Package>& package, const ec::Direction& direction = LEFT):
            qc(&qc), package(package), direction(direction) {
            permutation = qc.initialLayout;
            iterator    = qc.begin();
            end         = qc.end();
        }

        [[nodiscard]] bool finished() const { return iterator == end; }

        const std::unique_ptr<qc::Operation>& operator()() const { return *iterator; }

        DDType getInternalState() const {
            return internalState;
        }
        void setInternalState(const DDType& state) {
            internalState = state;
        }
        void flipDirection() { direction = (direction == LEFT) ? RIGHT : LEFT; }

        inline qc::MatrixDD getDD() { return (*iterator)->getDD(package, permutation); }
        inline qc::MatrixDD getInverseDD() { return (*iterator)->getInverseDD(package, permutation); }

        [[nodiscard]] const qc::QuantumComputation* getCircuit() const { return qc; }

        [[nodiscard]] const qc::Permutation& getPermutation() const { return permutation; }

        void advanceIterator() { ++iterator; }

        void applyGate(DDType& to) {
            auto saved = to;
            if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                // direction has no effect on state vector DDs
                to = package->multiply(getDD(), to);
            } else {
                if (direction == LEFT) {
                    to = package->multiply(getDD(), to);
                } else {
                    to = package->multiply(to, getInverseDD());
                }
            }
            package->incRef(to);
            package->decRef(saved);
            package->garbageCollect();
            ++iterator;
        }

        void applySwapOperations(DDType& state) {
            while (!finished() && (*iterator)->getType() == qc::SWAP) {
                applyGate(state);
            }
        }
        void applySwapOperations() { applySwapOperations(internalState); }

        void advance(DDType& state, std::size_t nops = 1) {
            // TODO: it might make sense to explore whether gate fusion improves performance
            for (std::size_t i = 0; i < nops && !finished(); ++i) {
                applyGate(state);
                applySwapOperations(state);
            }
        }
        void advance(std::size_t nops = 1) { advance(internalState, nops); }

        void finish(DDType& state) {
            while (!finished()) {
                advance(state);
            }
        }
        void finish() { finish(internalState); }

        void changePermutation(DDType& state) {
            qc::QuantumComputation::changePermutation(state, permutation, qc->outputPermutation, package, direction);
        }
        void changePermutation() { changePermutation(internalState); }

        void reduceAncillae(DDType& state) {
            if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                state = package->reduceAncillae(state, qc->ancillary, direction);
            }
        }
        void reduceAncillae() { reduceAncillae(internalState); }

        void reduceGarbage(DDType& state) {
            if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                state = package->reduceGarbage(state, qc->garbage);
            } else if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                state = package->reduceGarbage(state, qc->garbage, direction);
            }
        }
        void reduceGarbage() { reduceGarbage(internalState); }
    };
} // namespace ec

#endif //QCEC_TASKMANAGER_HPP
