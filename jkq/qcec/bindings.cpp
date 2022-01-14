/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "EquivalenceCheckingManager.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_json/pybind11_json.hpp"
#include "qiskit/QuantumCircuit.hpp"

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;

namespace ec {
    PYBIND11_MODULE(pyqcec, m) {
        m.doc() = "Python interface for the JKQ QCEC quantum circuit equivalence checking tool";

        py::enum_<ApplicationSchemeType>(m, "ApplicationScheme")
                .value("sequential", ApplicationSchemeType::Sequential)
                .value("one_to_one", ApplicationSchemeType::OneToOne)
                .value("proportional", ApplicationSchemeType::Proportional)
                .value("lookahead", ApplicationSchemeType::Lookahead)
                .value("gate_cost", ApplicationSchemeType::GateCost)
                .export_values()
                .def(py::init([](const std::string& str) -> ApplicationSchemeType { return applicationSchemeFromString(str); }));

        py::enum_<StateType>(m, "StateType")
                .value("classical", StateType::ComputationalBasis)
                .value("localquantum", StateType::Random1QBasis)
                .value("globalquantum", StateType::Stabilizer)
                .export_values()
                .def(py::init([](const std::string& str) -> StateType { return stateTypeFromString(str); }));

        py::enum_<EquivalenceCriterion>(m, "EquivalenceCriterion")
                .value("no_information", EquivalenceCriterion::NoInformation)
                .value("not_equivalent", EquivalenceCriterion::NotEquivalent)
                .value("equivalent", EquivalenceCriterion::Equivalent)
                .value("equivalent_up_to_phase", EquivalenceCriterion::EquivalentUpToPhase)
                .value("equivalent_up_to_global_phase", EquivalenceCriterion::EquivalentUpToGlobalPhase)
                .value("probably_equivalent", EquivalenceCriterion::ProbablyEquivalent)
                .export_values()
                .def(py::init([](const std::string& str) -> EquivalenceCriterion { return fromString(str); }));

#ifdef VERSION_INFO
        m.attr("__version__") = VERSION_INFO;
#else
        m.attr("__version__") = "dev";
#endif
    }
}
