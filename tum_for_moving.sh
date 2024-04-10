#!/bin/bash
pathDatasetTUM_VI='/media/extra/slam_net/datasets/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"

folder=$HOME/Datasets/TUM/
folder="$HOME/dynamic_SLAM/CombinedOutputScript/"

folders=("rgbd_dataset_freiburg3_walking_halfsphere" "rgbd_dataset_freiburg3_sitting_xyz" "rgbd_dataset_freiburg3_walking_rpy" "rgbd_dataset_freiburg3_walking_xyz")

number=3
numRuns=10
for dataset in "${folders[@]}"; do
  for i in $(seq 1 $numRuns); do
    if [ -d $folder/$dataset ]; then
      file_name="f_${dataset}"
      ./cmake-build-release/bin/mono_tum_vi ./Vocabulary/ORBvoc.txt ./Examples/Monocular/TUM$number.yaml "$folder/${dataset}/gray" "/media/scratch/TUM/$dataset/timestamp.txt" $file_name

      pushd ./evaluation
        python3 evaluate_ate_scale.py /media/scratch/TUM/$dataset/groundtruth.txt ../f_$file_name.txt --plot $file_name.pdf --verbose
      popd
    fi
  done
done

