import csv
from os import getcwd, path, system
from sys import exit
from subprocess import Popen, PIPE, check_output, CalledProcessError, call
from time import sleep,gmtime, strftime
from os.path import *
import glob
from datetime import datetime
import fnmatch
import os

##################################
# hardware specification
##################################
# this part should not be changed
cores_per_socket = 18
sockets_per_node = 2

##################################
# run configuration
##################################
# this part can be changed
nodes = 4
ranks_per_node = 2
threads_per_rank = 18

if ranks_per_node * threads_per_rank != ranks_per_node * threads_per_rank:
    print("Chosen configuration potentially not optimal!")
    print("For optimal performance, it is advised to have ranks_per_node * threads_per_rank = ",
            cores_per_socket * sockets_per_node)

# estimated maximum time of the simulation
time = 30 # [minutes]

###################################
# executable to run with arguments
###################################
prefix_to_executable = "./examples_giulia/"
executable = "shor"
args = dict()
# length of different values of arguments
n_configs = 3
args["N"] = [143, 191,253]

repetitions = 10

sbatch_script = "run.sh"

with open(sbatch_script, "w") as file:
    ##################################
    # setting up the configuration
    ##################################
    file.write("#!/bin/bash -l\n")
    file.write("#SBATCH --nodes=" + str(nodes) + "\n")
    file.write("#SBATCH --ntasks-per-node=" + str(ranks_per_node) + "\n")
    file.write("#SBATCH --constraint=mc\n")
    file.write("#SBATCH --partition=normal\n")
    file.write("#SBATCH --time=" + str(time) + "\n")
    file.write("\n")

    ##################################
    # loading modules
    ##################################
    file.write("module load daint-mc\n")
    file.write("module swap PrgEnv-cray PrgEnv-gnu\n")
    file.write("module load CMake\n")
    file.write("\n")
    file.write("export CC=`which cc`\n")
    file.write("export CXX=`which CC`\n")
    file.write("export CRAYPE_LINK_TYPE=dynamic\n")

    file.write("export OMP_NUM_THREADS=" + str(threads_per_rank) + "\n")
    file.write("\n")

    ##################################
    # argument values
    ##################################
    for key in args:
        arr = str(key) + "=("
        for value in args[key]:
            arr += str(value) + " "
        arr += ")\n"
        file.write(arr)
    file.write("\n")

    ##################################
    # running configurations
    ##################################
    file.write("echo \"=============================\"\n")
    file.write("echo \"  RUNNING CONFIGURATIONS\"\n")
    file.write("echo \"=============================\"\n")
    file.write("for i in $(seq 0 1 " + str(n_configs - 1) + ")\ndo\n")
    indent = "    "
    file.write(indent + "echo \"configuration $i:\"\n")
    arr = ""
    for key in args:
        arr += "-" + str(key) + " ${" + str(key) + "[i]}"

    file.write(indent + "output=$(srun -u -N " + str(nodes) + " --ntasks-per-node=" + str(ranks_per_node) +
            " " + prefix_to_executable + executable + " " + arr + ")\n")
    file.write(indent + "echo \"$output\"\n")
    file.write("done\n")

# job = Popen(["sbatch", sbatch_script], stdout=PIPE)
# job.wait()
# identifier = None
# for line in job.stdout:
#     if line.startswith("Submitted batch job "):
#         start = len("Submitted batch job ")
#         identifier = int(line[start:])
# if not identifier:
#     exit("could not start batch job")
# else:
#     print("-> submitting job: " + str(identifier))
# 
# """ wait until job terminates """
# print("-> waiting for job: " + str(identifier))
# statusOld = ''
# while True:
#     try:
#         result = check_output(["squeue -j" + str(identifier)], shell=True)
#         if len(result) < 110:
#             if not 'status' in locals():
#                 status = '-empty-'
#             if not 'f' in locals():
#                 f= open(join(folder,"timelog.txt"),"w+")                    
#                 f.write("Status: " + status + ", python_time: " + strftime("%H:%M:%S +0000", gmtime()) + "\n")
#             else:
#                 f.write("Status: " + status + ", sbatch_time: " + time + ", python_time: " + strftime("%H:%M:%S +0000", gmtime()) + "\n")
#             path = join(folder,"*.out")
#             for filename in glob.glob(path):
#                 with open(filename, 'r') as slurm:
#                     for line in slurm:
#                         if line.startswith('Wallclock'):
#                             f.write(line)
#             f.close()
#             break
#         status = result.split('\n')[1].split()[4]
#         if (status != 'PD'):
#             time = result.split('\n')[1].split()[6]
#             if status != statusOld:
#                 print("Executing! Status = " + status)
#                 statusOld = status
#                 if not 'f' in locals():
#                     f= open(join(folder,"timelog.txt"),"w+")                    
#                 f.write("Status: " + status + ", sbatch_time: " + time + ", python_time: " + strftime("%H:%M:%S +0000", gmtime()) + "\n")
#         sleep(1)
#     except CalledProcessError as error:
#         print(error)
#         break
# 
