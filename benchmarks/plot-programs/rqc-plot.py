import matplotlib.pyplot as plt
import numpy as np


nodes = [1, 2, 4, 8, 16]  # number of threads
row = [4, 5]
depth = [20, 40, 60]
a = [1, 2, 3, 4, 5]

numQubits = []
deptharr = []
for num in row:
    for dep in depth:
        numQubits.append(num**2)
        numQubits.append(num*(num+1))
        deptharr.append(dep)
        deptharr.append(dep)


# readout data from textfile
filenames = ["../output-files/rqc-benchmark.txt"]
master_data = []
data = [master_data]

for i, filename in enumerate(filenames):
    f = open(filename, 'r')
    data[i] = f.read()
    f.close()

    temp = data[i].split('\n')

    avg_times = [row.split(' ')[4] for row in temp if row.startswith("Avg")]
    avg_times = [k.split(',')[0] for k in avg_times]  # get rid of the commas in the list of avg_times

    avg_times = [float(num) for num in avg_times]

    lenThr = len(nodes)
    y = []
    for j in range(len(numQubits)):
        temp_avg = avg_times[j*lenThr:(j*lenThr + lenThr)]
        temp_avg.reverse()
        y.append(temp_avg)
    print(y)

plt.figure()
dimx = int(np.round(np.sqrt(len(y))))
dimy = dimx if dimx*dimx >= len(y) else dimx+1
for i in range(len(y)):
    plt.subplot(dimx, dimy, i+1)
    if i == 0:
        plt.plot(a[:-1], y[i][:-1], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="n = %a " % (numQubits[i]))
        plt.title("Qubits n = %a" "\n" "depth d = %a" % (numQubits[i], deptharr[i]), fontsize=10)
        plt.xticks(a[:-1], nodes[:-1])
    else:
        plt.plot(a, y[i], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="n = %a " % (numQubits[i]))
        plt.title("Qubits n = %i" "\n" "depth d = %a" %(numQubits[i], deptharr[i]), size=10)
        plt.xticks(a, nodes)
        plt.xlabel('number of nodes')
        plt.ylabel('time [s]')
        #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))
plt.suptitle('QuEST benchmark for RQC algortihm (over nodes)')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('../plots/rqc-benchmark-nodes.pdf')
plt.savefig('../plots/rqc-benchmark-nodes')
plt.show()