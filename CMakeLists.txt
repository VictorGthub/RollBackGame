cmake_minimum_required (VERSION 3.18)
project(RollbackGame)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/data.cmake)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /w14640)
    add_compile_definitions(_USE_MATH_DEFINES)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor)
endif()

add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(main)