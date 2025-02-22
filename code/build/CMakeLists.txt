
cmake_minimum_required(VERSION 3.31.5)

project(GyroScope)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Release)

include_directories(../include/glad ../include/KHR ../include/util)

file(GLOB SOURCES "../src/*.cpp" "../src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
add_compile_options(-Wall -Wextra -O0)

target_link_libraries(${PROJECT_NAME} -lglfw)
target_link_libraries(${PROJECT_NAME} -lGL)
