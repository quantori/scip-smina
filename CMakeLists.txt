cmake_minimum_required(VERSION 3.10)
project(smina)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

file(GLOB sminasrc
    "src/lib/*.h"
    "src/lib/*.cpp"
)

add_library(libsmina
    ${sminasrc}
)

target_include_directories(libsmina PUBLIC src/lib/)

find_package(Boost REQUIRED COMPONENTS 
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

find_package(OpenBabel2)
if(${OPENBABEL2_FOUND})
  include_directories(libsmina ${OPENBABEL2_INCLUDE_DIR})
  target_link_libraries(libsmina ${OPENBABEL2_LIBRARIES})
  message("-- Found OpenBabel2 include files at  ${OPENBABEL2_INCLUDE_DIR}")
  message("-- Found OpenBabel2 libraries at  ${OPENBABEL2_LIBRARIES}")
else()
  find_package(OpenBabel3 REQUIRED)
  include_directories(libsmina ${OPENBABEL3_INCLUDE_DIR})
  target_link_libraries(libsmina ${OPENBABEL3_LIBRARIES})
  message("-- Found OpenBabel3 include files at ${OPENBABEL3_INCLUDE_DIR}")
  message("-- Found OpenBabel3 libraries at ${OPENBABEL3_LIBRARIES}")
endif()

add_executable(smina src/main/main.cpp)
target_link_libraries(smina libsmina)