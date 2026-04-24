include(FetchContent)
cmake_minimum_required(VERSION 3.28)
project(secure_chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

# Add Targets Libraries
add_subdirectory(src/)

# Add Binary Targets
add_subdirectory(tests)
# add_subdirectory(apps)