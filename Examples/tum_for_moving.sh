#!/bin/bash
pathDatasetTUM_VI='/media/extra/slam_net/datasets/TUM' #Example, it is necesary to change it by the dataset path

#------------------------------------
# Monocular Examples
echo "Launching Room 1 with Monocular sensor"

folder=$HOME/Datasets/TUM/
folder="$HOME/dynamic_SLAM/CombinedOutputScript/"

number=3
for dataset in $(ls $folder); do
  if [ -d $folder/$dataset ]; then
    file_name="f_${dataset}"
    ../cmake-build-release/bin/mono_tum_vi ../Vocabulary/ORBvoc.txt Monocular/TUM$number.yaml "$folder/${dataset}/gray" "/media/scratch/TUM/$dataset/timestamp.txt" $file_name

    pushd ../evaluation
      python3 evaluate_ate_scale_dummy.py $folder/groundtruth.txt ../Examples/f_$file_name.txt --plot $file_name.pdf --verbose
    popd
    break
  fi
done

