cmake_minimum_required(VERSION 3.5)
project( execute )
macro(use_cxx_std)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endmacro(use_cxx_std)

macro(use_pedantic_errors)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
endmacro(use_pedantic_errors)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -D_GLIBCXX_DEBUG")

option (RelWithDebInfo "build release with debug symbols - same as in opencv" OFF)
if(RelWithDebInfo)
#-march=native
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -ggdb -g3")
endif()


use_cxx_std()
use_pedantic_errors()

file(GLOB source_files *.cpp)
add_executable(execute ${source_files})

find_package(OpenCV REQUIRED)
target_link_libraries( execute ${OpenCV_LIBS} )

include_directories(/home/szozda/git/imgwarp-opencv/src/lib)
target_link_libraries( execute /home/szozda/git/imgwarp-opencv/src/lib/libimgwarp-lib.a -latomic -lpthread)