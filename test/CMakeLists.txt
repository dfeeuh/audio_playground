
# Unit Testing
add_subdirectory(googletest)  

add_executable(unit_test 
            unit_test.cpp)

target_link_libraries(unit_test PUBLIC 
        gtest_main
        AudioEngine
        )

target_include_directories(unit_test PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/libs/AudioEngine"
)
