cmake_minimum_required(VERSION 3.31)

project(mirp-lab-1 VERSION 1.0.0 LANGUAGES C)


set(SANITIZERS_DESCRIPTION "Enable Address/Undefined Behavior Sanitizers")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(ENABLE_SANITIZERS OFF CACHE BOOL ${SANITIZERS_DESCRIPTION} FORCE)
else ()
    set(ENABLE_SANITIZERS ON CACHE BOOL ${SANITIZERS_DESCRIPTION} FORCE)
endif ()

if (ENABLE_SANITIZERS)
    message(STATUS "Sanitizers enabled")

    if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
    endif ()
endif ()

add_executable(mirp-lab-1)
target_sources(mirp-lab-1 PRIVATE src/vector.c src/topology.c src/parser.c src/main.c)

enable_testing()

add_test(
        NAME test-mirp-lab-1
        COMMAND $<TARGET_FILE:mirp-lab-1>
        ${CMAKE_SOURCE_DIR}/assets/topology-1.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
