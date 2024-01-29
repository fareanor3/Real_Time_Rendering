cmake_minimum_required(VERSION 3.26)
project(RealtimeRendering C)

# Setup the standards we want.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Lookup the source files and all that stuff
file(GLOB_RECURSE C_BIN_HEADERS "./RealTimeRendering/*.h")
file(GLOB_RECURSE C_BIN_SOURCES "./RealTimeRendering/*.c")

find_package(OpenMP)
find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)

# Use pkg-config to find SDL2_image
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)

include_directories(/usr/lib/llvm-16/lib/clang/16/include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_executable(
        ${PROJECT_NAME}
        ${C_BIN_HEADERS}
        ${C_BIN_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE m ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})