#include <stdexcept>
#include <iostream>
#include "utils.h"
#include "shamir.h"

namespace HW3 {
    const char *RECOVER_STR = "recover";
    const char *SPLIT_STR = "split";
    const int SPLIT_LEFT_BORDER = 2;
    const int SPLIT_RIGHT_BORDER = 100;
    const char *HEX_PREFIX = "0x";

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
        auto shadows = splitSecret(removeHexPrefix(secretString), totalShadow, minShadow);
        for (auto &shadow : shadows) {
            std::cout << shadow << std::endl;
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
        std::cout << appendHexPrefix(recoverSecret(shadows)) << std::endl;
    }

    std::string &removeHexPrefix(std::string &prefixString) {
        static std::string prefix(HEX_PREFIX);
        if (prefixString.compare(0, prefix.size(), prefix) != 0) {
            return prefixString;
        } else {
            prefixString = prefixString.substr(prefix.size(), prefixString.size());
            return prefixString;
        }
    }

    std::string appendHexPrefix(const std::string &prefixString) {
        return HEX_PREFIX + prefixString;
    }

    BIGRATIO::BIGRATIO() : numerator(BN_new(), BN_free), denominator(BN_new(), BN_free),
                           isNull(true), _ctx(bn::make_ctx()) {
        if (!numerator || !denominator) {throw std::runtime_error("error in ctor BIGRATIO"); }
        int rc = BN_zero(numerator.get());
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
        rc = BN_zero(denominator.get());
        if (rc == 0) {throw std::runtime_error("error in ctor BIGRATIO"); }
    }

    BIGRATIO &BIGRATIO::operator+=(const BIGRATIO &rhs) {
        isNull ? assign(rhs) : add(rhs);
        return *this;
    }

    BIGRATIO BIGRATIO::operator*(const std::shared_ptr<BIGNUM> &rhs) {
        BIGRATIO bigratio;

        bn::mul(bigratio.numerator, this->numerator, rhs, _ctx);
        bn::add(bigratio.denominator, bigratio.denominator, this->denominator);

        return bigratio;
    }

    void BIGRATIO::assign(const BIGRATIO &rhs) {
        bn::add(this->numerator, this->numerator, rhs.numerator);
        bn::add(this->denominator, this->denominator, rhs.denominator);
        isNull = false;
    }

    void BIGRATIO::add(const BIGRATIO &rhs) {
        auto mulLeft = bn::make_bignum();
        auto mulRight = bn::make_bignum();

        bn::mul(mulLeft, this->numerator, rhs.denominator, _ctx);
        bn::mul(mulRight, rhs.numerator, this->denominator, _ctx);
        bn::add(this->numerator, mulLeft, mulRight);
        bn::mul(this->denominator, this->denominator, rhs.denominator, _ctx);
    }

    std::shared_ptr<BIGNUM> BIGRATIO::intPart() {
        auto result = bn::make_bignum();
        bn::div(result, numerator, denominator, _ctx);
        return result;
    }

    BIGRATIO::BIGRATIO(std::shared_ptr<BIGNUM> &&numerator, std::shared_ptr<BIGNUM> &&denominator) :
    numerator(numerator), denominator(denominator), isNull(false /* let's trust that the parameters
    * do not contain a null BIGNUM */), _ctx(bn::make_ctx()){
    }

    void bn::mul(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &b, std::shared_ptr<BN_CTX> &ctx) {
        if (BN_mul(r.get(), a.get(), b.get(), ctx.get()) == 0) {
            throw std::runtime_error("error BN_mul");
        }
    }

    void bn::sub(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &b) {
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
        if(!bnPtr) { throw std::runtime_error("hex2bn: BIGNUM must be created"); }
        if (BN_hex2bn(&bnPtr, numString.c_str()) == 0) {
            throw std::runtime_error("hex2bn: some error, input value: " + numString);
        }
    }

    std::string bn::bn2hex(const std::shared_ptr<BIGNUM> &a) {
        char *resultCString = BN_bn2hex(a.get());
        if (!resultCString) { throw std::runtime_error("error BN_bn2hex"); }
        std::string resultString(resultCString);
        OPENSSL_free(resultCString);
        return resultString;
    }

    void bn::div(std::shared_ptr<BIGNUM> &dv, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &d, std::shared_ptr<BN_CTX> &ctx) {
        if (BN_div(dv.get(), nullptr, a.get(), d.get(), ctx.get()) == 0) {
            throw std::runtime_error("error BN_div");
        }
    }

    void bn::exp(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &p, std::shared_ptr<BN_CTX> &ctx) {
        if (BN_exp(r.get(), a.get(), p.get(), ctx.get()) == 0) {
            throw std::runtime_error("error BN_exp");
        }
    }

    void bn::add(std::shared_ptr<BIGNUM> &r, const std::shared_ptr<BIGNUM> &a,
                 const std::shared_ptr<BIGNUM> &b) {
        if (BN_add(r.get(), a.get(), b.get()) == 0) {
            throw std::runtime_error("error BN_add");
        }
    }
}  // namespace HW3
