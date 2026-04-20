project(Server)

add_library(Server STATIC
    AccountRegistry.cpp
    Server.cpp
    SessionManager.cpp
)

target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Server PROPERTIES
    CXX_STANDARD
    23
)

target_link_libraries(Server PRIVATE
    Logging
    Network
    Shared
    cryptopp
)