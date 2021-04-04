# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Define a C++ project.
project(audio_engine LANGUAGES CXX)

# standards version
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/AudioEngine)

add_executable(playsaw 
    ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(playsaw PUBLIC AudioEngine)

target_include_directories(playsaw PUBLIC
                          "${PROJECT_SOURCE_DIR}/libs/AudioEngine"
                          )

