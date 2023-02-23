import SimpleITK as sitk
import numpy as np
import os

if __name__ == '__main__':
    root_dir = "/home/SENSETIME/xulixin2/RJ_demo/volume"
    img_path = root_dir + '/pv.nii.gz'
    organ_path = root_dir + '/organ.nii.gz'
    vessel_path = root_dir + '/vessel.nii.gz'
    seg_path = root_dir + '/debug_fla.nii.gz'

    pv_img = sitk.ReadImage(img_path)
    pv_array = sitk.GetArrayFromImage(pv_img)

    organ_img = sitk.ReadImage(organ_path)
    organ_array = sitk.GetArrayFromImage(organ_img)

    vessel_img = sitk.ReadImage(vessel_path)
    vessel_array = sitk.GetArrayFromImage(vessel_img)

    seg_img = sitk.ReadImage(seg_path)
    seg_arr = sitk.GetArrayFromImage(seg_img)

    organ_array[organ_array != 4] = 0
    vessel_array[vessel_array>=3] = 0

    # 10 for gallbladder, 11 for vessel
    organ_array[organ_array!=0] = 10
    vessel_array[vessel_array!=0] = 11

    all_arr = np.zeros_like(pv_array)
    all_arr = (seg_arr | organ_array)
    all_arr[vessel_array!=0] = 11

    all_arr[all_arr == 10] = 0
    all_img = sitk.GetImageFromArray(all_arr)
    all_img.CopyInformation(pv_img)
    sitk.WriteImage(all_img, os.path.join(root_dir, 'no_gall.nii.gz'))
