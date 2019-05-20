#!/bin/bash -l
#SBATCH --job-name=matmul
#SBATCH --time=GLOBAL_TIME
#SBATCH --nodes=GLOBAL_NODES
#SBATCH --constraint=mc
#SBATCH --output="script_GLOBAL_NODES.out"
#set -x

module load daint-mc
module swap PrgEnv-cray PrgEnv-gnu
module unload cray-libsci
module load intel
module load CMake
module load hwloc

export CC=`which cc`
export CXX=`which CC`
export CRAYPE_LINK_TYPE=dynamic

export prefix="/scratch/snx3000/kabicm"

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/scratch/snx3000/kabicm/ctf/build/lib_shared"

m_range=GLOBAL_M_RANGE
n_range=GLOBAL_N_RANGE
k_range=GLOBAL_K_RANGE

n_tasks_upper=GLOBAL_TASKS

mem_limit=GLOBAL_MEM_LIMIT  #8847360 # in # of doubles and not in bytes

run_scalapack() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    p_rows=$5
    p_cols=$6
    idx=$7
    n_ranks_per_node=36
    n_threads_per_rank=1
    export OMP_NUM_THREADS=$n_threads_per_rank
    export MKL_NUM_THREADS=$n_threads_per_rank
    n_ranks=$((p_rows*p_cols))

    if [ $k -gt $m ]; then
        srun -p -y -N $nodes -n $n_ranks -c $n_threads_per_rank --hint=nomultithread \
             $prefix/DLA-interface/build/miniapp/matrix_multiplication \
             -m $m -n $n -k $k --scalapack \
             -p $p_rows -q $p_cols \
             -r $n_iter --transb
    else
        srun -p -y -N $nodes -n $n_ranks -c $n_threads_per_rank --hint=nomultithread \
             $prefix/DLA-interface/build/miniapp/matrix_multiplication \
             -m $m -n $n -k $k --scalapack \
             -p $p_rows -q $p_cols \
             -r $n_iter
    fi

#    if [ $idx -eq 2 ]; then
#        echo ""
#        echo "============================"
#        echo "      PARTIAL NODE"
#        echo "============================"
#        n_ranks=$((4*(nodes-1)))
#        index=$(find_index $n_ranks)
#        p_rows=${p_weird[$index]}
#        p_cols=${q_weird[$index]}
#
#        if [ $k -gt $m ]; then
#            srun -N $nodes -n $n_ranks -c $n_threads_per_rank --hint=nomultithread \
#                 $prefix/DLA-interface/build/miniapp/matrix_multiplication \
#                 -m $m -n $n -k $k --scalapack \
#                 -p $p_rows -q $p_cols \
#                 -r $n_iter --transb
#        else
#            srun -N $nodes -n $n_ranks -c $n_threads_per_rank --hint=nomultithread \
#                 $prefix/DLA-interface/build/miniapp/matrix_multiplication \
#                 -m $m -n $n -k $k --scalapack \
#                 -p $p_rows -q $p_cols \
#                 -r $n_iter
#        fi
#    fi
}

run_cosma() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    limited=$5
    idx=$6
    n_ranks_per_node=36
    n_threads_per_rank=1
    export OMP_NUM_THREADS=$n_threads_per_rank
    export MKL_NUM_THREADS=$n_threads_per_rank
    #n_ranks=$((nodes*n_ranks_per_node))
    n_ranks=$n_tasks_upper
    mem_limit=$((mem_limit/n_ranks_per_node))
    if [ $nodes -ge 100 ];
    then
        #mem_limit=$(echo "($mem_limit*0.4 + 0.5)/1"|bc)
        mem_limit=45000000
        echo "Decreased memory limit to "$mem_limit
    elif [ $nodes -ge 80 ];
    then
        mem_limit=$(echo "($mem_limit*0.5 + 0.5)/1"|bc)
    fi

    if [ "$limited" = true ]; 
    then
        srun -p -y -N $nodes -n $n_ranks \
             $prefix/cosma/build/miniapp/cosma-temp \
             -m $m -n $n -k $k -P $n_ranks --memory $mem_limit

        #if [ $idx -eq 2 ]; then
        #    echo ""
        #    echo "============================"
        #    echo "      PARTIAL NODE"
        #    echo "============================"
        #    n_ranks=$((36*(nodes-1)+1))
        #    echo "Total number of cores: "$n_ranks

        #    srun -N $nodes -n $n_ranks \
        #         $prefix/COSMA/build/miniapp/temp-miniapp \
        #         -m $m -n $n -k $k -P $n_ranks --memory $mem_limit
        #fi
    else
        srun -p -y -N $nodes -n $n_ranks \
             $prefix/cosma/build/miniapp/cosma-temp \
             -m $m -n $n -k $k -P $n_ranks

        #if [ $idx -eq 2 ]; then
        #    echo ""
        #    echo "============================"
        #    echo "      PARTIAL NODE"
        #    echo "============================"
        #    n_ranks=$((36*(nodes-1)+1))
        #    echo "Total number of cores: "$n_ranks

        #    srun -N $nodes -n $n_ranks \
        #         $prefix/COSMA/build/miniapp/temp-miniapp \
        #         -m $m -n $n -k $k -P $n_ranks
        #fi
    fi

    if [ $? -ne 0 ];
    then
        echo "error"
    fi
}

run_old_cosma() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    limited=$5
    idx=$6
    n_ranks_per_node=36
    n_threads_per_rank=1
    export OMP_NUM_THREADS=$n_threads_per_rank
    export MKL_NUM_THREADS=$n_threads_per_rank
    #n_ranks=$((nodes*n_ranks_per_node))
    n_ranks=$n_tasks_upper
    mem_limit=$((mem_limit/n_ranks_per_node))

    if [ $nodes -ge 100 ];
    then
        #mem_limit=$(echo "($mem_limit*0.4 + 0.5)/1"|bc)
        mem_limit=45000000
        echo "Decreased memory limit to "$mem_limit
    elif [ $nodes -ge 80 ];
    then
        mem_limit=$(echo "($mem_limit*0.5 + 0.5)/1"|bc)
    fi

    if [ "$limited" = true ]; 
    then
        srun -p -y -N $nodes -n $n_ranks \
             $prefix/carma/rect-class/bench-rect-nc \
             -m $m -n $n -k $k -L $mem_limit
    else
        srun -p -y -N $nodes -n $n_ranks \
             $prefix/carma/rect-class/bench-rect-nc \
             -m $m -n $n -k $k 
    fi

    if [ $? -ne 0 ]; 
    then
        echo "error"
    fi
}

run_cyclops() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    limited=$5
    idx=$6
    #n_ranks_per_node=36
    n_ranks_per_node=36
    n_threads_per_rank=1
    export OMP_NUM_THREADS=$n_threads_per_rank
    export MKL_NUM_THREADS=$n_threads_per_rank
    #n_ranks=$((nodes*n_ranks_per_node))
    n_ranks=$n_tasks_upper

    memory_in_bytes=$((8*mem_limit))
    memory_in_bytes=$(echo "($memory_in_bytes+0.5)/1"|bc)
    echo "Memory limit = "$memory_in_bytes

    if [ "$limited" = true ]; 
    then
        CTF_MEMORY_SIZE=$memory_in_bytes srun -p -y -N $nodes -n $n_ranks \
           $prefix/ctf/build/bin/matmul -m $m -n $n -k $k \
           -sym_A NS -sym_B NS -sym_C NS -sp_A 1.0 -sp_B 1.0 -sp_C 1.0 -niter $n_iter \
           | grep -v ERROR && echo "error"

        #if [ $idx -eq 2 ]; then
        #    echo ""
        #    echo "============================"
        #    echo "      PARTIAL NODE"
        #    echo "============================"
        #    n_ranks=$((36*(nodes-1)+1))
        #    echo "Total number of cores: "$n_ranks

        #    CTF_MEMORY_SIZE=$memory_in_bytes srun -N $nodes -n $n_ranks \
        #       $prefix/ctf/build/bin/matmul -m $m -n $n -k $k \
        #       -sym_A NS -sym_B NS -sym_C NS -sp_A 1.0 -sp_B 1.0 -sp_C 1.0 -niter $n_iter \
        #       -bench 1 -test 0 | grep -v -i ERROR && echo "error"
        #fi
    else
        srun -p -y -N $nodes -n $n_ranks \
           $prefix/ctf/build/bin/matmul -m $m -n $n -k $k \
           -sym_A NS -sym_B NS -sym_C NS -sp_A 1.0 -sp_B 1.0 -sp_C 1.0 -niter $n_iter \
           | grep -v ERROR && echo "error"

        #if [ $idx -eq 2 ]; then
        #    echo ""
        #    echo "============================"
        #    echo "      PARTIAL NODE"
        #    echo "============================"
        #    n_ranks=$((36*(nodes-1)+1))
        #    echo "Total number of cores: "$n_ranks

        #    srun -N $nodes -n $n_ranks \
        #       $prefix/ctf/build/bin/matmul -m $m -n $n -k $k \
        #       -sym_A NS -sym_B NS -sym_C NS -sp_A 1.0 -sp_B 1.0 -sp_C 1.0 -niter $n_iter \
        #       -bench 1 -test 0 | grep -v -i ERROR && echo "error"
        #fi
    fi

    if [ $? -ne 0 ] 
    then
        echo "error"
    fi
}

function contains() {
    local n=$#
    local value=${!n}
    for ((i=1;i < $#;i++)) {
        if [ "${!i}" == "${value}" ]; then
            echo "y"
            return 0
        fi
    }
    echo "n"
    return 1
}

function find_index() {
    local value=$1
    for i in "${!n_tasks_weird[@]}";
    do
        if [ "${n_tasks_weird[$i]}" -eq "$value" ]; then
            echo $i
            return 0
        fi
    done
    echo "-1"
    return 1
}

substring() {
    string="$1"
    substring="$2"
    if [ "${string/$substring}" = "$string" ];
    then
        echo "n"
    else
        echo "y"
    fi
}

run_one() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    p_rows=$5
    p_cols=$6
    algorithm=$7
    idx=$8

    echo ""
    echo ""
    echo "======================================================================================"
    echo "           EXPERIMENT: nodes = $nodes, (m, n, k) = ($m, $n, $k)"
    echo "======================================================================================"
    echo "memory limit = $mem_limit"
    echo $nodes" "$m" "$n" "$k" "$mem_limit >> "config.txt"

    if [ "$algorithm" = "cosma" ];
    then
        echo ""
        echo "================================="
        echo "           COSMA"
        echo "================================="
        # OUR ALGORITHM
        output=$(run_cosma $m $n $k $nodes true $idx)
        echo $output

        error=$(substring $output "error")
        errors=$(substring $output "errors")

        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then 
            echo "Failed with limited memory, retrying with infinite memory..."
            output=$(run_cosma $m $n $k $nodes false $idx)
            echo $output
        fi
        cosma_time=$(echo $output | awk -v n_iters="$n_iter" '/COSMA TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
        echo "COSMA TIMES = "$cosma_time
        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then
            echo $nodes" "$m" "$n" "$k" inf "$cosma_time >> "cosma_"$nodes".txt"
        else
            echo $nodes" "$m" "$n" "$k" "$mem_limit" "$cosma_time >> "cosma_"$nodes".txt"
        fi
        echo $nodes" "$m" "$n" "$k" inf "$cosma_time >> "cosma_"$nodes".txt"
        time=`date '+[%H:%M:%S]'`

        mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
        echo "communication file = "$mpi_file
        echo $algorithm" "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

        echo "Finished our algorithm at "$time

    elif [ "$algorithm" = "scalapack" ];
    then
        echo ""
        echo "================================="
        echo "           SCALAPACK"
        echo "================================="

        # SCALAPACK
        output=$(run_scalapack $m $n $k $nodes $p_rows $p_cols $idx)
        scalapack_time=$(echo $output | awk -v n_iters="$n_iter" '/ScaLAPACK TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
        echo $output
        echo "SCALAPACK TIME = "$scalapack_time
        time=`date '+[%H:%M:%S]'`
        echo $nodes" "$m" "$n" "$k" inf "$scalapack_time >> "scalapack_"$nodes".txt"

        mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
        echo "communication file = "$mpi_file
        echo $algorithm" "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

        echo "Finished ScaLAPACK algorithm at "$time

    elif [ "$algorithm" = "cyclops" ];
    then
        echo ""
        echo "================================="
        echo "           CYCLOPS"
        echo "================================="

        # CYCLOPS
        output=$(run_cyclops $m $n $k $nodes true $idx)
        echo $output
        error=$(substring $output "error")
        errors=$(substring $output "errors")
        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then 
            echo "Failed with limited memory, retrying with infinite memory..."
            output=$(run_cyclops $m $n $k $nodes false $idx)
            echo $output
        fi
        cyclops_time=$(echo $output | awk -v n_iters="$n_iter" '/CYCLOPS TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
        time=`date '+[%H:%M:%S]'`
        echo "CYCLOPS TIME = "$cyclops_time
        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then
            echo $nodes" "$m" "$n" "$k" inf "$cyclops_time >> "cyclops_"$nodes".txt"
        else
            echo $nodes" "$m" "$n" "$k" "$mem_limit" "$cyclops_time >> "cyclops_"$nodes".txt"
        fi

        mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
        echo "communication file = "$mpi_file
        echo $algorithm" "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

        echo "Finished CYCLOPS algorithm at "$time

    elif [ "$algorithm" = "carma" ];
    then
        echo ""
        echo "================================="
        echo "           OLD COSMA"
        echo "================================="
        # OLD COSMA
        #if [ $(contains "${n_nodes_powers[@]}" $nodes) == "y" ]; then
        output=$(run_old_cosma $m $n $k $nodes true $idx)
        echo $output

        error=$(substring $output "error")
        errors=$(substring $output "errors")

        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then 
            echo "Failed with limited memory, retrying with infinite memory..."
            output=$(run_old_cosma $m $n $k $nodes false $idx)
            echo $output
        fi
        old_cosma_time=$(echo $output | awk -v n_iters="$n_iter" '/OLD_CARMA TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
        echo "OLD CARMA TIME = "$old_cosma_time
        if [ "$error" = "y" ] && [ "$errors" = "n" ];
        then
            echo $nodes" "$m" "$n" "$k" inf "$old_cosma_time >> "old_carma_"$nodes".txt"
        else
            echo $nodes" "$m" "$n" "$k" "$mem_limit" "$old_cosma_time >> "old_carma_"$nodes".txt"
        fi
        #else
        #    echo "OLD COSMA TIME = not a power of 2"
        #    echo "not a power of 2" >> "old_cosma_"$nodes".txt"
        #fi
        time=`date '+[%H:%M:%S]'`

        mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
        echo "communication file = "$mpi_file
        echo $algorithm" "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

        echo "Finished OLD COSMA algorithm at "$time
    fi
}

run_all() {
    export n_iter=N_REP
    m=$1
    n=$2
    k=$3
    nodes=$4
    p_rows=$5
    p_cols=$6
    idx=$7

    echo ""
    echo ""
    echo "======================================================================================"
    echo "           EXPERIMENT: nodes = $nodes, (m, n, k) = ($m, $n, $k)"
    echo "======================================================================================"
    echo "memory limit = $mem_limit"
    echo $nodes" "$m" "$n" "$k" "$mem_limit >> "config.txt"

    echo ""
    echo "================================="
    echo "           COSMA"
    echo "================================="
    # OUR ALGORITHM
    output=$(run_cosma $m $n $k $nodes true $idx)
    echo $output

    error=$(substring $output "error")
    errors=$(substring $output "errors")

    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then 
        echo "Failed with limited memory, retrying with infinite memory..."
        output=$(run_cosma $m $n $k $nodes false $idx)
        echo $output
    fi
    cosma_time=$(echo $output | awk -v n_iters="$n_iter" '/COSMA TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
    echo "COSMA TIMES = "$cosma_time
    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then
        echo $nodes" "$m" "$n" "$k" inf "$cosma_time >> "cosma_"$nodes".txt"
    else
        echo $nodes" "$m" "$n" "$k" "$mem_limit" "$cosma_time >> "cosma_"$nodes".txt"
    fi
    time=`date '+[%H:%M:%S]'`
    echo "Finished our algorithm at "$time

    mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
    echo "communication file = "$mpi_file
    echo "cosma "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

    echo ""
    echo "================================="
    echo "           SCALAPACK"
    echo "================================="

    # SCALAPACK
    output=$(run_scalapack $m $n $k $nodes $p_rows $p_cols $idx)
    echo $output
    scalapack_time=$(echo $output | awk -v n_iters="$n_iter" '/ScaLAPACK TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
    echo "SCALAPACK TIMES = "$scalapack_time
    time=`date '+[%H:%M:%S]'`
    echo $nodes" "$m" "$n" "$k" inf "$scalapack_time >> "scalapack_"$nodes".txt"
    echo "Finished ScaLAPACK algorithm at "$time

    mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
    echo "communication file = "$mpi_file
    echo "scalapack "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

    echo ""
    echo "================================="
    echo "           CYCLOPS"
    echo "================================="

    # CYCLOPS
    output=$(run_cyclops $m $n $k $nodes true $idx)
    echo $output
    error=$(substring $output "error")
    errors=$(substring $output "errors")

    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then 
        echo "Failed with limited memory, retrying with infinite memory..."
        output=$(run_cyclops $m $n $k $nodes false $idx)
        echo $output
    fi
    cyclops_time=$(echo $output | awk -v n_iters="$n_iter" '/CYCLOPS TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
    time=`date '+[%H:%M:%S]'`
    echo "CYCLOPS TIME = "$cyclops_time
    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then
        echo $nodes" "$m" "$n" "$k" inf "$cyclops_time >> "cyclops_"$nodes".txt"
    else
        echo $nodes" "$m" "$n" "$k" "$mem_limit" "$cyclops_time >> "cyclops_"$nodes".txt"
    fi
    echo "Finished CYCLOPS algorithm at "$time

    mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
    echo "communication file = "$mpi_file
    echo "cyclops "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

    echo ""
    echo "================================="
    echo "           OLD COSMA"
    echo "================================="
    # OLD COSMA
    #if [ $(contains "${n_nodes_powers[@]}" $nodes) == "y" ]; then
    output=$(run_old_cosma $m $n $k $nodes true $idx)
    echo $output
    error=$(substring $output "error")
    errors=$(substring $output "errors")

    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then 
        echo "Failed with limited memory, retrying with infinite memory..."
        output=$(run_old_cosma $m $n $k $nodes false $idx)
        echo $output
    fi
    old_cosma_time=$(echo $output | awk -v n_iters="$n_iter" '/OLD_CARMA TIMES/ {for (i = 0; i < n_iters; i++) {printf "%d ", $(5+i)}}')
    echo "OLD CARMA TIME = "$old_cosma_time
    if [ "$error" = "y" ] && [ "$errors" = "n" ];
    then
        echo $nodes" "$m" "$n" "$k" inf "$old_cosma_time >> "old_carma_"$nodes".txt"
    else
        echo $nodes" "$m" "$n" "$k" "$mem_limit" "$old_cosma_time >> "old_carma_"$nodes".txt"
    fi
    #else
    #    echo "OLD COSMA TIME = not a power of 2"
    #    echo "not a power of 2" >> "old_cosma_"$nodes".txt"
    #fi
    time=`date '+[%H:%M:%S]'`
    echo "Finished OLD CARMA algorithm at "$time

    mpi_file=$(echo $output | awk -v n_iters="$n_iter" '/mpiP: Storing mpiP output in/ {print substr($6, 2, length($6) - 3)}')
    echo "communication file = "$mpi_file
    echo "carma "$n_tasks_upper" "$m" "$n" "$k" "$mem_limit" "$mpi_file >> "comm_files.txt"

}

compile() {
    # compile all libraries
    echo "======================================================================================"
    echo "           COMPILATION"
    echo "======================================================================================"
    echo ""
    (
        echo "Compiling Carma (OLD_CARMA) library..."
        cd $prefix/carma/rect-class/
        make -j
    )
    (
        echo "Compiling our library..."
        cd $prefix/cosma/build/
        make -j
    )
    (
        echo "Compiling DLA-interface (ScaLAPACK)"
        cd $prefix/DLA-interface/build/
        make -j
    )
    (
        echo "Compiling Cyclops library"
        cd $prefix/ctf/build/
        make -j
        make matmul
    )
}

IFS=

compile

export n_iter=$n_rep

for idx in ${!m_range[@]}
do
    m=${m_range[idx]}
    n=${n_range[idx]}
    k=${k_range[idx]}
    run_all $m $n $k GLOBAL_NODES GLOBAL_P GLOBAL_Q $idx
    # run_one $m $n $k GLOBAL_NODES GLOBAL_P GLOBAL_Q carma $idx
done
