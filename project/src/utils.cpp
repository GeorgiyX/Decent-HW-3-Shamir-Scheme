#include "utils.h"
#include <stdexcept>
#include <iostream>
#include <openssl/rand.h>

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
            // todo: hex
            std::cout << shadow.y << std::endl;
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
        return std::string();
    }


}  // namespace HW3
