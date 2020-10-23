#include <stdexcept>
#include <iostream>
#include <utility>
#include "utils.h"
#include "shamir.h"

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

    void runInRecoverMode() {
        std::vector<Shadow> shadows;
        std::string shadowY;
        size_t shadowX = 0;
        while (std::cin >> shadowY) {
            shadowX++;
            shadows.emplace_back(shadowX, shadowY);
        }
        std::cout << recoverSecret(shadows);
    }

    BIGRATIO::BIGRATIO() : numerator(BN_new(), BN_free), denominator(BN_new(), BN_free), isNull(true) {
        if (!numerator || !denominator) {throw std::runtime_error("error in ctor BIGRATIO"); }
        int rc = BN_zero(numerator.get());
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
        rc= BN_zero(denominator.get());
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
    }

    BIGRATIO &BIGRATIO::operator+=(const BIGRATIO &rhs) {
        isNull ? assign(rhs) : add(rhs);
        return *this;
    }

    BIGRATIO BIGRATIO::operator*(const std::shared_ptr<BIGNUM> &rhs) const {
        BIGRATIO bigratio;
        std::shared_ptr<BN_CTX> ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }

        int rc = BN_mul(bigratio.numerator.get(), this->numerator.get(), rhs.get(), ctx.get());
        if (rc == 0 ) { throw std::runtime_error("error BN_mul"); }

        rc = BN_add(bigratio.denominator.get(), bigratio.denominator.get(), this->denominator.get());
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }

        return bigratio;
    }

    void BIGRATIO::assign(const BIGRATIO &rhs) {
        int rc = BN_add(this->numerator.get(), this->numerator.get() ,rhs.numerator.get());
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }
        rc = BN_add(this->denominator.get(), this->denominator.get(), rhs.denominator.get());
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }
        isNull = false;
    }

    void BIGRATIO::add(const BIGRATIO &rhs) {
        std::shared_ptr<BN_CTX> ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }

        std::shared_ptr<BIGNUM> mulLeft(BN_new(), BN_free);
        if (!mulLeft) { throw std::runtime_error("error BN_new"); }

        std::shared_ptr<BIGNUM> mulRight(BN_new(), BN_free);
        if (!mulLeft) { throw std::runtime_error("error BN_new"); }

        int rc = BN_mul(mulLeft.get(), this->numerator.get(), rhs.denominator.get(), ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }

        rc = BN_mul(mulRight.get(), this->denominator.get(), rhs.numerator.get(), ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }

        rc = BN_add(this->numerator.get(), mulLeft.get(), mulRight.get());
        if (rc == 0) { throw std::runtime_error("error BN_add"); }

        rc = BN_mul(this->denominator.get(), this->denominator.get(), rhs.denominator.get(), ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }
    }

    std::shared_ptr<BIGNUM> BIGRATIO::intPart() const {
        std::shared_ptr<BN_CTX> ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }

        std::shared_ptr<BIGNUM> result(BN_new(), BN_free);
        if (!result) { throw std::runtime_error("error BN_new"); }

        int rc = BN_div(result.get(), nullptr, numerator.get(), denominator.get(), ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_div"); }

        return result;
    }

    BIGRATIO::BIGRATIO(std::shared_ptr<BIGNUM> &&numerator, std::shared_ptr<BIGNUM> &&denominator) {

    }

    void bn::mul(std::shared_ptr<BIGNUM> &r, std::shared_ptr<BIGNUM> &a, std::shared_ptr<BIGNUM> &b,
                 std::shared_ptr<BN_CTX> &ctx) {
        if (BN_mul(r.get(), a.get(), b.get(), ctx.get()) == 0) {
            throw std::runtime_error("error BN_mul");
        }
    }

    void bn::sub(std::shared_ptr<BIGNUM> &r, std::shared_ptr<BIGNUM> &a, std::shared_ptr<BIGNUM> &b) {
        if (BN_sub(r.get(), a.get(), b.get()) == 0) {
            throw std::runtime_error("error BN_mul");
        }
    }

    void bn::one(std::shared_ptr<BIGNUM> &a) {
        if (BN_one(a.get()) == 0) {
            throw std::runtime_error("error BN_one");
        }
    }

    std::shared_ptr<BN_CTX> bn::make_ctx() {
        std::shared_ptr<BN_CTX> ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx) { throw std::runtime_error("error BN_CTX_new"); }
        return ctx;
    }

    std::shared_ptr<BIGNUM> bn::make_bignum() {
        std::shared_ptr<BIGNUM> bignum(BN_new(), BN_free);
        if (!bignum) { throw std::runtime_error("error BN_new"); }
        return bignum;
    }

    void bn::dec2bn(std::shared_ptr<BIGNUM> &bn, const std::string &numString) {
        auto bnPtr = bn.get();
        if(!bnPtr) { throw std::runtime_error("dec2bn: BIGNUM must be created"); }
        if (BN_dec2bn(&bnPtr, numString.c_str()) == 0) {
            throw std::runtime_error("dec2bn: some error, input value: " + numString);
        }
    }

    void bn::hex2bn(std::shared_ptr<BIGNUM> &bn, const std::string &numString) {
        auto bnPtr = bn.get();
        if(!bnPtr) { throw std::runtime_error("dec2bn: BIGNUM must be created"); }
        if (BN_dec2bn(&bnPtr, numString.c_str()) == 0) {
            throw std::runtime_error("dec2bn: some error, input value: " + numString);
        }
    }
}  // namespace HW3
