cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(CppShot VERSION 0.5.1)

add_compile_definitions(UNICODE=1)

#if config release set mwindows
if(CMAKE_BUILD_TYPE STREQUAL "Release") 
	add_compile_definitions(DDNDEBUG=1)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-O2 -s -mwindows")
else()
	add_compile_definitions(DDEBUG=1)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/res/resources.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/res/resources.rc
  @ONLY)

file(GLOB_RECURSE SOURCES
	src/*.cpp
	res/*.rc
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/res/resources.rc)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libunicows.a gdiplus gdi32 user32 kernel32 comctl32)
target_include_directories(${PROJECT_NAME} PRIVATE src)