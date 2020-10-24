#ifndef HW3_SHAMIR_H
#define HW3_SHAMIR_H

#include "utils.h"

namespace HW3 {
    struct Shadow {
        /**
         * Constructs a shadow using the argument "x" in decimal
         * and "y" in hexadecimal. "y" - contains the prefix "0x"
         */
        Shadow(size_t x, std::string y);
        size_t x;       ///< @brief x = 1..100 (in dec)
        std::string y;  ///< @brief shadow (part of the secret, in hex)
        /**
         * Prints "shadow.y" with the prefix "0x" in the stream out
         * @param out std::cout
         * @param shadow
         * @return out
         */
        friend std::ostream &operator<<(std::ostream &out, Shadow &shadow);
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
    /**
     * Finds the value of the polynomial at "x" using high precision calculations.
     * @param x - argument in dec
     * @param parameters - vector of parameters (in dec) for a variable
     * @param constantTerm - value in hex
     * @return result string in hex
     */
    std::string valueOfPolynomial(size_t x, const std::vector<short> &parameters, const std::string &constantTerm);
    /**
     * Restores the secret according to Shamir's scheme. Parameter
     * verification is not required - the user enters a valid value
     * @param shadows - minimum set of shadows required for restoration
     * @return restored secret
     */
    std::string recoverSecret(const std::vector<Shadow> &shadows);
    /**
     * Constant term of the base Lagrange polynomial
     * @param shadows
     * @param shadowNum
     * @return constant term, represented by BIGRATIO
     */
    BIGRATIO getConstantTermPart(const std::vector<Shadow> &shadows, size_t shadowNum);

}  // namespace HW3

#endif //HW3_SHAMIR_H
