#include "checker/dd/HybridSchrodingerFeynmanChecker.hpp"

#include "Definitions.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/ComplexValue.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Operations.hpp"
#include "dd/Package.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <taskflow/core/async.hpp>
#include <taskflow/core/executor.hpp>

template <class Config>
std::size_t ec::HybridSchrodingerFeynmanChecker<Config>::getNDecisions(
    qc::Qubit splitQubit, qc::QuantumComputation& qc) {
  std::size_t ndecisions = 0;
  // calculate number of decisions
  for (const auto& op : qc) {
    if (op->getType() == qc::Barrier) {
      continue;
    }
    if (op->isStandardOperation()) {
      bool targetInLowerSlice = false;
      bool targetInUpperSlice = false;
      bool controlInLowerSlice = false;
      bool controlInUpperSlice = false;
      for (const auto& target : op->getTargets()) {
        targetInLowerSlice = targetInLowerSlice || target < splitQubit;
        targetInUpperSlice = targetInUpperSlice || target >= splitQubit;
      }
      for (const auto& control : op->getControls()) {
        controlInLowerSlice = controlInLowerSlice || control.qubit < splitQubit;
        controlInUpperSlice =
            controlInUpperSlice || control.qubit >= splitQubit;
      }
      if ((targetInLowerSlice && controlInUpperSlice) ||
          (targetInUpperSlice && controlInLowerSlice)) {
        ndecisions++;
      }
    } else {
      throw std::invalid_argument(
          "Only StandardOperations are supported for now.");
    }
  }
  return ndecisions;
}

template <class Config>
dd::ComplexValue ec::HybridSchrodingerFeynmanChecker<Config>::simulateSlicing(
    std::unique_ptr<dd::Package<Config>>& sliceDD1,
    std::unique_ptr<dd::Package<Config>>& sliceDD2, unsigned int splitQubit,
    size_t controls) {
  Slice lower(sliceDD1, 0, splitQubit - 1, controls);
  Slice upper(sliceDD2, splitQubit,
              static_cast<qc::Qubit>(this->qc1->getNqubits() - 1), controls);
  for (const auto& op : *this->qc1) {
    if (op->isUnitary()) {
      [[maybe_unused]] auto l = lower.apply(sliceDD1, op);
      [[maybe_unused]] auto u = upper.apply(sliceDD2, op);
      assert(l == u);
    }
    sliceDD1->garbageCollect();
    sliceDD2->garbageCollect();
  }
  for (const auto& op : *this->qc2) {
    if (op->isUnitary()) {
      [[maybe_unused]] auto l = lower.apply(sliceDD1, op);
      [[maybe_unused]] auto u = upper.apply(sliceDD2, op);
      assert(l == u);
    }
    sliceDD1->garbageCollect();
    sliceDD2->garbageCollect();
  }
  auto traceLower = sliceDD1->trace(lower.matrix, lower.nqubits);
  auto traceUpper = sliceDD2->trace(upper.matrix, upper.nqubits);
  auto result = traceLower * traceUpper;
  return result;
}

template <class Config>
bool ec::HybridSchrodingerFeynmanChecker<Config>::Slice::apply(
    std::unique_ptr<dd::Package<Config>>& sliceDD,
    const std::unique_ptr<qc::Operation>& op) {
  bool isSplitOp = false;
  if (dynamic_cast<qc::StandardOperation*>(op.get()) !=
      nullptr) { // TODO change control and target if wrong direction
    qc::Targets opTargets{};
    qc::Controls opControls{};

    // check targets
    bool targetInSplit = false;
    bool targetInOtherSplit = false;
    for (const auto& target : op->getTargets()) {
      if (start <= target && target <= end) {
        opTargets.push_back(target - start);
        targetInSplit = true;
      } else {
        targetInOtherSplit = true;
      }
    }

    if (targetInSplit && targetInOtherSplit && !op->getControls().empty()) {
      throw std::invalid_argument("Multiple Targets that are in different "
                                  "slices are not supported at the moment");
    }

    // check controls
    for (const auto& control : op->getControls()) {
      if (start <= control.qubit && control.qubit <= end) {
        opControls.emplace(control.qubit - start, control.type);
      } else { // other controls are set to the corresponding value
        if (targetInSplit) {
          isSplitOp = true;
          const bool nextControl = getNextControl();
          if ((control.type == qc::Control::Type::Pos &&
               !nextControl) || // break if control is not activated
              (control.type == qc::Control::Type::Neg && nextControl)) {
            nDecisionsExecuted++;
            return true;
          }
        }
      }
    }

    if (targetInOtherSplit && !opControls.empty()) { // control slice for split
      if (opControls.size() > 1) {
        throw std::invalid_argument(
            "Multiple controls in control slice of operation are not supported "
            "at the moment");
      }

      isSplitOp = true;
      const bool control = getNextControl();
      for (const auto& c : opControls) {
        auto tmp = matrix;
        auto project = control != (c.type == qc::Control::Type::Neg) ? 0 : 1;
        auto projMatrix = project == 1
                              ? sliceDD->makeGateDD(dd::MEAS_ZERO_MAT, c.qubit)
                              : sliceDD->makeGateDD(dd::MEAS_ONE_MAT, c.qubit);
        matrix = sliceDD->multiply(projMatrix, matrix);
        sliceDD->incRef(matrix);
        sliceDD->decRef(tmp);
      }
    } else if (targetInSplit) { // target slice for split or operation in split
      const auto& param = op->getParameter();
      qc::StandardOperation newOp(opControls, opTargets, op->getType(), param);
      auto tmp = matrix;
      matrix = sliceDD->multiply(dd::getDD(&newOp, *sliceDD), matrix);
      sliceDD->incRef(matrix);
      sliceDD->decRef(tmp);
    }
  } else {
    throw std::invalid_argument(
        "Only StandardOperations are supported for now.");
  }
  if (isSplitOp) {
    nDecisionsExecuted++;
  }
  return isSplitOp;
}

template <class Config>
std::map<std::string, std::size_t>
ec::HybridSchrodingerFeynmanChecker<Config>::check() {
  qc::CircuitOptimizer::removeFinalMeasurements(*qc1);
  qc::CircuitOptimizer::removeFinalMeasurements(*qc2);
  this->qc2->invert();
  auto nqubits = this->qc1->getNqubits();
  auto splitQubit = static_cast<qc::Qubit>(nqubits / 2);
  approximateVerification(splitQubit);
  return {};
}

template <class Config>
void ec::HybridSchrodingerFeynmanChecker<Config>::approximateVerification(
    qc::Qubit splitQubit) {
  const auto ndecisions =
      getNDecisions(splitQubit, *qc1) + getNDecisions(splitQubit, *qc2);
  const auto maxControl = 1ULL << ndecisions;
  const auto actuallyUsedThreads = std::min<std::size_t>(maxControl, nthreads);
  const auto chunkSize = static_cast<std::size_t>(
      std::ceil(static_cast<double>(maxControl) /
                static_cast<double>(actuallyUsedThreads)));
  const auto nslicesOnOneCpu = std::min<std::size_t>(64, chunkSize);
  dd::ComplexValue trace{0.0, 0.0};
  std::mutex traceMutex;
  tf::Executor executor(actuallyUsedThreads);
  for (std::size_t control = 0; control < maxControl;
       control += nslicesOnOneCpu) {
    executor.silent_async([this, &trace, nslicesOnOneCpu, control, splitQubit,
                           maxControl, &traceMutex]() {
      for (std::size_t localControl = 0; localControl < nslicesOnOneCpu;
           localControl++) {
        const std::size_t totalControl = control + localControl;
        if (totalControl >= maxControl) {
          break;
        }
        std::unique_ptr<dd::Package<Config>> sliceDD1 =
            std::make_unique<dd::Package<Config>>(splitQubit);
        std::unique_ptr<dd::Package<Config>> sliceDD2 =
            std::make_unique<dd::Package<Config>>(this->qc1->getNqubits() -
                                                  splitQubit);
        auto result =
            simulateSlicing(sliceDD1, sliceDD2, splitQubit, totalControl);
        std::lock_guard<std::mutex> guard(traceMutex);
        trace += result;
      }
    });
  }
  executor.wait_for_all();
}

template class ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig>;
