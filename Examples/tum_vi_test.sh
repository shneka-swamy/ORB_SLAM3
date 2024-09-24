#!/bin/bash
pathDatasetTUM_VI='/media/scratch/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"
./Monocular/mono_tum ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_halfsphere
#./Monocular/mono_tum ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$HOME"/Desktop/rgbd_dataset_freiburg3_walking_xyz_30/gray "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_xyz/timestamp.txt  dataset-freiburg3-walking-xyz_ours

#./Monocular/mono_tum ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$HOME"/Desktop/rgbd_dataset_freiburg3_walking_xyz_30/ 