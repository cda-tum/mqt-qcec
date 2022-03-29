/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "QuantumComputation.hpp"
#include "dd/Operations.hpp"

namespace ec {
    enum Direction : bool { Left  = true,
                            Right = false };

    template<class DDType, class DDPackage = dd::Package<>>
    class TaskManager {
    protected:
        const qc::QuantumComputation* qc;
        qc::Permutation               permutation{};
        decltype(qc->begin())         iterator;
        decltype(qc->end())           end;
        std::unique_ptr<DDPackage>&   package;
        DDType                        internalState{};
        ec::Direction                 direction = Left;

    public:
        explicit TaskManager(const qc::QuantumComputation& qc, std::unique_ptr<DDPackage>& package, const ec::Direction& direction = Left) noexcept:
            qc(&qc), package(package), direction(direction) {
            permutation = qc.initialLayout;
            iterator    = qc.begin();
            end         = qc.end();
        }

        [[nodiscard]] bool finished() const noexcept { return iterator == end; }

        const std::unique_ptr<qc::Operation>& operator()() const { return *iterator; }

        [[nodiscard]] const DDType& getInternalState() const noexcept {
            return internalState;
        }
        void setInternalState(const DDType& state) noexcept {
            internalState = state;
        }
        void flipDirection() noexcept { direction = (direction == Left) ? Right : Left; }

        [[nodiscard]] inline qc::MatrixDD getDD() { return dd::getDD((*iterator).get(), package, permutation); }
        [[nodiscard]] inline qc::MatrixDD getInverseDD() { return dd::getInverseDD((*iterator).get(), package, permutation); }

        [[nodiscard]] const qc::QuantumComputation* getCircuit() const noexcept { return qc; }

        [[nodiscard]] const qc::Permutation& getPermutation() const noexcept { return permutation; }

        void advanceIterator() { ++iterator; }

        void applyGate(DDType& to) {
            auto saved = to;
            if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                // direction has no effect on state vector DDs
                to = package->multiply(getDD(), to);
            } else {
                if (direction == Left) {
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

        void advance(DDType& state, std::size_t steps = 1U) {
            // TODO: it might make sense to explore whether gate fusion improves performance
            for (std::size_t i = 0U; i < steps && !finished(); ++i) {
                applyGate(state);
                applySwapOperations(state);
            }
        }
        void advance(std::size_t steps = 1U) { advance(internalState, steps); }

        void finish(DDType& state) {
            while (!finished()) {
                advance(state);
            }
        }
        void finish() { finish(internalState); }

        void changePermutation(DDType& state) {
            dd::changePermutation(state, permutation, qc->outputPermutation, package, direction);
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

        void incRef(DDType& state) {
            package->incRef(state);
        }
        void incRef() { incRef(internalState); }

        void decRef(DDType& state) {
            package->decRef(state);
        }
        void decRef() { decRef(internalState); }
    };
} // namespace ec
