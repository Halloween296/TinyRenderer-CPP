cmake_minimum_required(VERSION 3.14)
project(RetroRenderer CXX)

set(CMAKE_CXX_STANDARD 17)

# --- 路径设置 ---
# 定义 SDL2 的路径
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/vendor/SDL2")

# 头文件路径
include_directories(src)
include_directories("${SDL2_PATH}/include")

# --- 源文件 ---
file(GLOB SOURCES "src/*.cpp" "src/*.hpp")

add_executable(RetroRenderer ${SOURCES})


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SDL2_LIB_NAME "SDL2d.lib") 
else()
    set(SDL2_LIB_NAME "SDL2.lib")
endif()

# 强制链接 x64 版本的 lib
target_link_libraries(RetroRenderer PRIVATE "${SDL2_PATH}/lib/x64/SDL2.lib")
target_link_libraries(RetroRenderer PRIVATE "${SDL2_PATH}/lib/x64/SDL2main.lib")

add_custom_command(TARGET RetroRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_PATH}/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:RetroRenderer>)