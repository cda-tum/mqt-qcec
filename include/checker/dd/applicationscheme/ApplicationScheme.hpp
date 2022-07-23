//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "QuantumComputation.hpp"
#include "checker/dd/TaskManager.hpp"

#include <iostream>
#include <stdexcept>

namespace ec {
// A list of application schemes that implement the below interface
enum class ApplicationSchemeType {
  Sequential   = 0,
  OneToOne     = 1,
  Lookahead    = 2,
  GateCost     = 3,
  Proportional = 4
};

inline std::string
toString(const ApplicationSchemeType& applicationScheme) noexcept {
  switch (applicationScheme) {
  case ApplicationSchemeType::Sequential:
    return "sequential";
  case ApplicationSchemeType::OneToOne:
    return "one_to_one";
  case ApplicationSchemeType::Lookahead:
    return "lookahead";
  case ApplicationSchemeType::GateCost:
    return "gate_cost";
  default:
    return "proportional";
  }
}

inline ApplicationSchemeType
applicationSchemeFromString(const std::string& applicationScheme) noexcept {
  if ((applicationScheme == "sequential") || (applicationScheme == "0") ||
      (applicationScheme == "reference")) {
    return ApplicationSchemeType::Sequential;
  }
  if ((applicationScheme == "one_to_one") || (applicationScheme == "1") ||
      (applicationScheme == "naive")) {
    return ApplicationSchemeType::OneToOne;
  }
  if ((applicationScheme == "lookahead") || (applicationScheme == "2")) {
    return ApplicationSchemeType::Lookahead;
  }
  if ((applicationScheme == "gate_cost") || (applicationScheme == "3") ||
      (applicationScheme == "compilation_flow")) {
    return ApplicationSchemeType::GateCost;
  }
  if ((applicationScheme == "proportional") || (applicationScheme == "4")) {
    return ApplicationSchemeType::Proportional;
  }
  std::cerr << "Unknown application scheme: " << applicationScheme
            << ". Defaulting to proportional!\n";
  return ApplicationSchemeType::Proportional;
}

inline std::istream& operator>>(std::istream&          in,
                                ApplicationSchemeType& applicationScheme) {
  std::string token;
  in >> token;

  if (token.empty()) {
    in.setstate(std::istream::failbit);
    return in;
  }

  applicationScheme = applicationSchemeFromString(token);
  return in;
}

inline std::ostream&
operator<<(std::ostream& out, const ApplicationSchemeType& applicationScheme) {
  out << toString(applicationScheme);
  return out;
}

// Interface for describing an application scheme
// Given the current state of the check (tracked by two task managers), an
// application scheme describes how to proceed with the check, i.e., how many
// operations to apply from either circuit.
template <class DDType, class DDPackage = dd::Package<>>
class ApplicationScheme {
public:
  ApplicationScheme(TaskManager<DDType, DDPackage>& taskManager1,
                    TaskManager<DDType, DDPackage>& taskManager2) noexcept
      : taskManager1(taskManager1), taskManager2(taskManager2){};

  virtual ~ApplicationScheme() = default;

  // get how many gates from either circuit shall be applied next
  virtual std::pair<std::size_t, std::size_t> operator()() = 0;

protected:
  TaskManager<DDType, DDPackage>& taskManager1;
  TaskManager<DDType, DDPackage>& taskManager2;
};

} // namespace ec
