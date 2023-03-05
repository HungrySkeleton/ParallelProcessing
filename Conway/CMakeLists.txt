cmake_minimum_required(VERSION 2.8.12)

project(GOL)

add_definitions(-DOMPI_SKIP_MPICXX)

find_package(OpenCV REQUIRED)
find_package(MPI REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${MPI_C_INCLUDE_PATH})

add_executable(p1 GOL.cpp)


target_link_libraries(p1 ${OpenCV_LIBS} ${MPI_C_LIBRARIES})
