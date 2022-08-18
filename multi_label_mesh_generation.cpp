#ifndef INITIAL_OPENGL
#define INITIAL_OPENGL

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
#endif


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Image_3.h>
#include <CGAL/license/Triangulation_3.h>
#include <CGAL/facets_in_complex_3_to_triangle_mesh.h>
#include <CGAL/Mesh_constant_domain_field_3.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Tetrahedral_remeshing/Remeshing_triangulation_3.h>
#include <CGAL/tetrahedral_remeshing.h>
#include "/home/SENSETIME/xulixin2/code/cgal-master/Tetrahedral_remeshing/examples/Tetrahedral_remeshing/tetrahedral_remeshing_generate_input.h"

#include <iostream>
#include <fstream>
#include <string>

#include <CGAL/IO/output_to_vtu.h>
#include <CGAL/IO/File_binary_mesh_3.h>
#include <CGAL/IO/File_medit.h>
#include <CGAL/IO/File_tetgen.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPoints.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <CGAL/IO/io.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Labeled_mesh_domain_3<K> Mesh_domain;
typedef CGAL::Tetrahedral_remeshing::Remeshing_triangulation_3<K> Remeshing_triangulation;
#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif
// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3T3;
typedef CGAL::Mesh_constant_domain_field_3<Mesh_domain::R,Mesh_domain::Index> Sizing_field;
// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;
using namespace std;
//using namespace CGAL;
typedef typename C3T3::Triangulation::Geom_traits::Point_3             Point;
typedef std::vector<std::size_t>                                       Face;
typedef std::vector<Point> Points;
typedef std::vector<Face> Faces;


void user_defined_output_boundary_of_c3t3(const C3T3& c3t3,
                               typename C3T3::Subdomain_index sd_index,
                               Points &visual_points,
                               Faces &visual_faces,
                               bool normals_point_outside_of_the_subdomain = true)
{
    CGAL::Mesh_3::internal::facets_in_complex_3_to_triangle_soup(c3t3, sd_index, visual_points, visual_faces, normals_point_outside_of_the_subdomain);
}

void remeshing(const std::string file_name,const double target_edge_length)

{
    const char* file_name_cstr = file_name.c_str();
    std::ifstream input(file_name_cstr, std::ios_base::in);
    Remeshing_triangulation tr;
    CGAL::IO::read_MEDIT(input, tr);
    // get egdes length of the mesh
    CGAL::tetrahedral_isotropic_remeshing(tr, target_edge_length);
    std::ofstream output("output.mesh", std::ios_base::out);
    CGAL::IO::write_MEDIT(output, tr);
}

int main(int argc, char* argv[])
{
//    string file_name = "all_liver.mesh";
//    double target_edge_length = 1;
//    remeshing(file_name,target_edge_length);
//    return 0;
//



    const std::string fname = (argc>1)?argv[1]:CGAL::data_file_path("/home/SENSETIME/xulixin2/merged_liver.inr");
    std::cout<<fname<<std::endl;
    CGAL::Image_3 image;
    if(!image.read(fname)){
        std::cerr << "Error: Cannot read file " <<  fname << std::endl;
        return EXIT_FAILURE;
    }
    Mesh_domain domain = Mesh_domain::create_labeled_image_mesh_domain(image);
    double vessel_size = 5;

    Sizing_field size(15);
    size.set_size(vessel_size,3,domain.index_from_subdomain_index(2));
    size.set_size(vessel_size,3,domain.index_from_subdomain_index(1));

    // Mesh criteria
    Mesh_criteria criteria(facet_angle=25, facet_size=8, facet_distance=4,
                           cell_radius_edge_ratio=3, cell_size=size);
    C3T3 c3t3 = CGAL::make_mesh_3<C3T3>(domain, criteria);
    CGAL::refine_mesh_3(c3t3,domain,criteria);
    // Output
    std::ofstream medit_file("simple_liver.mesh");
    std::ofstream vtu_file("simple_liver.vtu");

    Points visual_surface_points, visual_interior_vessel_points;
    Faces visual_surface_faces, visual_interior_vessel_faces;
    user_defined_output_boundary_of_c3t3(c3t3,4,visual_surface_points,visual_surface_faces);
    user_defined_output_boundary_of_c3t3(c3t3,1,visual_interior_vessel_points,visual_interior_vessel_faces);
    c3t3.output_to_medit(medit_file);
    CGAL::IO::output_to_vtu(vtu_file,c3t3,CGAL::IO::ASCII);
    CGAL::IO::output_to_tetgen("simple_tetgen",c3t3);

    std::ofstream outFile;
    outFile.open("simple.off");

    // Output
    std::stringstream off_file;

    c3t3.output_facets_in_complex_to_off(off_file);
//    c3t3.output_boundary_to_off(off_file);
    assert( off_file.str().size() > 20 );

    outFile << off_file.rdbuf();
    outFile.close();




    return 0;
}