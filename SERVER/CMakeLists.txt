cmake_minimum_required(VERSION 3.16)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(KungFuChessServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# =========================================================
# LogicLib
# =========================================================

# מאפשר לבנות גם מתוך התיקייה הראשית
# וגם ישירות מתוך תיקיית SERVER.
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
    GIT_REPOSITORY
        https://github.com/nlohmann/json.git
    GIT_TAG
        v3.11.2
)

FetchContent_MakeAvailable(json)

# =========================================================
# Asio
# =========================================================

FetchContent_Declare(
    asio
    GIT_REPOSITORY
        https://github.com/chriskohlhoff/asio.git
    GIT_TAG
        asio-1-28-0
)

FetchContent_MakeAvailable(asio)

# =========================================================
# WebSocket++
# =========================================================

FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY
        https://github.com/zaphoyd/websocketpp.git
    GIT_TAG
        0.8.2
)

FetchContent_MakeAvailable(websocketpp)

# =========================================================
# SQLite
# =========================================================

FetchContent_Declare(
    sqlite3_src
    URL
        https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
    DOWNLOAD_EXTRACT_TIMESTAMP
        TRUE
)

FetchContent_MakeAvailable(sqlite3_src)

# =========================================================
# קובצי השרת
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

list(
    APPEND
    SERVER_SOURCES
    "${sqlite3_src_SOURCE_DIR}/sqlite3.c"
)

# =========================================================
# קובץ ההפעלה
# =========================================================

add_executable(
    ${PROJECT_NAME}
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

find_package(
    Threads
    REQUIRED
)

# =========================================================
# ספריות
# =========================================================

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE

    LogicLib
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# =========================================================
# תיקיות include
# =========================================================

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE

    # מאפשר:
    # include "game/game_session.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"

    # נשאר זמנית עבור includes ישנים שמתחילים ב-include/
    "${CMAKE_CURRENT_SOURCE_DIR}"

    "${asio_SOURCE_DIR}/asio/include"
    "${websocketpp_SOURCE_DIR}"
    "${sqlite3_src_SOURCE_DIR}"
)

# =========================================================
# הגדרות קומפילציה
# =========================================================

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE

    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
)