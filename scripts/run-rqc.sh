#!/bin/bash -l

# this part will be ignored if:
# - nodes are preallocated with salloc
# - and the file is executed as just run.sh (not sbatch run.sh)
#SBATCH --nodes=16
#SBATCH --ntasks-per-node=2
#SBATCH --constraint=mc
#SBATCH --partition=normal
#SBATCH --time=500

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

number_of_nodes=(16 8 4 2 1)
ranks_per_node=2

n_repetitions=4
N=(4 4 5 5 6)
depth=(15 20 25 35)

export OMP_NUM_THREADS=18

path_to_executable="./randomQC/rqc"

# iterate over the values of argument N
for arg in "${N[@]}"
do

    # iterate over the values of depth
    for dep in "${depth[@]}"
    do

        # iterate over the values of nodes (with m = n)
        for nodes in "${number_of_nodes[@]}"
        do

           echo "CONFIGURATION: n x m = ${arg}x${arg}, depth = ${dep}, nodes = ${nodes}"
           echo "CONFIGURATION: n x m = ${arg}x${arg}, depth = ${dep}, nodes = ${nodes}" >> ../benchmarks/rqc-benchmark.txt

            output=$(srun -u -N $nodes --ntasks-per-node=$ranks_per_node ${path_to_executable} -n ${arg} -m ${arg} -d ${dep} -r ${n_repetitions})
            echo "$output" >> ../benchmarks/rqc-benchmark.txt
            echo "$output"
            echo "--------------------------------" >> ../benchmarks/rqc-benchmark.txt
        done
        echo ""
        echo "================================"
        echo ""

        # iterate over the values of nodes (with m = n+1)
        for nodes in "${number_of_nodes[@]}"
        do

            echo "CONFIGURATION: n x m = ${arg}x$((${arg}+1)), depth = ${dep}, nodes = ${nodes}"
            echo "CONFIGURATION: n x m = ${arg}x$((${arg}+1)), depth = ${dep}, nodes = ${nodes}" >> ../benchmarks/rqc-benchmark.txt

            output=$(srun -u -N $nodes --ntasks-per-node=$ranks_per_node ${path_to_executable} -n ${arg} -m $((${arg}+1)) -d ${dep} -r ${n_repetitions})
            echo "$output" >> ../benchmarks/rqc-benchmark.txt
            echo "$output"
            echo "--------------------------------" >> ../benchmarks/rqc-benchmark.txt
        done
        echo ""
        echo "================================"
        echo ""

    done
done