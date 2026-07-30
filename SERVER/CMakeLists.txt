cmake_minimum_required(VERSION 3.16)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(KungFuChessServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# =========================================================
# LogicLib
# =========================================================
if(NOT TARGET LogicLib)
    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC"
        "${CMAKE_CURRENT_BINARY_DIR}/logic"
    )
endif()

# =========================================================
# JSON
# =========================================================
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# =========================================================
# Asio
# =========================================================
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)

# =========================================================
# WebSocket++
# =========================================================
FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)

# =========================================================
# Server Files
# =========================================================
file(
    GLOB_RECURSE
    SERVER_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(
    GLOB_RECURSE
    SERVER_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# =========================================================
# Executable
# =========================================================
add_executable(
    ${PROJECT_NAME}
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

find_package(Threads REQUIRED)

# =========================================================
# Libraries
# =========================================================
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE

    LogicLib
    nlohmann_json::nlohmann_json
    Threads::Threads
    hiredis
)

# =========================================================
# Include Directories
# =========================================================
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${asio_SOURCE_DIR}/asio/include"
    "${websocketpp_SOURCE_DIR}"
)

# =========================================================
# Compile Definitions
# =========================================================
target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE

    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
)

if(MSVC)
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        /FS
    )
endif()