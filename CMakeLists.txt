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

file(GLOB libboost
    "/usr/local/Cellar/boost/1.70.0/lib/*"
)

find_package(boost REQUIRED)
include_directories(libsmina ${Boost_INCLUDE_DIRS})
target_link_libraries(libsmina ${Boost_LIBRARIES})
target_link_libraries(libsmina ${libboost})
message("-- Found Boost include files at ${Boost_INCLUDE_DIRS}")
message("-- Found Boost libraries at ${Boost_LIBRARIES}")


find_package(Eigen3 REQUIRED)
include_directories(libsmina ${EIGEN3_INCLUDE_DIRS})
message("-- Found Eigen3 include files at ${EIGEN3_INCLUDE_DIRS}")

find_package(OpenBabel2 REQUIRED)
include_directories(libsmina ${OPENBABEL2_INCLUDE_DIR})
target_link_libraries(libsmina ${OPENBABEL2_LIB})

add_executable(smina src/main/main.cpp)
target_link_libraries (smina LINK_PUBLIC libsmina)