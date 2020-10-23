#include <openssl/rand.h>
#include <openssl/bn.h>
#include <sstream>
#include <stdexcept>
#include "shamir.h"

namespace HW3 {
    const char *ONE_NUM = "1";
    const char *ZERO_NUM = "0";

    std::vector<Shadow> splitSecret(const std::string &secret, size_t totalShadows, size_t minShadows) {
        std::vector<int> parameters(minShadows - 1, 0);
        std::vector<Shadow> shadows;
        int rc = 0;

        /* Generating parameters of a polynomial of degree "minShadows - 1" */
        for (auto &parameter : parameters) {
            rc = RAND_bytes(reinterpret_cast<unsigned char *>(&parameter), sizeof(parameter));
            if (rc != 1) { throw std::runtime_error("RAND_bytes error"); }
        }

        /* Calculating parts of a shared secret */
        for (size_t x = 1; x <= totalShadows; x++) {
            shadows.emplace_back(x, valueOfPolynomial(x, parameters, secret));
        }

        return shadows;
    }

    std::string valueOfPolynomial(size_t x, const std::vector<int> &parameters, const std::string &constantTerm) {
        BN_CTX *ctx = BN_CTX_new();  // todo: shared ptr - BN_CTX_free
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }

        int rc = 0;
        BIGNUM *result = nullptr;
        rc = BN_hex2bn(&result, constantTerm.c_str());
        if(rc == 0) { throw  std::runtime_error("error BN_hex2bn (constantTerm)"); }

        BIGNUM *argument = nullptr;
        rc = BN_dec2bn(&argument, std::to_string(x).c_str());
        if(rc == 0) { throw  std::runtime_error("error BN_dec2bn (x)"); }


        BIGNUM *pow = nullptr, *param = nullptr;
        BIGNUM *powResult = BN_new();  /// todo: shared ptr
        if (!powResult) { throw std::runtime_error("error BN_new"); }
        BIGNUM *mulResult = BN_new();  /// todo: shared ptr
        if (!mulResult) { throw std::runtime_error("error BN_new"); }

        for (size_t i = 0; i < parameters.size(); ++i) {
            /* simple do: result += parameters[i] * std::pow(x, i + 1) */
            /* pow: */
            rc = BN_dec2bn(&pow, std::to_string(i +  1).c_str()); // free..
            if(rc == 0) { throw  std::runtime_error("error BN_dec2bn (i)"); }

            rc = BN_exp(powResult, argument, pow, ctx);
            if(rc == 0) { throw  std::runtime_error("error BN_exp"); }

            /* mul: */
            rc = BN_dec2bn(&param, std::to_string(parameters[i]).c_str());
            if(rc == 0) { throw  std::runtime_error("error BN_dec2bn (parameters[i])"); }

            rc = BN_mul(mulResult, param, powResult, ctx);
            if(rc == 0) { throw  std::runtime_error("error BN_exp"); }

            /* add: */
            rc = BN_add(result, result, mulResult);
            if(rc == 0) { throw  std::runtime_error("error BN_add"); }
        }

        char *resultCString = BN_bn2hex(result);
        if (!resultCString) { throw std::runtime_error("error BN_bn2hex"); }
        std::string resultString(resultCString);
        OPENSSL_free(resultCString);

        // todo: FIX MEM LEAKS!
        return resultString; // todo: check "0x".
    }

    std::string recoverSecret(const std::vector<Shadow> &shadows) {
        std::shared_ptr<BN_CTX> ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }

        std::shared_ptr<BIGNUM> shadowY(BN_new(), BN_free);
        if (!shadowY) { throw std::runtime_error("error BN_new"); }
        auto shadowYPtr = shadowY.get();  // get() must not return nullptr

        BIGRATIO result;
        int rc = 0;

        for (const auto &shadow : shadows) {
            /* todo: test, may be problem with using shadowY, if shadowYPtr = null, and BN_hex2bn alloc */
            rc = BN_hex2bn(&shadowYPtr, shadow._y.c_str());  // input in hex (as well as output).
            if (rc == 0) { throw std::runtime_error("error BN_hex2bn"); }

            result += getConstantTermPart(shadows, shadow._x) * shadowY;
        }

        char *resultCString = BN_bn2hex(result.intPart().get());
        if (!resultCString) { throw std::runtime_error("error BN_bn2hex"); }
        std::string resultString(resultCString);
        OPENSSL_free(resultCString);

        return resultString;
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
            if (shadow._x == shadowNum) { continue; }
            bn::dec2bn(xJ, std::to_string(shadow._x));
            /* numerator */
            bn::mul(numerator, numerator, xJ, ctx);
            /* denominator */
            bn::sub(subResult, xI, xJ);
            bn::mul(denominator, denominator, subResult, ctx);
        }

        return BIGRATIO(std::move(numerator), std::move(denominator));
    }


    Shadow::Shadow(size_t x, std::string y) : _x(x), _y(std::move(y)) {
//        std::stringstream iss;
//        iss << std::hex << x;
//        _x = iss.str();
    }

}  // namespace HW3
