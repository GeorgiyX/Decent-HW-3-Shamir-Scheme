#include <stdexcept>
#include <iostream>
#include <utility>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <sstream>
#include "utils.h"

namespace HW3 {
    const char *RECOVER_STR = "recover";
    const char *SPLIT_STR = "split";
    const int SPLIT_LEFT_BORDER = 2;
    const int SPLIT_RIGHT_BORDER = 100;

    bool args2mode(int argc, char **argv) {
        if (argc <= 1 || !argv) { throw std::runtime_error("wrong arguments"); }

        bool ret = false;
        if (std::string(SPLIT_STR) == argv[1]) {
            ret = true;
        } else if (std::string(RECOVER_STR) == argv[1]) {
            ret = false;
        } else { throw std::runtime_error("unknown 2nd argument"); }

        return ret;
    }

    void runInSplitMode() {
        std::string secretString;
        size_t  minShadow = 0, totalShadow = 0;
        std::cin >> secretString;
        std::cin >> totalShadow >> minShadow;
        if (!(SPLIT_LEFT_BORDER < minShadow && minShadow <= totalShadow && totalShadow < SPLIT_RIGHT_BORDER)) {
            throw std::runtime_error("invalid number of shadows or the number of shadows required for recovery");
        }
        auto shadows = splitSecret(secretString, totalShadow, minShadow);
        for (const auto &shadow : shadows) {
            std::cout << shadow._y << std::endl;
        }
    }

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

        // todo: FIX MEM LEAKS!!!!
        return resultString; // todo: check "0x".
    }

    void runInRecoverMode() {

    }


    Shadow::Shadow(size_t x, std::string y) : _x(), _y(std::move(y)) {
        std::stringstream iss;
        iss << "0x" << std::hex << x;
        _x = iss.str();
    }
}  // namespace HW3
