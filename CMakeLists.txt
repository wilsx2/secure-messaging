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

add_library(cryptopp STATIC IMPORTED)
set_target_properties(cryptopp PROPERTIES
    IMPORTED_LOCATION "${cryptopp_SOURCE_DIR}/libcryptopp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${cryptopp_SOURCE_DIR}"
)

add_custom_command(
    OUTPUT ${cryptopp_SOURCE_DIR}/libcryptopp.a
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${cryptopp_SOURCE_DIR} static
    COMMENT "Building CryptoPP library"
    VERBATIM
)
add_custom_target(build_cryptopp ALL DEPENDS ${cryptopp_SOURCE_DIR}/libcryptopp.a)

# Add Targets Libraries
add_subdirectory(src/)

# Add Binary Targets
add_subdirectory(tests)
add_subdirectory(apps)