
cmake_minimum_required(VERSION 2.8.9)

project (sample_example)
project (randomize_picture_for_beamer)

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14 ")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++14 ")
endif()

find_package(OpenCV)

include_directories(include)


#add_executable(sample_example source/sample_example.cpp source/superpixel.cpp source/pixel.cpp source/point.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp source/sorting/portioner.cpp)
add_executable(sample_example source/sample_example.cpp source/superpixel.cpp source/pixel.cpp source/point.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp )
target_link_libraries( sample_example ${OpenCV_LIBS} )


add_executable(randomize_picture_for_beamer source/randomize_picture_for_beamer.cpp source/superpixel.cpp source/pixel.cpp source/point.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp )
target_link_libraries( randomize_picture_for_beamer ${OpenCV_LIBS} )

# add_executable(allfred source/allfred.cpp source/pixel.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp source/librarys/delaunator.hpp)
# target_link_libraries( allfred ${OpenCV_LIBS} )
