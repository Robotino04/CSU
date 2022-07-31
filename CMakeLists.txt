cmake_minimum_required(VERSION 3.0)

project(CSU)
SET(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE CSU_SOURCE "src/CSU/*.cpp" "src/CSU/*.hpp")
file(GLOB_RECURSE EMULATOR_SOURCE "src/emulator/*.cpp" "src/emulator/*.hpp")
file(GLOB_RECURSE ASSEMBLER_SOURCE "src/assembler/*.cpp" "src/assembler/*.hpp")

add_library(CSU STATIC ${CSU_SOURCE})
target_include_directories(CSU PUBLIC "src/CSU/include/")

add_executable(emulator ${EMULATOR_SOURCE})
target_link_libraries(emulator PUBLIC CSU)

add_executable(assembler ${ASSEMBLER_SOURCE})
target_include_directories(assembler PUBLIC "src/assembler/include/")
