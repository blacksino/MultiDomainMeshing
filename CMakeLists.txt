cmake_minimum_required(VERSION 3.20)
project(MultiDomainMeshing)
set(CMAKE_CXX_STANDARD 17)

find_package(VTK REQUIRED)
include_directories(${VTK_INCLUDE_DIRS})
link_directories(${VTK_LIBRARY_DIRS})
add_definitions(${VTK_DEFINITIONS})
#include_directories("/usr/local/include/vtk-9.1/")


SET(CGAL_DIR "/home/SENSETIME/xulixin2/code/cgal-master")
find_package(CGAL)

add_executable(MultiDomainMeshing multi_label_mesh_generation.cpp)
target_link_libraries(MultiDomainMeshing CGAL::CGAL)
target_link_libraries(MultiDomainMeshing ${VTK_LIBRARIES})

