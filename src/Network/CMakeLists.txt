project(Network)

add_library(Network STATIC
    SecureChannel.cpp
    Serialization.cpp
    TcpSocket.cpp
)

target_include_directories(Network PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Network PROPERTIES
    CXX_STANDARD
    20
)

target_link_libraries(Network PRIVATE
    Logging
    cryptopp
)