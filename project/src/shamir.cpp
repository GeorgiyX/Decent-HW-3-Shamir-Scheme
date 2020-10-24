#include <openssl/rand.h>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <iostream>
#include "shamir.h"

namespace HW3 {
    const char *MINUS_ONE = "-1";
    const int PARAM_BIT_LEN = 256;

    std::vector<Shadow> splitSecret(const std::string &secret, size_t totalShadows, size_t minShadows) {
        std::vector<std::shared_ptr<BIGNUM>> parameters(minShadows - 1, bn::make_bignum());
        std::vector<Shadow> shadows;
        int rc = 0;

        /* generating parameters of a polynomial of degree "minShadows - 1" */
        for (auto &parameter : parameters) {
            rc = BN_rand(parameter.get(), PARAM_BIT_LEN, 0, 0);
            if (rc != 1) { throw std::runtime_error("BN_rand error"); }
        }

        /* calculating parts of a shared secret */
        for (size_t x = 1; x <= totalShadows; x++) {
            shadows.emplace_back(x, valueOfPolynomial(x, parameters, secret));
        }

        return shadows;
    }

    std::string valueOfPolynomial(size_t x, const std::vector<std::shared_ptr<BIGNUM>> &parameters,
                                  const std::string &constantTerm) {
        static auto ctx = bn::make_ctx();
        auto result     = bn::make_bignum();
        auto argument   = bn::make_bignum();
        auto pow        = bn::make_bignum();
        auto powResult  = bn::make_bignum();
        auto mulResult  = bn::make_bignum();  // todo: re-use param

        bn::hex2bn(result, constantTerm);
        bn::dec2bn(argument, std::to_string(x));

        for (size_t i = 0; i < parameters.size(); ++i) {
            /* simple do: result += parameters[i] * std::pow(x, i + 1)
             * pow: */
            bn::dec2bn(pow, std::to_string(i + 1));
            bn::exp(powResult, argument, pow, ctx);
            /* mul: */
            bn::mul(mulResult, parameters[i], powResult, ctx);
            /* add: */
            bn::add(result, result, mulResult);
        }

        return bn::bn2hex(result);
    }

    std::string recoverSecret(const std::vector<Shadow> &shadows) {
        auto ctx = bn::make_ctx();
        auto shadowY = bn::make_bignum();
        BIGRATIO result;

        for (const auto &shadow : shadows) {
            bn::hex2bn(shadowY, shadow.y);  // input in hex (as well as output).
            result += getConstantTermPart(shadows, shadow.x) * shadowY;
        }

        return bn::bn2hex(result.intPart());
    }

    BIGRATIO getConstantTermPart(const std::vector<Shadow> &shadows, size_t shadowNum) {
        static auto ctx  = bn::make_ctx();
        auto xI          = bn::make_bignum();
        auto xJ          = bn::make_bignum();
        auto subResult   = bn::make_bignum();
        auto denominator = bn::make_bignum();
        auto numerator   = bn::make_bignum();

        bn::dec2bn(xI, std::to_string(shadowNum));
        bn::one(numerator);
        bn::one(denominator);

        for (const auto &shadow : shadows) {
            if (shadow.x == shadowNum) { continue; }
            bn::dec2bn(xJ, std::to_string(shadow.x));
            /* numerator */
            bn::mul(numerator, numerator, xJ, ctx);
            /* denominator */
            bn::sub(subResult, xI, xJ);
            bn::mul(denominator, denominator, subResult, ctx);
        }

        /* consider the sign of the numerator */
        if ((shadows.size() - 1) % 2) {
            auto minisOne = bn::make_bignum();
            bn::dec2bn(minisOne, std::string(MINUS_ONE));
            bn::mul(numerator, numerator, minisOne, ctx);
        }

        return BIGRATIO(std::move(numerator), std::move(denominator));
    }

    Shadow::Shadow(size_t x, std::string y) : x(x), y(std::move(removeHexPrefix(y))) { }

    std::ostream &operator<<(std::ostream &out, Shadow &shadow) {
        return out << appendHexPrefix(shadow.y);
    }

}  // namespace HW3
