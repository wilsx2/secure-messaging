project(Network)

add_library(Network STATIC
    IpAddress.cpp
    Serialization.cpp
    TcpClient.cpp
    TcpListener.cpp
    TcpSocket.cpp
)

target_include_directories(Network PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Network PROPERTIES
    CXX_STANDARD
    23
)

target_link_libraries(Network PRIVATE
    Logging
)