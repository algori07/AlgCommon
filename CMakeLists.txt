cmake_minimum_required(VERSION 3.15)

project(AlgCommon LANGUAGES NONE)

add_library(AlgCommon INTERFACE
  include/DoCheckPointer.hpp
  
  include/EnumFlags.hpp
  
  include/BiMap.hpp
)

target_include_directories(AlgCommon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
