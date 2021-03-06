cmake_minimum_required(VERSION 3.19)
project(catsight LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/zydis)

set(HELLOIMGUI_USE_GLFW_OPENGL3 ON CACHE BOOL "" FORCE)
add_subdirectory(external/hello_imgui)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${HELLOIMGUI_BASEPATH}/hello_imgui_cmake)

add_subdirectory(external/sol2)

include(FindLua)
find_package(Lua REQUIRED)

set(CATSIGHT_SOURCES
	src/main.cpp
	src/Common.cpp
	src/System/ProcessHandle.cpp
	src/System/ProcessMemoryRegion.cpp
	src/System/ProcessInfo.cpp

	src/Lib/MurmurHash2.cpp

	src/Resources.cpp
	src/Folder.cpp
	src/Chrono.cpp
	src/Disassembler.cpp
	src/Patterns.cpp

	src/Tasks/Tasks.cpp
	src/Tasks/TaskWorker.cpp
	src/Tasks/Task.cpp

	src/Explorer.cpp
	src/Inspector.cpp
	src/Tab.cpp

	src/Tabs/MapsTab.cpp
	src/Tabs/ModulesTab.cpp
	src/Tabs/ConsoleTab.cpp
	src/Tabs/MemoryTab.cpp
	src/Tabs/DataTab.cpp
	src/Tabs/CodeTab.cpp
	src/Tabs/TaskWaitTab.cpp
	src/Tabs/ResultsTab.cpp
	src/Tabs/CodeResultsTab.cpp
	src/Tabs/StringsTab.cpp

	src/Helpers/MemoryValidator.cpp
	src/Helpers/DataButton.cpp
	src/Helpers/CodeButton.cpp
	src/Helpers/PointerText.cpp
	src/Helpers/ImGuiString.cpp
	src/Helpers/TypeRenderer.cpp
	src/Helpers/Expression.cpp
	src/Helpers/LabelManager.cpp

	src/Script/Engine.cpp
	src/Script/Types/Pointer.cpp
)

set(CATSIGHT_LIBS
	Zydis
	${LUA_LIBRARIES}
)

if(WIN32)
	list(APPEND CATSIGHT_SOURCES
		src/System/Windows/WindowsSystem.cpp
		src/System/Windows/WindowsProcessHandle.cpp
		src/System/Windows/WindowsError.cpp
	)
	list(APPEND CATSIGHT_LIBS
		DbgHelp
	)
elseif(UNIX)
	list(APPEND CATSIGHT_SOURCES
		src/System/Linux/LinuxSystem.cpp
		src/System/Linux/LinuxProcessHandle.cpp
	)
endif()

include(hello_imgui_add_app)
hello_imgui_add_app(catsight ${CATSIGHT_SOURCES})

include_directories(
	src/
	external/scratch2/scratch2
	external/glm/
	external/zydis/include
	external/sol2/include
	${LUA_INCLUDE_DIR}
)

target_link_libraries(catsight PUBLIC ${CATSIGHT_LIBS})

if(WIN32)
	add_definitions(
		-DPLATFORM_WINDOWS
		-DWIN32_LEAN_AND_MEAN
	)
elseif(UNIX)
	add_definitions(
		-DPLATFORM_LINUX
	)
	include_directories(
		external/ELFIO
	)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_definitions(-DPLATFORM_64)
else()
	add_definitions(-DPLATFORM_32)
endif()
