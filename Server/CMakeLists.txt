cmake_minimum_required(VERSION 3.10)

file(GLOB HDR_FILES src/*.h)
file(GLOB SRC_FILES src/*.cpp)

add_executable(Server src/main.cpp)

source_group(
	TREE ${PROJECT_SOURCE_DIR}
	FILES ${SRC_FILES} ${HDR_FILES}
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Server PRIVATE ${OpenCV_LIBS} PRIVATE Ws2_32)

set(OpenCV_SHARED On)
set_property(GLOBAL PROPERTY USE_FOLDERS On)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Server)
