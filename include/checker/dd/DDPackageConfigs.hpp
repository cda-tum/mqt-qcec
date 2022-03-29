/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "dd/Package.hpp"

namespace ec {
    struct SimulationDDPackageConfig: public dd::DDPackageConfig {
        // simulation requires more resources for vectors.
        static constexpr std::size_t UT_VEC_NBUCKET            = 65536U;
        static constexpr std::size_t CT_VEC_ADD_NBUCKET        = 65536U;
        static constexpr std::size_t CT_MAT_VEC_MULT_NBUCKET   = 65536U;
        static constexpr std::size_t CT_VEC_INNER_PROD_NBUCKET = 32768U;

        // simulation only needs matrices for representing operations. Hence, very little is needed here.
        static constexpr std::size_t UT_MAT_NBUCKET                 = 128U;
        static constexpr std::size_t UT_MAT_INITIAL_ALLOCATION_SIZE = 32U;

        // simulation needs no matrix addition, conjugate transposition, matrix-matrix multiplication, or kronecker products.
        static constexpr std::size_t CT_MAT_ADD_NBUCKET        = 1U;
        static constexpr std::size_t CT_MAT_CONJ_TRANS_NBUCKET = 1U;
        static constexpr std::size_t CT_MAT_MAT_MULT_NBUCKET   = 1U;
        static constexpr std::size_t CT_VEC_KRON_NBUCKET       = 1U;
        static constexpr std::size_t CT_MAT_KRON_NBUCKET       = 1U;
    };

    using SimulationDDPackage = dd::Package<SimulationDDPackageConfig::UT_VEC_NBUCKET, SimulationDDPackageConfig::UT_VEC_INITIAL_ALLOCATION_SIZE,
                                            SimulationDDPackageConfig::UT_MAT_NBUCKET, SimulationDDPackageConfig::UT_MAT_INITIAL_ALLOCATION_SIZE,
                                            SimulationDDPackageConfig::CT_VEC_ADD_NBUCKET, SimulationDDPackageConfig::CT_MAT_ADD_NBUCKET,
                                            SimulationDDPackageConfig::CT_MAT_TRANS_NBUCKET, SimulationDDPackageConfig::CT_MAT_CONJ_TRANS_NBUCKET,
                                            SimulationDDPackageConfig::CT_MAT_VEC_MULT_NBUCKET, SimulationDDPackageConfig::CT_MAT_MAT_MULT_NBUCKET,
                                            SimulationDDPackageConfig::CT_VEC_KRON_NBUCKET, SimulationDDPackageConfig::CT_MAT_KRON_NBUCKET,
                                            SimulationDDPackageConfig::CT_VEC_INNER_PROD_NBUCKET>;

    struct ConstructionDDPackageConfig: public dd::DDPackageConfig {
        // construction requires more resources for matrices.
        static constexpr std::size_t UT_MAT_NBUCKET            = 65536U;
        static constexpr std::size_t CT_MAT_ADD_NBUCKET        = 65536U;
        static constexpr std::size_t CT_MAT_MAT_MULT_NBUCKET   = 65536U;
        static constexpr std::size_t CT_MAT_CONJ_TRANS_NBUCKET = 32768U;

        // construction does not need any vector nodes
        static constexpr std::size_t UT_VEC_NBUCKET                 = 1U;
        static constexpr std::size_t UT_VEC_INITIAL_ALLOCATION_SIZE = 1U;

        // construction needs no vector addition, matrix-vector multiplication, kronecker products, or inner products.
        static constexpr std::size_t CT_VEC_ADD_NBUCKET        = 1U;
        static constexpr std::size_t CT_MAT_VEC_MULT_NBUCKET   = 1U;
        static constexpr std::size_t CT_VEC_KRON_NBUCKET       = 1U;
        static constexpr std::size_t CT_MAT_KRON_NBUCKET       = 1U;
        static constexpr std::size_t CT_VEC_INNER_PROD_NBUCKET = 1U;
    };

    using ConstructionDDPackage = dd::Package<ConstructionDDPackageConfig::UT_VEC_NBUCKET, ConstructionDDPackageConfig::UT_VEC_INITIAL_ALLOCATION_SIZE,
                                              ConstructionDDPackageConfig::UT_MAT_NBUCKET, ConstructionDDPackageConfig::UT_MAT_INITIAL_ALLOCATION_SIZE,
                                              ConstructionDDPackageConfig::CT_VEC_ADD_NBUCKET, ConstructionDDPackageConfig::CT_MAT_ADD_NBUCKET,
                                              ConstructionDDPackageConfig::CT_MAT_TRANS_NBUCKET, ConstructionDDPackageConfig::CT_MAT_CONJ_TRANS_NBUCKET,
                                              ConstructionDDPackageConfig::CT_MAT_VEC_MULT_NBUCKET, ConstructionDDPackageConfig::CT_MAT_MAT_MULT_NBUCKET,
                                              ConstructionDDPackageConfig::CT_VEC_KRON_NBUCKET, ConstructionDDPackageConfig::CT_MAT_KRON_NBUCKET,
                                              ConstructionDDPackageConfig::CT_VEC_INNER_PROD_NBUCKET>;
} // namespace ec

struct AlternatingDDPackageConfig: public dd::DDPackageConfig {
    // The alternating checker requires more resources for matrices.
    static constexpr std::size_t UT_MAT_NBUCKET          = 65536U;
    static constexpr std::size_t CT_MAT_ADD_NBUCKET      = 65536U;
    static constexpr std::size_t CT_MAT_MAT_MULT_NBUCKET = 65536U;

    // The alternating checker does not need any vector nodes
    static constexpr std::size_t UT_VEC_NBUCKET                 = 1U;
    static constexpr std::size_t UT_VEC_INITIAL_ALLOCATION_SIZE = 1U;

    // The alternating needs no vector addition, matrix-vector multiplication, kronecker products, or inner products.
    static constexpr std::size_t CT_VEC_ADD_NBUCKET        = 1U;
    static constexpr std::size_t CT_MAT_VEC_MULT_NBUCKET   = 1U;
    static constexpr std::size_t CT_VEC_KRON_NBUCKET       = 1U;
    static constexpr std::size_t CT_MAT_KRON_NBUCKET       = 1U;
    static constexpr std::size_t CT_VEC_INNER_PROD_NBUCKET = 1U;
};

using AlternatingDDPackage = dd::Package<AlternatingDDPackageConfig::UT_VEC_NBUCKET, AlternatingDDPackageConfig::UT_VEC_INITIAL_ALLOCATION_SIZE,
                                         AlternatingDDPackageConfig::UT_MAT_NBUCKET, AlternatingDDPackageConfig::UT_MAT_INITIAL_ALLOCATION_SIZE,
                                         AlternatingDDPackageConfig::CT_VEC_ADD_NBUCKET, AlternatingDDPackageConfig::CT_MAT_ADD_NBUCKET,
                                         AlternatingDDPackageConfig::CT_MAT_TRANS_NBUCKET, AlternatingDDPackageConfig::CT_MAT_CONJ_TRANS_NBUCKET,
                                         AlternatingDDPackageConfig::CT_MAT_VEC_MULT_NBUCKET, AlternatingDDPackageConfig::CT_MAT_MAT_MULT_NBUCKET,
                                         AlternatingDDPackageConfig::CT_VEC_KRON_NBUCKET, AlternatingDDPackageConfig::CT_MAT_KRON_NBUCKET,
                                         AlternatingDDPackageConfig::CT_VEC_INNER_PROD_NBUCKET>;
