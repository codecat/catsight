cmake_minimum_required(VERSION 3.19)
project(catsight LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)

set(HELLOIMGUI_USE_GLFW_OPENGL3 ON CACHE BOOL "" FORCE)
add_subdirectory(external/hello_imgui)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${HELLOIMGUI_BASEPATH}/hello_imgui_cmake)

include(hello_imgui_add_app)
hello_imgui_add_app(catsight
	src/main.cpp
	src/Common.cpp
	src/System.cpp

	src/Resources.cpp
	src/Random.cpp
	src/Folder.cpp

	src/Explorer.cpp
	src/Inspector.cpp
	src/Tab.cpp

	src/Tabs/Memory.cpp
)

include_directories(
	external/scratch2/scratch2/
	src/
)

target_link_libraries(catsight PUBLIC dl)

if(WIN32)
	add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX)
	add_definitions(-DPLATFORM_LINUX)
endif()
