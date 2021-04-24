cmake_minimum_required(VERSION 3.1)
project(GodotCV)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bouncy/")
set(GODOT_HEADERS "${CMAKE_SOURCE_DIR}/godot-headers")
set(GODOT_HEADERS_ZIP "${CMAKE_SOURCE_DIR}/godot-headers.zip")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -fPIC")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -g -shared")

find_package(OpenCV REQUIRED)
include_directories(AFTER "${OpenCV_INCLUDE_DIRS}" "include" "godot-headers")
link_libraries(${OpenCV_LIBS})

add_library(bouncy SHARED camera.cpp processing.cpp wrapper.c)

# Check contents of godot-headers; if empty, fetch and extract godot-headers-master.zip
file(GLOB RESULT ${GODOT_HEADERS})
list(LENGTH RESULT RES_LEN)
if(RES_LEN EQUAL 0)
	# if there's no godot_headers/, fetch it from the main repo
	if(NOT EXISTS "${GODOT_HEADERS_ZIP}")
		file(DOWNLOAD "https://github.com/godotengine/godot-headers/archive/master.zip" "${GODOT_HEADERS_ZIP}"
			STATUS DOWNLOAD_STATUS
		)
		list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
		list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
		if(${STATUS_CODE} EQUAL 1)
			message(FATAL_ERROR "Error occurred during download: ${ERROR_MESSAGE}")
		endif()
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${GODOT_HEADERS_ZIP} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	file(RENAME "godot-headers-master" "godot-headers")
endif()

include_directories(${CMAKE_BINARY_DIR}/godot-headers)

