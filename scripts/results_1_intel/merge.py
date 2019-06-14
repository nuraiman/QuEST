import math
import os
import datetime
from subprocess import call
import numpy as np

n_nodes = [1,2,4,8,16,32,64,128]
N_dj = [16,19,22,24,26,27,28,29,30,31,33]
N_rqc = [4,5]
depth_rqc = [20,40,60]


exec_names = ["dj", "rqc"]

for exe in exec_names:
    N_size_name = "N_" + exe
    N_size = eval(N_size_name)
    output_name = exe + "_full.txt"
    exe_output = open(output_name, "w")
    cnt = np.full(len(n_nodes),-1)
    temp_cnt = 0
    for N in N_size:

        for j, nodes in enumerate(n_nodes):
            file_name = exe + "_" + str(nodes) + ".txt"
            file = open(file_name, "r")

            for i, line in enumerate(file):
                temp_cnt = i
                if i <= cnt[j]:
                    continue
                exe_output.write(line)
                if line.startswith("--"):
                    break

            cnt[j] = temp_cnt

            file.close()

    exe_output.close()

