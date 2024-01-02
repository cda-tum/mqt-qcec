#include "EquivalenceCheckingManager.hpp"
#include "algorithms/BernsteinVazirani.hpp"
#include "algorithms/QFT.hpp"
#include "algorithms/QPE.hpp"

#include <bitset>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <utility>

class DynamicCircuitTestExactQPE : public testing::TestWithParam<std::size_t> {
protected:
  std::size_t                             precision{};
  qc::fp                                  theta{};
  std::size_t                             expectedResult{};
  std::string                             expectedResultRepresentation{};
  std::unique_ptr<qc::QuantumComputation> qpe;
  std::unique_ptr<qc::QuantumComputation> iqpe;
  std::unique_ptr<dd::Package<>>          dd;
  std::ofstream                           ofs;

  ec::Configuration config{};

  void TearDown() override {}
  void SetUp() override {
    precision = GetParam();

    dd = std::make_unique<dd::Package<>>(precision + 1);

    qpe = std::make_unique<qc::QPE>(precision);

    const auto lambda = dynamic_cast<qc::QPE*>(qpe.get())->lambda;
    iqpe              = std::make_unique<qc::QPE>(lambda, precision, true);

    std::cout << "Estimating lambda = " << lambda << "π up to " << precision
              << "-bit precision.\n";

    theta = lambda / 2;

    std::cout << "Expected theta=" << theta << "\n";
    std::bitset<64> binaryExpansion{};
    dd::fp          expansion = theta * 2;
    std::size_t     index     = 0;
    while (std::abs(expansion) > 1e-8) {
      if (expansion >= 1.) {
        binaryExpansion.set(index);
        expansion -= 1.0;
      }
      index++;
      expansion *= 2;
    }

    expectedResult = 0ULL;
    for (std::size_t i = 0; i < precision; ++i) {
      if (binaryExpansion.test(i)) {
        expectedResult |= (1ULL << (precision - 1 - i));
      }
    }
    std::stringstream ss{};
    for (auto i = static_cast<int>(precision - 1); i >= 0; --i) {
      if ((expectedResult & (1ULL << i)) != 0) {
        ss << 1;
      } else {
        ss << 0;
      }
    }
    expectedResultRepresentation = ss.str();

    std::cout << "Theta is exactly representable using " << precision
              << " bits.\n";
    std::cout << "The expected output state is |"
              << expectedResultRepresentation << ">.\n";

    config.optimizations.transformDynamicCircuit        = true;
    config.optimizations.backpropagateOutputPermutation = true;
  }
};

INSTANTIATE_TEST_SUITE_P(
    Eval, DynamicCircuitTestExactQPE, testing::Range<std::size_t>(1U, 64U, 5U),
    [](const testing::TestParamInfo<DynamicCircuitTestExactQPE::ParamType>&
           inf) {
      const auto        nqubits = inf.param;
      std::stringstream ss{};
      ss << nqubits;
      if (nqubits == 1) {
        ss << "_qubit";
      } else {
        ss << "_qubits";
      }
      return ss.str();
    });

TEST_P(DynamicCircuitTestExactQPE, UnitaryEquivalence) {
  ec::EquivalenceCheckingManager ecm(*qpe, *iqpe, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

class DynamicCircuitTestInexactQPE
    : public testing::TestWithParam<std::size_t> {
protected:
  std::size_t                             precision{};
  dd::fp                                  theta{};
  std::size_t                             expectedResult{};
  std::string                             expectedResultRepresentation{};
  std::size_t                             secondExpectedResult{};
  std::string                             secondExpectedResultRepresentation{};
  std::unique_ptr<qc::QuantumComputation> qpe;
  std::unique_ptr<qc::QuantumComputation> iqpe;
  std::unique_ptr<dd::Package<>>          dd;
  std::ofstream                           ofs;

  ec::Configuration config{};

  void TearDown() override {}
  void SetUp() override {
    precision = GetParam();

    dd = std::make_unique<dd::Package<>>(precision + 1);

    qpe = std::make_unique<qc::QPE>(precision, false);

    const auto lambda = dynamic_cast<qc::QPE*>(qpe.get())->lambda;
    iqpe              = std::make_unique<qc::QPE>(lambda, precision, true);

    std::cout << "Estimating lambda = " << lambda << "π up to " << precision
              << "-bit precision.\n";

    theta = lambda / 2;

    std::cout << "Expected theta=" << theta << "\n";
    std::bitset<64> binaryExpansion{};
    dd::fp          expansion = theta * 2;
    std::size_t     index     = 0;
    while (std::abs(expansion) > 1e-8) {
      if (expansion >= 1.) {
        binaryExpansion.set(index);
        expansion -= 1.0;
      }
      index++;
      expansion *= 2;
    }

    expectedResult = 0ULL;
    for (std::size_t i = 0; i < precision; ++i) {
      if (binaryExpansion.test(i)) {
        expectedResult |= (1ULL << (precision - 1 - i));
      }
    }
    std::stringstream ss{};
    for (auto i = precision; i > 0; --i) {
      if ((expectedResult & (1ULL << (i - 1))) != 0) {
        ss << 1;
      } else {
        ss << 0;
      }
    }
    expectedResultRepresentation = ss.str();

    secondExpectedResult = expectedResult + 1;
    ss.str("");
    for (auto i = precision; i > 0; --i) {
      if ((secondExpectedResult & (1ULL << (i - 1))) != 0) {
        ss << 1;
      } else {
        ss << 0;
      }
    }
    secondExpectedResultRepresentation = ss.str();

    std::cout << "Theta is not exactly representable using " << precision
              << " bits.\n";
    std::cout << "Most probable output states are |"
              << expectedResultRepresentation << "> and |"
              << secondExpectedResultRepresentation << ">.\n";

    config.optimizations.transformDynamicCircuit        = true;
    config.optimizations.backpropagateOutputPermutation = true;
  }
};

INSTANTIATE_TEST_SUITE_P(Eval, DynamicCircuitTestInexactQPE,
                         testing::Range<std::size_t>(1U, 15U, 3U),
                         [](const testing::TestParamInfo<
                             DynamicCircuitTestInexactQPE::ParamType>& inf) {
                           const auto        nqubits = inf.param;
                           std::stringstream ss{};
                           ss << nqubits;
                           if (nqubits == 1) {
                             ss << "_qubit";
                           } else {
                             ss << "_qubits";
                           }
                           return ss.str();
                         });

TEST_P(DynamicCircuitTestInexactQPE, UnitaryEquivalence) {
  ec::EquivalenceCheckingManager ecm(*qpe, *iqpe, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

class DynamicCircuitTestBV : public testing::TestWithParam<std::size_t> {
protected:
  std::size_t                             bitwidth{};
  std::unique_ptr<qc::QuantumComputation> bv;
  std::unique_ptr<qc::QuantumComputation> dbv;
  std::unique_ptr<dd::Package<>>          dd;
  std::ofstream                           ofs;

  ec::Configuration config{};

  void TearDown() override {}
  void SetUp() override {
    bitwidth = GetParam();

    dd = std::make_unique<dd::Package<>>(bitwidth + 1);

    bv = std::make_unique<qc::BernsteinVazirani>(bitwidth);

    const auto s = dynamic_cast<qc::BernsteinVazirani*>(bv.get())->s;
    dbv          = std::make_unique<qc::BernsteinVazirani>(s, bitwidth, true);

    const auto expected =
        dynamic_cast<qc::BernsteinVazirani*>(bv.get())->expected;
    std::cout << "Hidden bitstring: " << expected << " (" << bitwidth
              << " qubits)\n";

    config.optimizations.transformDynamicCircuit        = true;
    config.optimizations.backpropagateOutputPermutation = true;
  }
};

INSTANTIATE_TEST_SUITE_P(
    Eval, DynamicCircuitTestBV, testing::Range<std::size_t>(1U, 64U, 5U),
    [](const testing::TestParamInfo<DynamicCircuitTestBV::ParamType>& inf) {
      const auto        nqubits = inf.param;
      std::stringstream ss{};
      ss << nqubits;
      if (nqubits == 1) {
        ss << "_qubit";
      } else {
        ss << "_qubits";
      }
      return ss.str();
    });

TEST_P(DynamicCircuitTestBV, UnitaryEquivalence) {
  ec::EquivalenceCheckingManager ecm(*bv, *dbv, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

class DynamicCircuitTestQFT : public testing::TestWithParam<std::size_t> {
protected:
  std::size_t                             precision{};
  std::unique_ptr<qc::QuantumComputation> qft;
  std::unique_ptr<qc::QuantumComputation> dqft;
  std::unique_ptr<dd::Package<>>          dd;
  std::ofstream                           ofs;

  ec::Configuration config{};

  void TearDown() override {}
  void SetUp() override {
    precision = GetParam();

    dd = std::make_unique<dd::Package<>>(precision);

    qft = std::make_unique<qc::QFT>(precision);

    dqft = std::make_unique<qc::QFT>(precision, true, true);

    config.optimizations.transformDynamicCircuit        = true;
    config.optimizations.backpropagateOutputPermutation = true;
  }
};

INSTANTIATE_TEST_SUITE_P(
    Eval, DynamicCircuitTestQFT, testing::Range<std::size_t>(1U, 65U, 5U),
    [](const testing::TestParamInfo<DynamicCircuitTestQFT::ParamType>& inf) {
      const auto        nqubits = inf.param;
      std::stringstream ss{};
      ss << nqubits;
      if (nqubits == 1) {
        ss << "_qubit";
      } else {
        ss << "_qubits";
      }
      return ss.str();
    });

TEST_P(DynamicCircuitTestQFT, UnitaryEquivalence) {
  ec::EquivalenceCheckingManager ecm(*qft, *dqft, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}
