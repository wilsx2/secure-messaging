project(Tests LANGUAGES CXX)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/tests")

set(SRCS
    disconnectingserver.cpp
    handshake.cpp
    largemessage.cpp
	message.cpp
	singlelogin.cpp
	socket.cpp
)

enable_testing()
foreach(SRC ${SRCS})
    get_filename_component(EXE ${SRC} NAME_WE)
    
    add_executable(${EXE} ${SRC})
    target_include_directories(${EXE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(${EXE} PRIVATE Client Logging Network Server Shared)

    add_test(${EXE} ${EXE})
endforeach()