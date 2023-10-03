file(GLOB SOURCE_FILES
        "./*"
        "./CacheState/*"
        "./Address/*"
        "./AffineFunction/*"
        "./IteratorState/*"
        "./ProgramInfo/*"
        "./Settings/*"
        "./Simulation/*"
        "./Snapshot/*"
        "./Util/*"
        "./Warping/*")

#find_path(PET_INCLUDE pet)
set(PET_INCLUDE /usr/local/include)
find_path(ISL_INCLUDE isl)

find_library(PET_LIB pet)
find_library(ISL_LIB isl)

message(STATUS "pet lib path: ${PET_LIB}")
message(STATUS "isl lib path: ${ISL_LIB}")
message(STATUS "pet link dir: ${PET_INCLUDE}")
message(STATUS "isl link dir: ${ISL_INCLUDE}")

set(LINK_LIBS ${PET_LIB} ${ISL_LIB})
set(LINK_DIRS ${PET_INCLUDE} ${ISL_INCLUDE})

add_library(warping-cache-simulation-lib ${SOURCE_FILES})
add_executable(warping-cache-simulation ${SOURCE_FILES})

target_link_libraries (warping-cache-simulation ${LINK_LIBS})
target_link_directories(warping-cache-simulation PUBLIC SYSTEM ${LINK_DIRS})

# TODO: find a better way to include source code in tests
target_link_libraries (warping-cache-simulation-lib ${LINK_LIBS})
target_link_directories(warping-cache-simulation-lib PUBLIC SYSTEM ${LINK_DIRS})
