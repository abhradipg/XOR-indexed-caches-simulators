
add_executable(Errors main.cpp)
target_link_libraries (Errors isl barvinok polylibgmp pet ntl m gmp ${Boost_LIBRARIES} -lpthread -lm) 