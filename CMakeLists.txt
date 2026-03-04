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

execute_process(
        COMMAND git log -1 --format=%H
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(mirp-lab-1)
target_sources(mirp-lab-1 PRIVATE
        src/to_string.c
        src/vector.c
        src/topology.c
        src/parser.c
        src/cli.c
        src/main.c
)

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(mirp-lab-1 PRIVATE
            -Weverything

            -Wno-extra-semi
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-declaration-after-statement
            -Wno-unsafe-buffer-usage
            -Wno-strict-prototypes
            -Wno-padded
            -Wno-implicit-void-ptr-cast
            -Wno-switch-default
    )
endif ()

target_compile_options(mirp-lab-1 PRIVATE
        -Wall
        -Wextra
        # -Werror
)

target_compile_definitions(mirp-lab-1 PRIVATE VERSION="${GIT_HASH}")

enable_testing()

add_test(
        NAME test-mirp-lab-1
        COMMAND $<TARGET_FILE:mirp-lab-1>
        ${CMAKE_SOURCE_DIR}/assets/topology-2.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
