cmake_minimum_required(VERSION 3.16)

project(KungFuChessProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# הסדר קובע! הלוגיקה נבנית קודם
add_subdirectory(LOGIC)
add_subdirectory(UI)
add_subdirectory(SERVER)