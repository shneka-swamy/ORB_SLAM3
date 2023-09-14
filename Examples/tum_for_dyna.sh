#!/bin/bash
pathDatasetTUM_VI='/media/extra/slam_net/datasets/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"

folder='/media/scratch/TUM/rgbd_dataset_freiburg3_walking_halfsphere/'
pathdataset='/media/scratch/dynamicSlamOutput/'

number=3
file_name='f_freiburg3_walking_halfsphere'
./Monocular/mono_tum_vi ../Vocabulary/ORBvoc.txt Monocular/TUM$number.yaml "$pathdataset/gray" "$folder/timestamp.txt"  $file_name

cd ../evaluation
python3 evaluate_ate_scale_dummy.py $folder/groundtruth.txt ../Examples/f_$file_name.txt --plot $file_name.pdf --verbose
cd ../Examples

