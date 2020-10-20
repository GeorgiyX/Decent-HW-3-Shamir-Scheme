cmake_minimum_required(VERSION 3.16.3)
project(hw3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

include_directories(project/include)
find_package(OpenSSL REQUIRED)

add_executable(hw-3 project/src/main.cpp project/include/utils.h project/src/utils.cpp project/include/shamir.h project/src/shamir.cpp)
target_link_libraries(hw-3 OpenSSL::Crypto)