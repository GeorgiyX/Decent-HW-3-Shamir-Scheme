#ifndef HW_3_UTILS_H
#define HW_3_UTILS_H

#include <string>
#include <utility>
#include <vector>

namespace HW3 {
    /**
     * Parses startup arguments to determine the program's operating mode
     * @param argc - argument count
     * @param argv - argument values
     * @return true - split mode; false - recover mode
     * @throw std::runtime_error - wrong args
     */
    bool args2mode(int argc, char **argv);
    /**
     * Running the program in secret sharing mode
     */
    void runInSplitMode();
    /**
     * Running the program in secret recovery mode
     */
    void runInRecoverMode();

    struct Shadow {
        Shadow(size_t x, std::string y) : x(x), y(std::move(y)) {}
        size_t x;       ///< @brief x = 1..100;
        std::string y;  ///< @brief shadow (part of the secret)
    };
    /**
     * Divides the secret "secret" according to the Shamir scheme into "totalShadows" parts
     * with the number of parts needed to restore "minShadows"
     * @param secret - secret sting
     * @param totalShadows
     * @param minShadows
     * @return shadow vector
     */
    std::vector<Shadow> splitSecret(const std::string &secret, size_t totalShadows, size_t minShadows);
    std::string valueOfPolynomial(size_t x, const std::vector<int> &parameters, const std::string &constantTerm);

}  // namespace hw3

#endif  // HW_3_UTILS_H
