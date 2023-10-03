# copy the source files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/cholesky.c ${CMAKE_CURRENT_BINARY_DIR}/cholesky.c COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/multiplication.c ${CMAKE_CURRENT_BINARY_DIR}/multiplication.c COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/toy.c ${CMAKE_CURRENT_BINARY_DIR}/toy.c COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/blink.c ${CMAKE_CURRENT_BINARY_DIR}/blink.c COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/copy.c ${CMAKE_CURRENT_BINARY_DIR}/copy.c COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../examples/stencil.c ${CMAKE_CURRENT_BINARY_DIR}/stencil.c COPYONLY)


# build and register the tests
include_directories (${GTEST_INCLUDE_DIRS})

add_executable (CholeskyTest CholeskyTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (CholeskyTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(CholeskyTest "" CholeskyTest.cpp)

add_executable (MultiplicationTest MultiplicationTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (MultiplicationTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(MultiplicationTest "" MultiplicationTest.cpp)

add_executable (ToyTest ToyTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (ToyTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(ToyTest "" ToyTest.cpp)

add_executable (BlinkTest BlinkTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (BlinkTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(BlinkTest "" BlinkTest.cpp)

add_executable (CopyTest CopyTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (CopyTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(CopyTest "" CopyTest.cpp)

add_executable (StencilTest StencilTest.cpp CacheEmulator.cpp $<TARGET_OBJECTS:CacheModel>)
target_link_libraries (StencilTest ${GTEST_LIBRARIES} pthread isl barvinok polylibgmp pet ntl m gmp)
gtest_add_tests(StencilTest "" StencilTest.cpp)