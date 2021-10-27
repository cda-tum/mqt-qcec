/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDCONSTRUCTIONCHECKER_HPP
#define QCEC_DDCONSTRUCTIONCHECKER_HPP

#include "checker/EquivalenceChecker.hpp"

namespace ec {
    class DDConstructionChecker: public EquivalenceChecker<qc::MatrixDD> {
    protected:
        void initializeTask(TaskManager<qc::MatrixDD>& task) override;
    };
} // namespace ec

#endif //QCEC_DDCONSTRUCTIONCHECKER_HPP
