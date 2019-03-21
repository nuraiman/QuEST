#!/bin/bash -l

# this part will be ignored if:
# - nodes are preallocated with salloc
# - and the file is executed as just run.sh (not sbatch run.sh)
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=2
#SBATCH --constraint=mc
#SBATCH --partition=normal
#SBATCH --time=30

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

echo "============================="
echo "  RUNNING BENCHMARKS"
echo "============================="

nodes=1
ranks_per_node=1

N=(100 191 253)
threads_per_rank=(1 2 4 8 16 32)

path_to_executable="./examples_giulia/shor"

# iterate over the indices of list N
for arg in "${N[@]}"
do
    for threads in "${threads_per_rank[@]}"
    do
        export OMP_NUM_THREADS=${threads}

        echo "CONFIGURATION: N = ${arg}, threads = ${threads}"

        output=$(srun -u -N $nodes --ntasks-per-node=$ranks_per_node ${path_to_executable} -N ${arg})
        echo "$output"
        echo "============================================="
        echo ""
    done
done
