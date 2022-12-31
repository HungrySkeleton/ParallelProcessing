cmake_minimum_required(VERSION 2.8.12)

project(lab2)

add_definitions(-DOMPI_SKIP_MPICXX)

find_package(OpenCV REQUIRED)
find_package(MPI REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${MPI_C_INCLUDE_PATH})

add_executable(p1 Lab_2_Part_1.cpp)
add_executable(p2 Lab_2_Part_2.cpp)
add_executable(p3 Lab_2_Part_3.cpp)

target_link_libraries(p1 ${OpenCV_LIBS} ${MPI_C_LIBRARIES})
target_link_libraries(p2 ${OpenCV_LIBS} ${MPI_C_LIBRARIES})
target_link_libraries(p3 ${OpenCV_LIBS} ${MPI_C_LIBRARIES})