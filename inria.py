from os import path
import gzip
from io import StringIO
import cv2
import numpy as np
import matplotlib.pyplot as plt
import SimpleITK as sitk

def get_data_from_inria_image(inr_path):
    filename = inr_path
    if path.splitext(filename)[1] in (".gz", ".zip"):
        fzip = gzip.open(filename, "rb")
        f = StringIO(fzip.read())
        fzip.close()
    else:
        f = open(filename, "rb")

    header = ""
    while header[-4:] != "##}\n":
        header += f.read(256).decode("utf-8")  # convert bytes to string

    prop = {}
    h_start = header.find("{\n") + 1
    h_end = header.find("##}")

    infos = [gr for gr in header[h_start:h_end].split("\n") if len(gr) > 0]

    for prop_def in infos:
        key, val = prop_def.split("=")
        prop[key] = val

    # dict.pop(key, default_value): delete the key-value pair of the given key, return the value,
    # default_value will be returned if the specified key do not exist.
    # If default_value is not specified, and the no item with the specified key is found, an error is raised
    xdim, ydim, zdim, vdim = int(prop.pop("XDIM")), int(prop.pop("YDIM")), int(prop.pop("ZDIM")), int(prop.pop("VDIM"))
    pix_size = int(prop.pop("PIXSIZE", "0").split(" ")[0])
    dtype = prop.pop("TYPE")

    if dtype == "unsigned fixed":
        if pix_size == 0:
            ntype = np.dtype(np.uint8)
        else:
            try:
                ntype = eval(f"np.dtype(np.uint({pix_size}))")
            except AttributeError:
                raise UserWarning(f"undefined pix size: {pix_size}")
    elif dtype == "float":
        if pix_size == 0:
            ntype = np.dtype(np.float)
        else:
            try:
                ntype = eval(f"np.dtype(np.float{pix_size})")
            except AttributeError:
                raise UserWarning(f"undefined pix size: {pix_size}")
    else:
        raise UserWarning(f"unable to read that type of datas : {dtype}")

    size = 8 * xdim * ydim * zdim * vdim
    mat = np.frombuffer(f.read(size), np.uint8)
    return infos,mat

def nii2inr(nii_path,target_path):
    itk_image = sitk.ReadImage(nii_path)
    # convert to int8
    itk_image = sitk.Cast(itk_image, sitk.sitkUInt8)
    x,y,z = itk_image.GetSize()
    x_spacing,y_spacing,z_spacing = itk_image.GetSpacing()
    itk_array = sitk.GetArrayFromImage(itk_image)
    type = itk_image.GetPixelIDTypeAsString()
    assert type== "8-bit unsigned integer"

    """
    YOU SHALL NOT CHANGE THE HEADER FORMAT!
    """
    info_prefix = f"#INRIMAGE-4#{{\nXDIM={x}\nYDIM={y}\nZDIM={z}\nVDIM=1\nTYPE=unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0\nCPU=decm\nVX={x_spacing}\nVY={y_spacing}\nVZ={z_spacing}"
    header_suffix = "##}}\n"
    num_of_EOL = 257 - len(info_prefix) - len(header_suffix)
    header = f"#INRIMAGE-4#{{\nXDIM={x}\nYDIM={y}\nZDIM={z}\nVDIM=1\nTYPE=unsigned fixed\nPIXSIZE=8 bits\nSCALE=2**0\nCPU=decm\nVX={x_spacing}\nVY={y_spacing}\nVZ={z_spacing}"+ \
             "\n"*num_of_EOL+"##}\n"

    with open(target_path,"wb") as f:
        f.write(header.encode("utf-8"))
        f.write(itk_array.flatten())

    return 1

if __name__ == '__main__':
    target_inria_filename = "/home/SENSETIME/xulixin2/RJ_demo/mesh/all.inr"
    itk_path = "/home/SENSETIME/xulixin2/RJ_demo/volume/no_gall.nii.gz"
    nii2inr(itk_path,target_inria_filename)
    get_data_from_inria_image(target_inria_filename)
