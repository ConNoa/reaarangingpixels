
cmake_minimum_required(VERSION 2.8.9)

project (sample_example)
project (allfred)

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14 ")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++14 ")
endif()

find_package(OpenCV)

include_directories(include)


add_executable(sample_example source/sample_example.cpp source/pixel.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp)
target_link_libraries( sample_example ${OpenCV_LIBS} )

add_executable(allfred source/allfred.cpp source/pixel.cpp source/sampling/sample.cpp source/interpretation/interpret.cpp source/evaluation/evaluate.cpp)
target_link_libraries( allfred ${OpenCV_LIBS} )
