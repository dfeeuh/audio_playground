# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Define a C++ project.
project(audio_engine LANGUAGES CXX)

# standards version
set(CMAKE_CXX_STANDARD 17)

# Check for Jack (any OS)
#find_library(RTAUDIO rtaudio)
find_package(rtaudio REQUIRED)
if (rtaudio_FOUND)
  message(STATUS "RTAudio include: ${rtaudio_DIR}")

  add_executable(playsaw 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/AudioEngine.cpp)

  target_include_directories(playsaw PUBLIC ${rtaudio_DIR}/../../include/rtaudio)
  target_link_libraries(playsaw ${RTAUDIO})

else ()
  message(WARNING "Could not find RTAUDIO SDK")
endif ()

