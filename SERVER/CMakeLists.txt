# cmake_minimum_required(VERSION 3.16)
# set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
# project(KungFuChessServer)

# include(FetchContent)

# # 1. משיכת ספריית JSON
# FetchContent_Declare(
#     json
#     GIT_REPOSITORY https://github.com/nlohmann/json.git
#     GIT_TAG v3.11.2
# )
# FetchContent_MakeAvailable(json)

# # 2. משיכת Asio (Standalone)
# FetchContent_Declare(
#     asio
#     GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
#     GIT_TAG asio-1-28-0
# )
# FetchContent_MakeAvailable(asio)

# # 3. משיכת WebSocket++
# FetchContent_Declare(
#     websocketpp
#     GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
#     GIT_TAG 0.8.2
# )
# FetchContent_MakeAvailable(websocketpp)

# # 4. משיכת SQLite3 אוטומטית בצורה ישירה
# FetchContent_Declare(
#     sqlite3_src
#     URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
#     DOWNLOAD_EXTRACT_TIMESTAMP TRUE
# )
# FetchContent_MakeAvailable(sqlite3_src)

# # איסוף קבצי המקור של השרת וצירוף קובץ ה-SQLite שהורד אוטומטית
# file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# list(APPEND SERVER_SOURCES "${sqlite3_src_SOURCE_DIR}/sqlite3.c")

# file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
# list(APPEND SERVER_HEADERS "${sqlite3_src_SOURCE_DIR}/sqlite3.h")

# # יצירת קובץ ההפעלה
# add_executable(${PROJECT_NAME} ${SERVER_SOURCES} ${SERVER_HEADERS})

# find_package(Threads REQUIRED)

# target_link_libraries(${PROJECT_NAME} PRIVATE 
#     nlohmann_json::nlohmann_json
#     Threads::Threads
# )

# # =========================================================
# # התיקון כאן: הוספנו את "CMAKE_CURRENT_SOURCE_DIR" לנתיבים 
# # כדי שהקומפיילר ימצא את תיקיית include של השרת!
# # =========================================================
# target_include_directories(${PROJECT_NAME} PRIVATE 
#     "${asio_SOURCE_DIR}/asio/include"
#     "${websocketpp_SOURCE_DIR}"
#     "${sqlite3_src_SOURCE_DIR}"
#     "${CMAKE_CURRENT_SOURCE_DIR}"
# )

# target_compile_definitions(${PROJECT_NAME} PRIVATE ASIO_STANDALONE)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(KungFuChessServer)

include(FetchContent)

# 1. משיכת ספריית JSON
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# 2. משיכת Asio (Standalone)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)

# 3. משיכת WebSocket++
FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)

# 4. משיכת SQLite3 אוטומטית בצורה ישירה
FetchContent_Declare(
    sqlite3_src
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite3_src)

# =========================================================
# הוספת תיקיית הלוגיקה מחוץ לתיקיית השרת
# =========================================================

# איסוף קבצי המקור של השרת וצירוף קובץ ה-SQLite שהורד אוטומטית
file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(APPEND SERVER_SOURCES "${sqlite3_src_SOURCE_DIR}/sqlite3.c")

file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
list(APPEND SERVER_HEADERS "${sqlite3_src_SOURCE_DIR}/sqlite3.h")

# יצירת קובץ ההפעלה
add_executable(${PROJECT_NAME} ${SERVER_SOURCES} ${SERVER_HEADERS})

find_package(Threads REQUIRED)

# =========================================================
# הוספנו את LogicLib לרשימת הקישורים של השרת!
# =========================================================
target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Threads::Threads
    LogicLib
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${asio_SOURCE_DIR}/asio/include"
    "${websocketpp_SOURCE_DIR}"
    "${sqlite3_src_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    ASIO_STANDALONE 
    _WEBSOCKETPP_CPP11_STL_
)