cmake_minimum_required(VERSION 3.28)
project("Cpp Core")

set(ShouldTest CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
if(${ShouldTest})
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
endif()

#################################
# Drandall Code
#################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_definitions(-DGNU)
    else()
        add_definitions(-DCLANG)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DMSVC)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG)
else()
    add_definitions(-DRELEASE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Core)
if(${ShouldTest})
    add_subdirectory(CoreTest)
endif()

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
