#!/bin/bash
pathDatasetTUM_VI='/media/extra/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"
#./Monocular/mono_tum_vi ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_halfsphere/gray "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_halfsphere/timestamp.txt  dataset-freiburg3-walking-halfsphere
./Monocular/mono_tum_vi ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$HOME/Desktop/DynamicSLAM/gray" "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_halfsphere/timestamp.txt  dataset-freiburg3-walking-halfsphere_ours
