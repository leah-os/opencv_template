cmake_minimum_required(VERSION 3.10)

project(OpenCV_Exp
	LANGUAGES C CXX
	VERSION 0.1
)

file(GLOB HDR_FILES src/*.h)
file(GLOB SRC_FILES src/*.cpp)

message("Header files" + ${HDR_FILES})
message("Source files" + ${SRC_FILES})

add_executable(OpenCV_Exp src/main.cpp)

source_group(
	TREE ${PROJECT_SOURCE_DIR}
	FILES ${SRC_FILES} ${HDR_FILES}
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(OpenCV_Exp ${OpenCV_LIBS})

set(OpenCV_SHARED On)
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})