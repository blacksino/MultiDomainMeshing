# ensure python version is 3.8 or higher
import sys
# assert sys.version_info >= (3, 8)
import meshio
import numpy as np
import os



# convert vtu to vtk
def vtu2vtk(vtu_path, vtk_path):
    mesh = meshio.read(vtu_path)
    # translate mesh to its center
    mesh.points = mesh.points - np.mean(mesh.points, axis=0)
    meshio.vtk.write(vtk_path,mesh,fmt_version="4.2")


def update_vtk(vtk_path,deformed_vtu_path):
    new_vtk_path = vtk_path.replace(".vtk","_deformed.vtk")
    d_mesh = meshio.read(deformed_vtu_path)
    mesh = meshio.read(vtk_path)
    mesh.points = d_mesh.points
    meshio.vtk.write(new_vtk_path,mesh,fmt_version="4.2")


def update_tetgen_node(node_path,deformed_vtu_path):
    new_node_path = node_path.replace(".node","_deformed.node")
    d_mesh = meshio.read(deformed_vtu_path)
    meshio.write(new_node_path,d_mesh,file_format="tetgen")
    os.remove(new_node_path.replace(".node",".ele"))

def copy_label(original_vtu_path,vtk_path):
    mesh = meshio.read(original_vtu_path)
    d_mesh = meshio.read(vtk_path)
    new_mesh = mesh.copy()
    new_mesh.points = d_mesh.points
    meshio.vtk.write(vtk_path,new_mesh,fmt_version="4.2")




# vtu2vtk("/home/SENSETIME/xulixin2/RJ_demo/mesh/all.vtu",
#         "/home/SENSETIME/xulixin2/RJ_demo/mesh/all.vtk")
if __name__ == '__main__':
    # vtu2vtk("/home/SENSETIME/xulixin2/RJ_demo/mesh/all.vtu","/home/SENSETIME/xulixin2/RJ_demo/mesh/all.vtk")
    # print("Now,you could simulate deformation using SOFA.\n")
    vtu2vtk("/home/SENSETIME/xulixin2/RJ_demo/mesh/deformed.vtu","/home/SENSETIME/xulixin2/RJ_demo/mesh/deformed.vtk")
    copy_label("/home/SENSETIME/xulixin2/RJ_demo/mesh/all.vtu","/home/SENSETIME/xulixin2/RJ_demo/mesh/deformed.vtk")
    update_tetgen_node("/home/SENSETIME/xulixin2/RJ_demo/mesh/all.node","/home/SENSETIME/xulixin2/RJ_demo/mesh/deformed.vtu")
