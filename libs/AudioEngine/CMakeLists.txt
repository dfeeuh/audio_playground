# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Define a C++ project.
project(AudioEngine LANGUAGES CXX)

# standards version
set(CMAKE_CXX_STANDARD 17)

# Check for Jack (any OS)
#find_library(RTAUDIO rtaudio)
find_package(rtaudio REQUIRED)
find_library(RTAUDIO rtaudio)
if (rtaudio_FOUND)
  message(STATUS "RTAudio include: ${rtaudio_DIR}, ${RTAUDIO}")

  add_library(AudioEngine 
    ${PROJECT_SOURCE_DIR}/src/AudioEngine.cpp)

  target_include_directories(AudioEngine PUBLIC 
                    "${PROJECT_SOURCE_DIR}"
                    "${rtaudio_DIR}/../../include/rtaudio")
  target_link_libraries(AudioEngine ${RTAUDIO})

else ()
  message(WARNING "Could not find RTAUDIO SDK")
endif ()

