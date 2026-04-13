project(Logging)

add_library(Logging STATIC
    ConsoleLog.cpp
    Logger.cpp
)

target_include_directories(Logging PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set_target_properties(Logging PROPERTIES
    CXX_STANDARD
    20
)