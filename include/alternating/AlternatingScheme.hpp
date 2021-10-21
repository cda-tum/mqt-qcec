/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_ALTERNATINGSCHEME_HPP
#define QCEC_ALTERNATINGSCHEME_HPP

#include <exception>
#include <iostream>

namespace ec {
    enum class AlternatingScheme {
        Naive,
        Proportional,
        Lookahead,
        CompilationFlow
    };

    inline std::string toString(const AlternatingScheme& scheme) {
        switch (scheme) {
            case AlternatingScheme::Naive:
                return "naive";
            case AlternatingScheme::Proportional:
                return "proportional";
            case AlternatingScheme::Lookahead:
                return "lookahead";
            case AlternatingScheme::CompilationFlow:
                return "compilation flow";
        }
        return " ";
    }

    inline AlternatingScheme alternatingSchemeFromString(const std::string& scheme) {
        if (scheme == "naive" || scheme == "0") {
            return AlternatingScheme::Naive;
        } else if (scheme == "proportional" || scheme == "1") {
            return AlternatingScheme::Proportional;
        } else if (scheme == "lookahead" || scheme == "2") {
            return AlternatingScheme::Lookahead;
        } else if (scheme == "compilation flow" || scheme == "3") {
            return AlternatingScheme::CompilationFlow;
        } else {
            throw std::runtime_error("Unknown alternating multiplication scheme: " + scheme);
        }
    }

    std::istream& operator>>(std::istream& in, AlternatingScheme& scheme) {
        std::string token;
        in >> token;
        scheme = alternatingSchemeFromString(token);
        return in;
    }

    std::ostream& operator<<(std::ostream& out, AlternatingScheme& scheme) {
        out << toString(scheme);
        return out;
    }

} // namespace ec
#endif //QCEC_ALTERNATINGSCHEME_HPP
