cmake_minimum_required(VERSION 3.16)
project(KungFuChess)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# עיגון מוחלט לנתיב התיקייה הנוכחית (UI)
file(GLOB_RECURSE LOGIC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/src/*.cpp")
file(GLOB_RECURSE LOGIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/*.hpp")
file(GLOB_RECURSE UI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE UI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

# Combine sources
set(SOURCES ${LOGIC_SOURCES} ${UI_SOURCES})
set(HEADERS ${LOGIC_HEADERS} ${UI_HEADERS})

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# הוספת נתיבי ה-include
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include"          # <--- זו השורה שפותרת את הבעיה!
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/engine"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/input"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/io"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/model"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/realtime"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LOGIC/include/rules"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# הגדרת נתיבי ספריית OpenCV
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")
set(OPENCV_LIB_DIR "${OPENCV_DIR}/bin")

# Include OpenCV directories
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_INCLUDE_DIR})

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE ${OPENCV_LIB_DIR})

# Link OpenCV libraries
target_link_libraries(${PROJECT_NAME} 
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.lib>
)

# Copy OpenCV DLLs to output directory
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

# הדפסות למעקב
message(STATUS "Found logic sources: ${LOGIC_SOURCES}")
message(STATUS "Found UI sources: ${UI_SOURCES}")