project(Shared)

file(GLOB_RECURSE SRCS "*.cpp")

add_library(Shared STATIC ${SRCS})

target_include_directories(Shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Shared PROPERTIES
    CXX_STANDARD
    20
)

target_link_libraries(Shared PRIVATE
    Client
    Logging
    Network
    Server
)