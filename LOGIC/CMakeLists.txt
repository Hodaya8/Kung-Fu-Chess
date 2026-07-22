cmake_minimum_required(VERSION 3.16)
project(KungFuChessLogic)

# איסוף קבצי הלוגיקה בלבד מתוך התיקייה הנוכחית
file(GLOB_RECURSE LOGIC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE LOGIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# יצירת ספרייה פנימית מהקבצים
add_library(LogicLib STATIC ${LOGIC_SOURCES} ${LOGIC_HEADERS})

# הגדרת נתיבי הגישה
# ההוראה PUBLIC אומרת שכל מי שיתחבר לספרייה הזו (כמו ה-UI)
# יקבל את כל נתיבי ה-include האלו אוטומטית!
target_include_directories(LogicLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/input"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/io"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/model"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/realtime"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rules"
)