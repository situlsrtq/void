
cmake_minimum_required(VERSION 3.31.5)

project(GyroScope)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Release)
get_filename_component(LIB_DIR "../lib" REALPATH)

include_directories(../include)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	file(GLOB SOURCES "../src/*.cpp" "../src/*.c")

	add_executable(${PROJECT_NAME} ${SOURCES})

	target_link_libraries(${PROJECT_NAME} "${LIB_DIR}/libglfw3.a")
	target_link_libraries(${PROJECT_NAME} -lGL)
	add_compile_options(-Wall -Wextra -O0)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	file(GLOB SOURCES "../src/*.cpp" "../src/*.c" "../src/*.h" "../include/util/*.h")

	add_executable(${PROJECT_NAME} ${SOURCES})

	target_link_libraries(${PROJECT_NAME} "${LIB_DIR}/glfw3.lib")
	target_link_libraries(${PROJECT_NAME} opengl32.lib)
	add_compile_options("/W4 /wd4996")

endif()