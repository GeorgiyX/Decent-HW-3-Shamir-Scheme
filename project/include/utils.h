#ifndef HW_3_UTILS_H
#define HW_3_UTILS_H

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

}  // namespace hw3

#endif  // HW_3_UTILS_H
