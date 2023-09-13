#!/bin/bash
#

set -e # Exit immediately if a command exits with a non-zero status.

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 --input-dir <input-dir> [--verbose] [--output-dir <output-dir>]"
    exit 1
fi

# make a list of desk_with_person, sitting_xyz and walking_xyz
patternList=("desk_with_person" "sitting_xyz" "walking_xyz")

input_dir=""
verbose=false
output_dir="./rundatacollection"
# parse arguments
while [[ $# -gt 1 ]]; do
    key="$1"
    case $key in
        --input-dir)
            input_dir="$2"
            shift # past argument
            ;;
        -v|--verbose)
            verbose=true
            shift # past argument
            ;;
        -o|--output-dir)
            output_dir="$2"
            shift # past argument
            ;;
        *)
            # unknown option
            ;;
    esac
    shift # past argument or value
done

# make sure input_dir is not exists
if [ ! -d "$input_dir" ]; then
    echo "Input directory does not exist"
    exit 1
fi

# make output_dir if it does not exist
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

# make a function with name launch_mono_tum
launch_mono_tum() {
    num="$1"
    dir="$2"
    yamlFile=./Examples/Monocular/TUM${num}.yaml
    
    command="./Examples/Monocular/mono_tum ./Vocabulary/ORBvoc.txt $yamlFile $dir"
    if [ "$verbose" = true ]; then
        echo -e "\e[K running cmd: $command"
    fi

    # run the command 10 times and create output file for each run, outputfile will be named as basename of dir with {2d}.txt extension
    for i in {1..10}; do
        num2d=$(printf "%02d" "$i")
        outputfile=$(basename "$dir")_${num2d}
        # override the saving to echo command each time
        echo -e "\e[K $command $outputfile"
        $command $outputfile

        plotpath="$output_dir"/plot_${outputfile}.png
        resultpath="$output_dir"/path_${outputfile}

        evo_cmd="./Examples/ROS/evaluate_ape.py $dir/groundtruth.txt f_$outputfile.txt -as -r full -va -p --plot_mode xy --save_plot ${plotpath} --save_results ${resultpath}"
        echo -e "\e[K $evo_cmd"
        $evo_cmd
    done
}

echo "Launching Monocular"
# iterate through all folders in input_dir, and see which is a superstring of the pattern
for pattern in "${patternList[@]}"; do
    for dir in "$input_dir"/*; do
        if [[ "$dir" == *"$pattern"* ]]; then
            # dir will have rgbd_dataset_freiberg{1d}_pattern_{2d}, extract 1d number
            num=$(echo "$dir" | grep -oP '(?<=rgbd_dataset_freiburg)\d(?=_'"$pattern"')')
            if [ "$verbose" = true ]; then
                echo -e "\e[K Launching $dir with Monocular, num: $num"
            fi
            launch_mono_tum "$num" "$dir"
        fi
    done
done
