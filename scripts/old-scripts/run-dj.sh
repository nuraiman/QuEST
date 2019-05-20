#!/bin/bash -l

# this part will be ignored if:
# - nodes are preallocated with salloc
# - and the file is executed as just run.sh (not sbatch run.sh)
#SBATCH --nodes=16
#SBATCH --ntasks-per-node=2
#SBATCH --constraint=mc
#SBATCH --partiition=normal
#SBATCH --time=300

# loading necessary modules
echo "============================="
echo "  LOADING MODULES"
echo "============================="
module load daint-mc
module swap PrgEnv-cray PrgEnv-gnu
module load CMake

export CC=`which cc`
export CXX=`which CC`
export CRAYPE_LINK_TYPE=dynamic

echo "============================="
echo "  BUILDING AND COMPILATION"
echo "============================="

cd ../build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=~ -DDISTRIBUTED:BOOL=TRUE -DMULTITHREADED:BOOL=TRUE -DGPUACCELERATED:BOOL=FALSE -DCMAKE_CXX_FLAGS='-fPIC' -DGPU_COMPUTE_CAPABILITY=60 -DPRECISION=2 -DQuEST_DIR='QuEST' -DQuEST_TEST_DIR='tests'
make -j

echo "================================"
echo "       RUNNING BENCHMARKS"
echo "================================"

number_of_nodes=(1 2 4 8 16)
ranks_per_node=2

n_repetitions=10
N=(16 19 22 24 26 27 28 29 30 31 33)

export OMP_NUM_THREADS=18

path_to_executable="./examples_giulia/dj"

# iterate over the values of argument N
for arg in "${N[@]}"
do
    # iterate over the values of threads
    for nodes in "${number_of_nodes[@]}"
    do

        echo "CONFIGURATION: N = ${arg}, nodes = ${nodes}"
        echo "CONFIGURATION: N = ${arg}, nodes = ${nodes}" >> ../benchmarks/dj-benchmark.txt

        output=$(srun -u -N $nodes --ntasks-per-node=$ranks_per_node ${path_to_executable} -N ${arg} -r ${n_repetitions})
        echo "$output" >> ../benchmarks/dj-benchmark.txt
        echo "$output"
        echo "--------------------------------" >> ../benchmarks/dj-benchmark.txt
    done
    echo ""
    echo "================================"
    echo ""
done
