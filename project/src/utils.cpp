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


    BIGNUM *BIGRATIO::intPart() {
        return nullptr;
    }

    BIGRATIO::BIGRATIO() : numerator(BN_new()), denominator(BN_new()), isInit(false){
        if (!numerator || !denominator) {throw std::runtime_error("error in ctor BIGRATIO"); }
        int rc = BN_zero(numerator);
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
        rc= BN_zero(denominator);
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
    }

    BIGRATIO::~BIGRATIO() {
        BN_free(numerator);
        BN_free(denominator);
    }

    BIGRATIO &BIGRATIO::operator+=(const BIGRATIO &rhs) {
        isInit ? add(rhs) : assign(rhs);
        return *this;
    }

    BIGRATIO BIGRATIO::operator*(const BIGNUM *rhs) const {
        BIGRATIO bigratio;
        BN_CTX_ptr ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx.get()) { throw std::runtime_error("error BN_CTX_new"); }

        int rc = BN_mul(bigratio.numerator, this->numerator, rhs, ctx.get());
        if (rc == 0 ) { throw std::runtime_error("error BN_mul"); }

        rc = BN_add(bigratio.denominator, bigratio.denominator, this->denominator);
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }

        return bigratio; // TODO: copy ptr then free => sigsegv
    }

    void BIGRATIO::assign(const BIGRATIO &rhs) {
        int rc = BN_add(this->numerator, this->numerator ,rhs.numerator);
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }
        rc = BN_add(this->denominator, this->denominator ,rhs.denominator);
        if (rc == 0 ) { throw std::runtime_error("error BN_add"); }
        isInit = true;
    }

    void BIGRATIO::add(const BIGRATIO &rhs) {
        BN_CTX_ptr ctx(BN_CTX_new(), BN_CTX_free);
        if (!ctx.get()) { throw std::runtime_error("error BN_CTX_new"); }

        BIGNUM_ptr mulLeft(BN_new(), BN_free);
        if (!mulLeft.get()) { throw std::runtime_error("error BN_new"); }

        BIGNUM_ptr mulRight(BN_new(), BN_free);
        if (!mulLeft.get()) { throw std::runtime_error("error BN_new"); }

        int rc = BN_mul(mulLeft.get(), this->numerator, rhs.denominator, ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }

        rc = BN_mul(mulRight.get(), this->denominator, rhs.numerator, ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }

        rc = BN_add(this->numerator, mulLeft.get(), mulRight.get());
        if (rc == 0) { throw std::runtime_error("error BN_add"); }

        rc = BN_mul(this->denominator, this->denominator, rhs.denominator, ctx.get());
        if (rc == 0) { throw std::runtime_error("error BN_mul"); }
    }
}  // namespace HW3
