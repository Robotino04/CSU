cmake_minimum_required(VERSION 3.0)

project(CSU)

file(GLOB_RECURSE CSU_SOURCE "src/CSU/*.cpp" "src/CSU/*.hpp")

add_library(CSU STATIC ${CSU_SOURCE})
target_include_directories(CSU PUBLIC "src/CSU/include/")

add_executable(emulator "src/main.cpp")
target_link_libraries(emulator PUBLIC CSU)