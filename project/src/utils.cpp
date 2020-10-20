#include "utils.h"
#include <stdexcept>

namespace HW3 {
    const char *RECOVER_STR = "recover";
    const char *SPLIT_STR = "split";

    bool args2mode(int argc, char **argv) {
        if (argc <= 1 || !argv) { throw std::runtime_error("wrong arguments"); }

        bool ret = false;
        if (std::string(SPLIT_STR) == argv[1]) {
            ret = true;
        } else if (std::string(RECOVER_STR) == argv[1]) {
            ret = false;
        } else { throw std::runtime_error("unknown 2nd argument") ;}

        return ret;
    }


}  // namespace HW3
