add_executable(lemon_cpp lemon.cpp)
add_executable(lemon_original lemon_original.c)
set_property(GLOBAL PROPERTY LEMPAR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lempar.c)