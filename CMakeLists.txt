cmake_minimum_required(VERSION 3.19)
project(catsight LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)

set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/zydis)

set(HELLOIMGUI_USE_GLFW_OPENGL3 ON CACHE BOOL "" FORCE)
add_subdirectory(external/hello_imgui)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${HELLOIMGUI_BASEPATH}/hello_imgui_cmake)

include(hello_imgui_add_app)
hello_imgui_add_app(catsight
	src/main.cpp
	src/Common.cpp
	src/System/ProcessHandle.cpp
	src/System/ProcessMemoryRegion.cpp

	src/System/Linux/System.cpp
	src/System/Linux/LinuxProcessHandle.cpp

	src/System/Windows/System.cpp

	src/Resources.cpp
	src/Random.cpp
	src/Folder.cpp

	src/Explorer.cpp
	src/Inspector.cpp
	src/Tab.cpp

	src/Tabs/MapsTab.cpp
	src/Tabs/MemoryTab.cpp
	src/Tabs/DataTab.cpp
	src/Tabs/CodeTab.cpp

	src/Helpers/DataButton.cpp
	src/Helpers/CodeButton.cpp
)

include_directories(
	external/scratch2/scratch2/
	external/glm/
	external/zydis/include
	src/
)

target_link_libraries(catsight PUBLIC Zydis)

if(WIN32)
	add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX)
	add_definitions(-DPLATFORM_LINUX)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_definitions(-DPLATFORM_64)
else()
	add_definitions(-DPLATFORM_32)
endif()
