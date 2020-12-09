cmake_minimum_required(VERSION 3.5)
project(smina)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/Modules)

file(GLOB sminasrc
    "src/lib/*.h"
    "src/lib/*.cpp"
)
list(APPEND sminasrc "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

message(${CMAKE_CURRENT_BINARY_DIR})
# Add a custom command that produces version.cpp, plus
# a dummy output that's not actually produced, in order
# to force version.cmake to always be re-run before the build
add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/_version.cpp
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/version.cpp           
    COMMAND ${CMAKE_COMMAND} -P
            ${CMAKE_SOURCE_DIR}/CMake/version.cmake)
add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_version.cpp)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/_version.cpp PROPERTIES SYMBOLIC TRUE)

add_library(libsmina
    ${sminasrc}
)
set_target_properties(libsmina PROPERTIES OUTPUT_NAME "smina")
target_include_directories(libsmina PUBLIC src/lib/)
set(Boost_NO_BOOST_CMAKE ON)  #apparently required for newer boosts?
find_package(Boost REQUIRED  COMPONENTS 
  iostreams 
  timer 
  system 
  regex 
  thread 
  serialization 
  filesystem 
  program_options 
  date_time
)
include_directories(libsmina ${Boost_INCLUDE_DIRS})
target_link_libraries(libsmina ${Boost_LIBRARIES})
message("-- Found Boost include files at ${Boost_INCLUDE_DIRS}")
message("-- Found Boost libraries at ${Boost_LIBRARIES}")

find_package(Eigen3 REQUIRED)
include_directories(libsmina ${EIGEN3_INCLUDE_DIRS})
message("-- Found Eigen3 include files at ${EIGEN3_INCLUDE_DIRS}")

#flexible docking implementation requires features of openbabel3
find_package(OpenBabel3 REQUIRED)
include_directories(libsmina ${OPENBABEL3_INCLUDE_DIR})
target_link_libraries(libsmina ${OPENBABEL3_LIBRARIES})
message("-- Found OpenBabel3 include files at ${OPENBABEL3_INCLUDE_DIR}")
message("-- Found OpenBabel3 libraries at ${OPENBABEL3_LIBRARIES}")

add_executable(smina src/main/main.cpp)
target_link_libraries(smina libsmina)
target_link_libraries(smina Threads::Threads)
