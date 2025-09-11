cmake_minimum_required(VERSION 3.24)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(void)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Release)

get_filename_component(LIB_DIR "../vendor/lib" REALPATH)

include_directories(../src ../src/util ../src/pal)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	file(GLOB SOURCES "../src/*.cpp" "../src/*.c" "../src/util/*.cpp" "../src/pal/*.cpp" "../vendor/imgui/*.cpp")

	add_executable(${PROJECT_NAME} ${SOURCES})
	target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:DEBUG>)

	target_link_libraries(${PROJECT_NAME} "${LIB_DIR}/libglfw3.a")
	target_link_libraries(${PROJECT_NAME} -lGL)
	add_compile_options(-Wall -Wextra -O0)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Including .h files here due to a quirk of Visual Studio's CMAKE implementation - necessary for seeing files in editor
	file(GLOB SOURCES "../src/*.cpp" "../src/*.c" "../src/*.h" "../src/util/*.cpp" "../src/util/*.h" "../src/pal/*.cpp" "../src/pal/*.cpp" "../vendor/imgui/*.cpp")

	add_executable(${PROJECT_NAME} ${SOURCES})
	target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:DEBUG>)

	target_link_libraries(${PROJECT_NAME} "${LIB_DIR}/glfw3.lib")
	target_link_libraries(${PROJECT_NAME} opengl32.lib)
	add_compile_options("/W4 /wd4996")

endif()
