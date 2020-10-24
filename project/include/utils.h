#ifndef HW_3_UTILS_H
#define HW_3_UTILS_H

#include <string>
#include <utility>
#include <vector>
#include <openssl/bn.h>
#include <memory>
#include <functional>

namespace HW3 {
    extern const char *HEX_PREFIX;

    namespace bn {
        std::shared_ptr<BIGNUM> make_bignum();
        std::shared_ptr<BN_CTX> make_ctx();
        void dec2bn(std::shared_ptr<BIGNUM> &bn, const std::string &numString);
        void hex2bn(std::shared_ptr<BIGNUM> &bn, const std::string &numString);
        void mul(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &b, std::shared_ptr<BN_CTX> &ctx);
        void sub(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a, const std::shared_ptr<BIGNUM> &b);
        void one(std::shared_ptr<BIGNUM> &a);
        std::string bn2hex(const std::shared_ptr<BIGNUM> &a);
        void exp(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &p, std::shared_ptr<BN_CTX> &ctx);
        void add(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a, const std::shared_ptr<BIGNUM> &b);
        void div(std::shared_ptr<BIGNUM> &dv, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &d, std::shared_ptr<BN_CTX> &ctx);
    }  // namespace bn

    /**
     * Additional class for working with ratio based on BIGNUMs
     */
    struct BIGRATIO {
        BIGRATIO();

        BIGRATIO(std::shared_ptr<BIGNUM> &&numerator, std::shared_ptr<BIGNUM> &&denominator);

        BIGRATIO(const BIGRATIO &rhs) = default;
        BIGRATIO(BIGRATIO &&rhs) = default;
        BIGRATIO &operator=(const BIGRATIO &rhs) = delete;
        BIGRATIO &operator=(BIGRATIO &&rhs) = delete;

        /**
         * Performs the operation "a = a + b", modifying the caller
         * @param rhs - "b"
         * @return a reference to the calling object
         * @throw std::runtime_error
         */
        BIGRATIO &operator+=(const BIGRATIO &rhs);

        /**
         * Returns a new BIGRATIO obtained as ((this.numerator * rhs) / this.denominator)
         * @param rhs - right BIGNUM multiplier
         * @return the result of the operation
         * @throw std::runtime_error
         */
        BIGRATIO operator*(const std::shared_ptr<BIGNUM> &rhs);

        /**
         * Returns the integer part of the ratio
         * @return shared pointer to BIGNUM
         * @throw std::runtime_error
         */
        std::shared_ptr<BIGNUM> intPart();

        std::shared_ptr<BIGNUM> numerator;
        std::shared_ptr<BIGNUM> denominator;
        bool isNull = false;

    private:
        std::shared_ptr<BN_CTX> _ctx;

        void add(const BIGRATIO &rhs);

        void assign(const BIGRATIO &rhs);
    };

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

    /**
     * Removes the 0x prefix from the input argument
     * @param prefixString - secret and parts of the secret, with a hex prefix
     * @return a reference to a string without a prefix
     */
    std::string &removeHexPrefix(std::string &prefixString);

    /**
    * Append the 0x prefix to the input argument
    * @param prefixString - secret and parts of the secret, without a hex prefix
    * @return a reference to a new string with a prefix
    */
    std::string appendHexPrefix(const std::string &prefixString);

}  // namespace hw3

#endif  // HW_3_UTILS_H
