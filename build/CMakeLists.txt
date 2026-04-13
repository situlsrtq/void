cmake_minimum_required(VERSION 3.24)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(void)

#--------------------------------------------------------------------------------------------------
# Helper functions
#--------------------------------------------------------------------------------------------------
function(require_submodule OPTION_NAME CHECK_FILE_PATH)
	if(${OPTION_NAME})
		get_filename_component(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${CHECK_FILE_PATH}" ABSOLUTE)
        
        	if(NOT EXISTS "${FULL_PATH}")
			message(FATAL_ERROR 
			"\n[ERROR]: ${OPTION_NAME} is ON, but the submodule is missing.\n"
                	"To fix this, either run:\n"
                	"  git submodule update --init --recursive\n"
                	"Or turn off the toggle\n"
            	)
        	endif()
    	endif()
endfunction()

#--------------------------------------------------------------------------------------------------
# Build-time options
#--------------------------------------------------------------------------------------------------
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE Release)

# Toggle and check for optional vendor libraries. Every non-core vendor needs these. Ensure that the path passed to require_submodle() points to a specific file that cannot exist unless the module has been initialized
option(TRACY_ENABLE "Enables Tracy profiler" ON)
require_submodule(TRACY_ENABLE, "../vendor/optional/tracy/public/tracy/Tracy.hpp")

#--------------------------------------------------------------------------------------------------
# Core config
#--------------------------------------------------------------------------------------------------
# Header files included here because of a Visual Studio quirk that requires them to be present to show up in the editor
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS
	"../src/*.cpp" "../src/*.c" "../src/*.h" 
	"../src/util/*.cpp" "../src/util/*.h" 
	"../src/pal/*.cpp" "../src/pal/*.h"
)

# Core vendors get added here if they are a new default for the project. For evaluating a vendor, use the code in |--Optional/non-core--| as a template
file(GLOB VENDOR_CORE_SOURCES CONFIGURE_DEPENDS
	"../vendor/core/glad/glad.c" 
	"../vendor/core/murmur/MurmurHash3.cpp" 
	"../vendor/core/imgui/*.cpp"
)

add_executable(${PROJECT_NAME} ${CORE_SOURCES} ${VENDOR_CORE_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ../src ../src/util ../src/pal)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "../vendor/core")

target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:DEBUG>)

# Strip conflicting global debug tag so vendor libraries can be compiled in 'release' mode
if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

    add_compile_options("$<$<CONFIG:Debug>:/RTC1>")
endif()


if(MSVC)
	add_compile_options(/W4 /wd4996 /D_HAS_EXCEPTIONS=0 /GR-)
	set_source_files_properties(${VENDOR_CORE_SOURCES} PROPERTIES COMPILE_OPTIONS "/W0;/O2;/DNDEBUG;/RTC-")
else()
	add_compile_options(-Wall -Wextra -Werror -fno-exceptions)
	add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
	set_source_files_properties(${VENDOR_CORE_SOURCES} PROPERTIES COMPILE_OPTIONS "-w;-O3;-DNDEBUG;-g0")
endif()

#--------------------------------------------------------------------------------------------------
# Optional/non-core vendors. MUST be protected by a build-time cmake toggle
#--------------------------------------------------------------------------------------------------
if(TRACY_ENABLE)
	set(ADD_SRC "../vendor/optional/tracy/public/TracyClient.cpp")
	set(ADD_INC "../vendor/optional/tracy/public/")
	target_sources(${PROJECT_NAME} PRIVATE ${ADD_SRC})
	target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${ADD_INC})
	
	# Optional, not all vendors will require project-wide macros
	target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_ENABLE)

	if(MSVC)
		set_source_files_properties(${ADD_SRC} PROPERTIES COMPILE_OPTIONS "/W0;/O2;/DNDEBUG;-RTC")
	else()
		set_source_files_properties(${ADD_SRC} PROPERTIES COMPILE_OPTIONS "-w;-O3;-DNDEBUG;-g0")
	endif()
endif()

#--------------------------------------------------------------------------------------------------
# Linking
#--------------------------------------------------------------------------------------------------
get_filename_component(LIB_DIR "../vendor/lib" REALPATH)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE "${LIB_DIR}/glfw3.lib" opengl32)
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE "${LIB_DIR}/libglfw3.a" GL)
endif()
