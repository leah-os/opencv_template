cmake_minimum_required(VERSION 3.10)

file(GLOB HDR_FILES src/*.h)
file(GLOB SRC_FILES src/*.cpp)

add_executable(Client src/main.cpp)

source_group(
	TREE ${PROJECT_SOURCE_DIR}
	FILES ${SRC_FILES} ${HDR_FILES}
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Client PRIVATE ${OpenCV_LIBS} PRIVATE Ws2_32)

set(OpenCV_SHARED On)
set_property(GLOBAL PROPERTY USE_FOLDERS On)
