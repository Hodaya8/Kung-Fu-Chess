cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# בנייה עצמאית של UI
if(NOT TARGET LogicLib)
    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC"
        "${CMAKE_CURRENT_BINARY_DIR}/LOGIC"
    )
endif()

# JSON
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)

# Asio
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)

FetchContent_MakeAvailable(asio)

# WebSocket++
FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)

FetchContent_MakeAvailable(websocketpp)

# קובצי UI
file(
    GLOB_RECURSE
    UI_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(
    GLOB_RECURSE
    UI_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable(
    ${PROJECT_NAME}
    ${UI_SOURCES}
    ${UI_HEADERS}
)

find_package(Threads REQUIRED)

# OpenCV
set(
    OPENCV_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
)

set(
    OPENCV_INCLUDE_DIR
    "${OPENCV_DIR}/include"
)

set(
    OPENCV_LIB_DIR
    "${OPENCV_DIR}/bin"
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${OPENCV_INCLUDE_DIR}"
    "${asio_SOURCE_DIR}/asio/include"
    "${websocketpp_SOURCE_DIR}"
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${OPENCV_LIB_DIR}"
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    LogicLib
    nlohmann_json::nlohmann_json
    Threads::Threads

    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.lib>
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_if_different

        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.dll>

        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()