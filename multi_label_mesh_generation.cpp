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

int main(int argc, char* argv[])
{
    const std::string fname = (argc>1)?argv[1]:CGAL::data_file_path("/home/SENSETIME/xulixin2/Desktop/merged_liver.inr");
    std::cout<<fname<<std::endl;
    CGAL::Image_3 image;
    if(!image.read(fname)){
        std::cerr << "Error: Cannot read file " <<  fname << std::endl;
        return EXIT_FAILURE;
    }
    Mesh_domain domain = Mesh_domain::create_labeled_image_mesh_domain(image);
    double vessel_size = 3;
    Sizing_field size(10);
    size.set_size(vessel_size,3,domain.index_from_subdomain_index(2));
    size.set_size(vessel_size,3,domain.index_from_subdomain_index(1));
    // Mesh criteria
    Mesh_criteria criteria(facet_angle=25, facet_size=4, facet_distance=4,
                           cell_radius_edge_ratio=3, cell_size=size);
    C3T3 c3t3 = CGAL::make_mesh_3<C3T3>(domain, criteria);
    CGAL::refine_mesh_3(c3t3,domain,criteria);
    // Output
    std::ofstream medit_file("all_liver.mesh");
    std::ofstream vtu_file("all_liver.vtu");

    Points visual_surface_points, visual_interior_vessel_points;
    Faces visual_surface_faces, visual_interior_vessel_faces;
    user_defined_output_boundary_of_c3t3(c3t3,4,visual_surface_points,visual_surface_faces);
    user_defined_output_boundary_of_c3t3(c3t3,1,visual_interior_vessel_points,visual_interior_vessel_faces);
    c3t3.output_to_medit(medit_file);
    CGAL::IO::output_to_vtu(vtu_file,c3t3,CGAL::IO::ASCII);
    CGAL::IO::output_to_tetgen("test_tetgen",c3t3);

    std::ofstream outFile;
    outFile.open("test.off");


    // Output
    std::stringstream off_file;

    c3t3.output_facets_in_complex_to_off(off_file);
//    c3t3.output_boundary_to_off(off_file);
    assert( off_file.str().size() > 20 );

    outFile << off_file.rdbuf();
    outFile.close();


//    vtkSmartPointer<vtkPolyData> surface_poly_data = vtkSmartPointer<vtkPolyData>::New();
//    vtkSmartPointer<vtkPolyData> vessel_poly_data = vtkSmartPointer<vtkPolyData>::New();
//
//    vtkSmartPointer<vtkPoints> surface_points = vtkSmartPointer<vtkPoints>::New();
//    vtkSmartPointer<vtkPoints> vessel_points = vtkSmartPointer<vtkPoints>::New();
//
//    for(long long i= 0;i<visual_surface_points.size();i++)
//        surface_points->InsertPoint(i,visual_surface_points[i].x(),visual_surface_points[i].y(),visual_surface_points[i].z());
//    for(long long i= 0;i<visual_interior_vessel_points.size();i++)
//        vessel_points->InsertPoint(i,visual_interior_vessel_points[i].x(),visual_interior_vessel_points[i].y(),visual_interior_vessel_points[i].z());
//
//    vtkSmartPointer<vtkCellArray> surface_cells = vtkSmartPointer<vtkCellArray>::New();
//    vtkSmartPointer<vtkCellArray> vessel_cells = vtkSmartPointer<vtkCellArray>::New();
//
//    for(size_t i= 0;i<visual_surface_faces.size();i++)
//    {
//        vtkIdType id[3] = {static_cast<long long>(visual_surface_faces[i][0]),
//                           static_cast<long long>(visual_surface_faces[i][1]),
//                           static_cast<long long>(visual_surface_faces[i][2])};
//        surface_cells->InsertNextCell(3, id);
//    }
//
//    for(size_t i= 0;i<visual_interior_vessel_faces.size();i++)
//    {
//        vtkIdType id[3] = {static_cast<long long>(visual_interior_vessel_faces[i][0]),
//                           static_cast<long long>(visual_interior_vessel_faces[i][1]),
//                           static_cast<long long>(visual_interior_vessel_faces[i][2])};
//        surface_cells->InsertNextCell(3, id);
//    }
//
//
//    surface_poly_data->SetPoints(surface_points);
//    surface_poly_data->SetPolys(surface_cells);
//
//    vessel_poly_data->SetPoints(vessel_points);
//    vessel_poly_data->SetPolys(vessel_cells);

//    vtkSmartPointer<vtkAppendPolyData> appendFilter =
//            vtkSmartPointer<vtkAppendPolyData>::New();
//    appendFilter->AddInputData(surface_poly_data);
//    appendFilter->AddInputData(vessel_poly_data);
//    appendFilter->Update();

//    vtkSmartPointer<vtkPolyDataMapper> mapper =
//            vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInputData(vessel_poly_data);
//    vtkSmartPointer<vtkActor>  actor =
//            vtkSmartPointer<vtkActor>::New();
//    actor->SetMapper(mapper);
//    actor->GetProperty()->SetOpacity(0.2);
//    vtkSmartPointer<vtkRenderer> render =
//            vtkSmartPointer<vtkRenderer>::New();
//    render->AddActor(actor);
//    render->SetBackground(0, 0, 0);
//
//    vtkSmartPointer<vtkRenderWindow> rw =
//            vtkSmartPointer<vtkRenderWindow>::New();
//    rw->AddRenderer(render);
//    rw->SetSize(1280, 960);
//    rw->SetWindowName("PolyData Structure Learning");
//    rw->Render();
//
//    vtkSmartPointer<vtkRenderWindowInteractor> rwi =
//            vtkSmartPointer<vtkRenderWindowInteractor>::New();
//    rwi->SetRenderWindow(rw);
//    rwi->Initialize();
//    rwi->Start();

    return 0;
}