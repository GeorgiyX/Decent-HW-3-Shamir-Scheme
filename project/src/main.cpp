#include <iostream>
#include "utils.h"

int main(int argc, char **argv) {
    try {
        HW3::args2mode(argc, argv) ?
        HW3::runInSplitMode() :
        HW3::runInRecoverMode();
    } catch (std::runtime_error &error) {
        std::cout << error.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error" << std::endl;
    }
    return 0;
}
