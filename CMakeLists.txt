cmake_minimum_required(VERSION 3.31)

project(mirp-lab-1 VERSION 1.0.0 LANGUAGES CXX)

add_executable(mirp-lab-1)
target_sources(mirp-lab-1 PRIVATE src/main.cpp)

enable_testing()

add_test(
    NAME test-mirp-lab-1
    COMMAND $<TARGET_FILE:mirp-lab-1>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
