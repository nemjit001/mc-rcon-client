cmake_minimum_required(VERSION 3.0.0)
project(MC_RCON_Client VERSION 0.1.0)

include_directories(src)
include_directories(headers)

include(CTest)
enable_testing()

add_executable(
    MC_RCON_Client
    src/main.cpp 
    src/CircularLineBuffer.cpp 
    src/RCONClient.cpp 
    src/socket.cpp
)

link_libraries(Ws2_win32)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
