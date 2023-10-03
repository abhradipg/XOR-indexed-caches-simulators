cmake_minimum_required (VERSION 3.9)

project (absinthe)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(experimentConfigurations baseline noequal noraster onecache threecaches enumerateall polycache)
set(EXPERIMENT baseline CACHE STRING "experimental configuration")
set_property(CACHE EXPERIMENT PROPERTY STRINGS ${experimentConfigurations})

if("${EXPERIMENT}" STREQUAL "baseline")
    add_definitions(-DRASTERIZATION -DEQUALIZATION)
elseif("${EXPERIMENT}" STREQUAL "noraster")
    add_definitions(-DEQUALIZATION)
elseif("${EXPERIMENT}" STREQUAL "onecache")
    add_definitions(-DRASTERIZATION -DEQUALIZATION -DONE_CACHE)
elseif("${EXPERIMENT}" STREQUAL "threecaches")
    add_definitions(-DRASTERIZATION -DEQUALIZATION -DTHREE_CACHES)
elseif("${EXPERIMENT}" STREQUAL "polycache")
    add_definitions(-DRASTERIZATION -DEQUALIZATION -DPOLY_CACHE)
elseif("${EXPERIMENT}" STREQUAL "enumerateall")
    add_definitions(-DRASTERIZATION -DEQUALIZATION -DENUMERATE_POINTS)
endif()

# include google test
enable_testing()
set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/lib/googletest CACHE PATH "google path")
find_package(GTest REQUIRED)

# include boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

# isl c++ headers
include_directories (${CMAKE_SOURCE_DIR}/include)

set(POLY_BASE ${CMAKE_SOURCE_DIR}/../../ CACHE FILEPATH "polyhedral dependencies directory")

# isl
set(ISL_INC_DIR include CACHE FILEPATH "isl include directory respective to the poly prefix")
set(ISL_LIB_DIR lib CACHE FILEPATH "isl library directory respective to the poly prefix")
include_directories (${POLY_BASE}/${ISL_INC_DIR})
link_directories (${POLY_BASE}/${ISL_LIB_DIR})

# barvinok
set(BARVINOK_INC_DIR include CACHE FILEPATH "barvinok include directory respective to the poly prefix")
set(BARVINOK_LIB_DIR lib CACHE FILEPATH "barvinok library directory respective to the poly prefix")
set(POLY_LIB_DIR lib CACHE FILEPATH "polylib library directory respective to the poly prefix")
include_directories (${POLY_BASE}/${BARVINOK_INC_DIR})
link_directories (${POLY_BASE}/${BARVINOK_LIB_DIR})
link_directories (${POLY_BASE}/${POLY_LIB_DIR})

# pet
set(PET_INC_DIR include CACHE FILEPATH "pet include directory respective to the poly prefix")
set(PET_LIB_DIR lib CACHE FILEPATH "pet library directory respective to the poly prefix")
include_directories (${POLY_BASE}/${PET_INC_DIR})
link_directories (${POLY_BASE}/${PET_LIB_DIR})

# llvm for linking clang statically
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
link_directories(${LLVM_LIBRARY_DIRS})

# add the errors directory
add_subdirectory(./errors)

# add the source directory
add_subdirectory(./src)

# add the test directory
add_subdirectory(./test)
