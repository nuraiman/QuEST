experiment_time=()

n_nodes=()
n_tasks=()
p_range=()
p_rows=()
p_cols=()

strong_scaling_mn=()
strong_scaling_k=()

weak_scaling_p0_mn=()
weak_scaling_p0_k=()

weak_scaling_p1_mn=()
weak_scaling_p1_k=()

mem_limit=()
n_rep=()

DATE=`date '+%d-%m-%Y[%H:%M:%S]'`
mkdir $DATE
cd ./$DATE


files=()

for node_idx in ${!n_nodes[@]}
do
    if [ $node_idx -le 4 ]
    then
        m_values=(${weak_scaling_p0_mn[node_idx]} ${weak_scaling_p1_mn[node_idx]})
        n_values=(${weak_scaling_p0_mn[node_idx]} ${weak_scaling_p1_mn[node_idx]})
        k_values=(${weak_scaling_p0_k[node_idx]} ${weak_scaling_p1_k[node_idx]})
    else
        m_values=($strong_scaling_mn ${weak_scaling_p0_mn[node_idx]} ${weak_scaling_p1_mn[node_idx]})
        n_values=($strong_scaling_mn ${weak_scaling_p0_mn[node_idx]} ${weak_scaling_p1_mn[node_idx]})
        k_values=($strong_scaling_k ${weak_scaling_p0_k[node_idx]} ${weak_scaling_p1_k[node_idx]})
    fi

    nodes=${n_nodes[node_idx]}
    tasks=${n_tasks[node_idx]}
    sname=script_$nodes.sh
    sed "s|GLOBAL_NODES|$nodes|g; s|GLOBAL_P|${p_rows[node_idx]}|g; s|GLOBAL_Q|${p_cols[node_idx]}|g; s|GLOBAL_TIME|$experiment_time|g; s|GLOBAL_MEM_LIMIT|$mem_limit|g; s|GLOBAL_M_RANGE|(${m_values[*]})|g; s|GLOBAL_N_RANGE|(${n_values[*]})|g; s|GLOBAL_K_RANGE|(${k_values[*]})|g; s|GLOBAL_TASKS|${tasks}|g; s|N_REP|$n_rep|g; " ./mm_comparison.sh > $sname

    chmod a+x $sname
    files+=($sname)
done

echo "Generated the following files: "
echo ${files[*]}

for file in ${files[@]}
do
    sbatch ./$file
    echo "Executing the script "$file
done


