/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDConstructionChecker.hpp"

namespace ec {
    void DDConstructionChecker::initializeTask(TaskManager<qc::MatrixDD>& task) {
        const auto initial = dd->makeIdent(nqubits);
        task.setInternalState(initial);
        task.incRef();
        task.reduceAncillae();
    }
} // namespace ec
