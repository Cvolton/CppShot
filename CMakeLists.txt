cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(CppShot VERSION 0.5.0)

file(GLOB_RECURSE SOURCES
	src/*.cpp
	res/*.rc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libunicows.a gdiplus gdi32 user32 kernel32 comctl32)
target_include_directories(${PROJECT_NAME} PRIVATE src)
add_definitions(-DUNICODE=1)
