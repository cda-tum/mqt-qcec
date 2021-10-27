/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDConstructionChecker.hpp"

namespace ec {
    void DDConstructionChecker::initializeTask(TaskManager<qc::MatrixDD>& task) {
        auto initial = dd->makeIdent(nqubits);
        dd->incRef(initial);
        task.setInternalState(initial);
        task.reduceAncillae();
    }
} // namespace ec
