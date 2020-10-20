#ifndef HW_3_UTILS_H
#define HW_3_UTILS_H

#include <string>
#include <utility>
#include <vector>
#include <openssl/bn.h>
#include <memory>

namespace HW3 {

    using BN_CTX_ptr = std::unique_ptr<BN_CTX, decltype(&BN_CTX_free)>;
    using BIGNUM_ptr = std::unique_ptr<BIGNUM , decltype(&BN_free)>;


    /**
     * Additional class for working with ratio based on BIGNUMs
     */
    struct BIGRATIO {
        BIGRATIO();
        BIGRATIO(const BIGRATIO &rhs) = default;
        BIGRATIO(BIGRATIO &&rhs) = delete;
        BIGRATIO &operator=(const BIGRATIO &rhs) = default;
        BIGRATIO &operator=(BIGRATIO &&rhs) = delete;
        ~BIGRATIO();
        BIGNUM *numerator = nullptr;
        BIGNUM *denominator = nullptr;
        bool isInit = false;
        BIGNUM *intPart();
        BIGRATIO &operator+=(const BIGRATIO &rhs);
        BIGRATIO operator*(const BIGNUM *rhs) const;

    private:
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


}  // namespace hw3

#endif  // HW_3_UTILS_H
