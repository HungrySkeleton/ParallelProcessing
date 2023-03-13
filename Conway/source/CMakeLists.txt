cmake_minimum_required(VERSION 3.10)

project(Tutorial VERSION 1.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(MPI REQUIRED)

# configure_file(Game.hpp Game.hpp)

add_executable(Tutorial main.cpp Game.cpp)

target_include_directories(Tutorial PUBLIC ${PROJECT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${MPI_C_INCLUDE_PATH})
target_link_libraries(Tutorial ${OpenCV_LIBS} ${MPI_C_LIBRARIES} ) 

# cmake_minimum_required(VERSION 2.8.12)

# project(GOL)

# add_definitions(-DOMPI_SKIP_MPICXX)

# find_package(OpenCV REQUIRED)
# find_package(MPI REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(${MPI_C_INCLUDE_PATH})

# add_executable(p1 GOL.cpp)


# target_link_libraries(p1 ${OpenCV_LIBS} ${MPI_C_LIBRARIES})
