project(Crypto)

add_library(Crypto STATIC
    Hash.cpp
    KeyAgreement.cpp
    SecureKey.cpp
)

target_include_directories(Crypto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Crypto PROPERTIES
    CXX_STANDARD
    20
)

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

target_link_libraries(Crypto PRIVATE
    cryptopp
)