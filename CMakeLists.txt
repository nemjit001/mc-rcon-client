cmake_minimum_required(VERSION 3.0.0)
project(MC_RCON_Client VERSION 1.1.0 DESCRIPTION "RCON Client for Minecraft servers")
include(GNUInstallDirs)
include(CTest)

set(TARGET_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include(CTest)
enable_testing()

add_executable(
    ${TARGET_NAME}
    src/socket.cpp
    src/RCONClient.cpp
    src/main.cpp
)

if(WIN32)
    target_link_libraries(${TARGET_NAME} Ws2_32)
else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_NAME} Threads::Threads)
endif()

target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror -pedantic -g)

target_include_directories(${TARGET_NAME} PRIVATE src)
target_include_directories(${TARGET_NAME} PRIVATE headers)

install(
    TARGETS ${TARGET_NAME}
    RUNTIME
)
