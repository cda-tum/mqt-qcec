/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_COSTFUNCTION_HPP
#define QCEC_COSTFUNCTION_HPP

#include "QuantumComputation.hpp"

// Abstract base class for describing a cost function
class CostFunction {
public:
    enum Type {
        Naive,
        Proportional,
        Compilationflow
    };

    CostFunction(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Type& type):
        qc1(qc1), qc2(qc2), type(type) {
        // compute ratio of gates between both circuits
        const auto size1 = qc1.size();
        const auto size2 = qc2.size();
        firstIsLarger    = size1 > size2;
        const auto max   = firstIsLarger ? size1 : size2;
        const auto min   = firstIsLarger ? size2 : size1;

        gateRatio = std::max((max + min / 2U) / min, static_cast<std::size_t>(1U));
    };

    std::size_t operator()(const std::unique_ptr<qc::Operation>& op, const bool firstCircuit = true) {
        switch (type) {
            case Proportional:
                // return 1 for the smaller circuit and the gate ratio otherwise
                return (firstCircuit ^ firstIsLarger) ? 1U : gateRatio;
            case Compilationflow:
                return IBMCostFunction(op->getType(), op->getNcontrols());
            case Naive:
            default:
                // just return 1 in any case
                return 1U;
        }
    };

protected:
    const qc::QuantumComputation& qc1;
    const qc::QuantumComputation& qc2;

    Type type;

    // parameters for the proportional cost function
    std::size_t gateRatio     = 1U;
    bool        firstIsLarger = false;

    // compilation flow
    unsigned long long IBMCostFunction(const qc::OpType& gate, unsigned short nc) {
        switch (gate) {
            case qc::I:
                return 1;

            case qc::X:
                if (nc <= 1)
                    return 1;
                else {
                    return 2 * (nc - 2) * (2 * IBMCostFunction(qc::Phase, 0) + 2 * IBMCostFunction(qc::U2, 0) + 3 * IBMCostFunction(qc::X, 1)) + 6 * IBMCostFunction(qc::X, 1) + 8 * IBMCostFunction(qc::U3, 0);
                }
                //if (nc == 3) return 2 * (4 * IBMCostFunction(qc::U1, 1) + 3 * IBMCostFunction(qc::X, 1)) + IBMCostFunction(qc::X, 2);
                //7 * IBMCostFunction(qc::U1, 1) + 14 * IBMCostFunction(qc::H, 0) + 6 * IBMCostFunction(qc::X, 1);
                //if (nc == 4) return
                //2 * IBMCostFunction(qc::U1, 1) + 4 * IBMCostFunction(qc::H, 0) + 3 * IBMCostFunction(qc::X, 3);
                //else {
                //	int n = std::ceil(nc / 2.);
                //	return 2 * IBMCostFunction(qc::X, n+1) + 2 * IBMCostFunction(qc::X, nc-n+1);
                //}

            case qc::U3:
            case qc::U2:
            case qc::V:
            case qc::Vdag:
            case qc::RX:
            case qc::RY:
            case qc::H:
            case qc::SX:
            case qc::SXdag:
                if (nc == 0) return 1;
                if (nc == 1)
                    return 2 * IBMCostFunction(qc::X, 1) + 4 * IBMCostFunction(qc::U3, 0);
                else
                    return 2 * IBMCostFunction(qc::X, nc) + 4 * IBMCostFunction(qc::U3, 0); // heuristic

            case qc::Phase:
            case qc::S:
            case qc::Sdag:
            case qc::T:
            case qc::Tdag:
            case qc::RZ:
                if (nc == 0) return 1;
                if (nc == 1)
                    return 2 * IBMCostFunction(qc::X, 1) + 3 * IBMCostFunction(qc::Phase, 0);
                else
                    return 2 * IBMCostFunction(qc::X, nc) + 3 * IBMCostFunction(qc::U3, 0); // heuristic

            case qc::Y:
            case qc::Z:
                if (nc == 0)
                    return 1;
                else
                    return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::U3, 0);

            case qc::SWAP:
                return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::X, 1);

            case qc::iSWAP:
                return IBMCostFunction(qc::SWAP, nc) + 2 * IBMCostFunction(qc::S, nc - 1) + IBMCostFunction(qc::Z, nc);

            case qc::Peres:
            case qc::Peresdag:
                return IBMCostFunction(qc::X, nc) + IBMCostFunction(qc::X, nc - 1);

            case qc::Compound: // this assumes that compound operations only arise from single qubit fusion
            case qc::Measure:
            case qc::Barrier:
            case qc::ShowProbabilities:
            case qc::Snapshot:
                return 1; // these operations are assumed to incur no cost, but to advance the procedure 1 is used

            default:
                std::cerr << "No cost for gate " << std::to_string(gate) << " -> assuming cost of 1!" << std::endl;
                return 1;
        }
    }
};

#endif //QCEC_COSTFUNCTION_HPP
