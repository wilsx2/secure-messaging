project(Messaging)

add_library(Messaging STATIC
    Message.cpp
    SecureChannel.cpp
    TcpSocket.cpp
)

target_include_directories(Messaging PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# target_link_libraries(Messaging PRIVATE
    # Logging
# )