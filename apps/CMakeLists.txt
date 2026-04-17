project(Apps LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/apps")

set(SRCS
    #client.cpp
    #server.cpp
)

foreach(SRC ${SRCS})
    get_filename_component(EXE ${SRC} NAME_WE)
    
    add_executable(${EXE} ${SRC})
    target_include_directories(${EXE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(${EXE} PRIVATE Logging Network Server Shared) #Client
endforeach()