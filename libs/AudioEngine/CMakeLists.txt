# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Define a C++ project.
project(AudioEngine LANGUAGES CXX)

# standards version
set(CMAKE_CXX_STANDARD 17)

find_package(rtmidi REQUIRED)
find_library(RTMIDI rtmidi)

find_package(rtaudio REQUIRED)
find_library(RTAUDIO rtaudio)
if (rtaudio_FOUND AND rtmidi_FOUND)
  message(STATUS "RTAudio include: ${rtaudio_DIR}, ${RTAUDIO}")
  message(STATUS "RTMidi include: ${rtmidi_DIR}, ${RTMIDI}")

  add_library(AudioEngine 
    ${PROJECT_SOURCE_DIR}/src/AudioEngine.cpp)

  target_include_directories(AudioEngine PUBLIC 
                    "${PROJECT_SOURCE_DIR}"
                    "${rtaudio_DIR}/../../include/rtaudio"
                    "/usr/local/include")

  target_link_libraries(AudioEngine 
                    ${RTAUDIO}
                    ${RTMIDI})

else ()
  message(WARNING "Could not find RTAUDIO SDK")
endif ()

