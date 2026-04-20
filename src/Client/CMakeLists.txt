project(Client)

add_library(Client STATIC
    Client.cpp
    ClientCLI.cpp
    MessageBuilder.cpp
)

target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Client PROPERTIES
    CXX_STANDARD
    23
)