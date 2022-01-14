/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_APPLICATIONSCHEME_HPP
#define QCEC_APPLICATIONSCHEME_HPP

#include "QuantumComputation.hpp"
#include "TaskManager.hpp"

#include <iostream>

namespace ec {
    // A list of application schemes that implement the below interface
    enum class ApplicationSchemeType {
        OneToOne,
        Proportional,
        GateCost,
        Lookahead
    };

    inline std::string toString(const ApplicationSchemeType& applicationScheme) {
        switch (applicationScheme) {
            case ApplicationSchemeType::OneToOne:
                return "one_to_one";
            case ApplicationSchemeType::Proportional:
                return "proportional";
            case ApplicationSchemeType::GateCost:
                return "gate_cost";
            case ApplicationSchemeType::Lookahead:
                return "lookahead";
        }
        return "";
    }

    inline ApplicationSchemeType applicationSchemeFromString(const std::string& applicationScheme) {
        if (applicationScheme == "one_to_one" || applicationScheme == "0") {
            return ApplicationSchemeType::OneToOne;
        } else if (applicationScheme == "proportional" || applicationScheme == "1") {
            return ApplicationSchemeType::Proportional;
        } else if (applicationScheme == "gate_cost" || applicationScheme == "2") {
            return ApplicationSchemeType::GateCost;
        } else if (applicationScheme == "lookahead" || applicationScheme == "3") {
            return ApplicationSchemeType::Lookahead;
        } else {
            throw std::runtime_error("Unknown application scheme: " + applicationScheme);
        }
    }

    inline std::istream& operator>>(std::istream& in, ApplicationSchemeType& applicationScheme) {
        std::string token;
        in >> token;
        applicationScheme = applicationSchemeFromString(token);
        return in;
    }

    inline std::ostream& operator<<(std::ostream& out, ApplicationSchemeType& applicationScheme) {
        out << toString(applicationScheme);
        return out;
    }

    // Interface for describing an application scheme
    // Given the current state of the check (tracked by two task managers), an application scheme describes how to
    // proceed with the check, i.e., how many operations to apply from either circuit.
    template<class DDType>
    class ApplicationScheme {
    public:
        ApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2):
            taskManager1(taskManager1), taskManager2(taskManager2){};

        virtual ~ApplicationScheme() = default;

        // get how many gates from either circuit shall be applied next
        virtual std::pair<std::size_t, std::size_t> operator()() = 0;

    protected:
        TaskManager<DDType>& taskManager1;
        TaskManager<DDType>& taskManager2;
    };

} // namespace ec
#endif //QCEC_APPLICATIONSCHEME_HPP
