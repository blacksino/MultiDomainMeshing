cmake_minimum_required(VERSION 3.20)
project(MultiDomainMeshing)

#set(Boost_NO_BOOST_CMAKE ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_ROOT "/home/SENSETIME/xulixin2/Downloads/boost_1_68_0")

# check Boost version
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")



set(CGAL_DIR "/home/SENSETIME/xulixin2/code/cgal-master")
#set(Boost_DIR "/home/SENSETIME/xulixin2/Downloads/Boost_1_68_0/Boost_1.68_install")
#set(BOOST_INCLUDEDIR /home/SENSETIME/xulixin2/Downloads/boost_1_68_0/boost_1.68_install/include)
#set(BOOST_LIBRARYDIR /home/SENSETIME/xulixin2/Downloads/boost_1_68_0/boost_1.68_install/lib)
message(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost_LIBRARY_DIR: ${Boost_LIBRARYDIR}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
find_package(CGAL)


add_executable(MultiDomainMeshing multi_label_mesh_generation.cpp)


target_link_libraries(MultiDomainMeshing CGAL::CGAL)
target_link_libraries(MultiDomainMeshing ${VTK_LIBRARIES})
target_link_libraries(MultiDomainMeshing ${Boost_LIBRARIES})
target_link_directories(MultiDomainMeshing PUBLIC "/home/SENSETIME/xulixin2/Downloads/boost_1_68_0/boost_1.68_install/lib")

