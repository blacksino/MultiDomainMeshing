cmake_minimum_required(VERSION 3.20)
project(MultiDomainMeshing)
set(CMAKE_CXX_STANDARD 17)


# ensure boost version is 1.68.0.
find_package(Boost REQUIRED)
if(NOT Boost_VERSION VERSION_EQUAL "1.68.0")
    message(FATAL_ERROR "Boost version  must be 1.68")
    exit(FATAL_ERROR)
endif()


SET(CGAL_DIR "/home/SENSETIME/xulixin2/code/cgal-master")
find_package(CGAL)

add_executable(MultiDomainMeshing multi_label_mesh_generation.cpp)
target_link_libraries(MultiDomainMeshing CGAL::CGAL)
target_link_libraries(MultiDomainMeshing ${VTK_LIBRARIES})

