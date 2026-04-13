include(FetchContent)
cmake_minimum_required(VERSION 3.28)
project(secure_chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

FetchContent_Declare(
    cryptopp
    GIT_REPOSITORY https://github.com/weidai11/cryptopp.git
    GIT_TAG        CRYPTOPP_8_9_0
)

set(CRYPTOPP_USE_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(cryptopp)

set(CRYPTOPP_LIB_DIR "${cryptopp_SOURCE_DIR}")
add_library(cryptopp STATIC IMPORTED)
set_target_properties(cryptopp PROPERTIES
    IMPORTED_LOCATION "${CRYPTOPP_LIB_DIR}/libcryptopp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CRYPTOPP_LIB_DIR}"
)

# Add Targets Libraries
add_subdirectory(src/)

# Add Binary Targets
add_subdirectory(tests)
add_subdirectory(apps)