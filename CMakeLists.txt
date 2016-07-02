cmake_minimum_required (VERSION 3.2)
project(chartsampler)
set(chartsampler_VERSION_MAJOR 0)
set(chartsampler_VERSION_MINOR 1)

file(GLOB_RECURSE ${CMAKE_SOURCE_DIR}/src/*.c ${CMAKE_SOURCE_DIR}/src/*.h)

set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/main.c
    )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_CFLAGS} -lm")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Werror -Wall -Wextra -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -W -O3")

add_executable(chartsampler ${SRC_FILES})

if (UNIX)
    install(TARGETS chartsampler DESTINATION bin)
endif()
