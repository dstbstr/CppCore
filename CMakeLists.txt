cmake_minimum_required(VERSION 3.16)
project("Cpp Core")

include(FetchContent)

#################################
# GTest
#################################
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#################################
# Drandall Code
#################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_definitions(-DGNU)
    else()
        add_definitions(-DCLANG)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
    add_definitions(-DMSVC)
endif()
if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG)
else()
    add_definitions(-DRELEASE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Core)
add_subdirectory(CoreTest)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
