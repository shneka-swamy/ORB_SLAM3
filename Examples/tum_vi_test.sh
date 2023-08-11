#!/bin/bash
pathDatasetTUM_VI='/media/extra/slam_net/datasets/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"
./Monocular/mono_tum_vi ../Vocabulary/ORBvoc.txt Monocular/TUM3.yaml "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_xyz/gray "$pathDatasetTUM_VI"/rgbd_dataset_freiburg3_walking_xyz/timestamp.txt  dataset-freiburg3-walking-xyz

