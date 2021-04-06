cmake_minimum_required(VERSION 3.0.0)
project(MC_RCON_Client VERSION 1.1.0)

set(TARGET_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)

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
    src/UI.cpp
)

if(WIN32)
    target_link_libraries(${TARGET_NAME} Ws2_32)
else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_NAME} Threads::Threads)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
