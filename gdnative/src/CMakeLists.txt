cmake_minimum_required(VERSION 3.1)
project(GodotCV)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(GODOT_HEADERS, "${CMAKE_BINARY_DIR}/godot-headers")
set(GODOT_HEADERS_ZIP, "${CMAKE_BINARY_DIR}/godot-headers.zip")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -fPIC")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -g -shared")

find_package(OpenCV REQUIRED)
include_directories(AFTER "${OpenCV_INCLUDE_DIRS}" "include" "godot-headers")
link_libraries(${OpenCV_LIBS})

add_library(bouncy SHARED camera.cpp processing.cpp wrapper.c)

if(NOT EXISTS "${GODOT_HEADERS}")
	if(NOT EXISTS "${GODOT_HEADERS_ZIP}")
		file(DOWNLOAD "https://github.com/GodotNativeTools/godot_headers/archive/master.zip" "${GODOT_HEADERS_ZIP}")
    endif()
	file(MAKE_DIRECTORY "${GODOT_HEADERS}")
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/godot-headers.zip WORKING_DIRECTORY "${GODOT_HEADERS}")
	file(RENAME "godot-headers-master" "godot-headers")
endif()

include_directories(${CMAKE_BINARY_DIR}/godot-headers)

