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

filenames2 = ["../output-files/rqc-benchmark-intel.txt"]
master_data2 = []
data2 = [master_data2]

for i, filename in enumerate(filenames2):
    f2 = open(filename, 'r')
    data2[i] = f2.read()
    f2.close()

    temp2 = data2[i].split('\n')

    avg_times2 = [row.split(' ')[4] for row in temp2 if row.startswith("Avg")]
    avg_times2 = [k.split(',')[0] for k in avg_times2]  # get rid of the commas in the list of avg_times

    avg_times2 = [float(num) for num in avg_times2]

    lenThr = len(nodes)
    y2 = []
    for j in range(len(numQubits)):
        temp_avg2 = avg_times2[j*lenThr:(j*lenThr + lenThr)]
        temp_avg2.reverse()
        y2.append(temp_avg2)
    print(y2)


plt.figure(figsize=(12, 10))
dimx = int(np.round(np.sqrt(len(y))))
dimy = dimx if dimx*dimx >= len(y) else dimx+1
for i in range(len(y)):
    y[i] = [x*1e-3 for x in y[i]]
    y2[i] = [x*1e-3 for x in y2[i]]
    plt.subplot(dimx, dimy, i+1)
    if i == 0:
        plt.plot(a[:-1], y[i][:-1], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="QuEST")
        plt.plot(a[:-1], y2[i][:-1], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="Intel-QS")
        plt.title("Qubits n = %a" "\n" "depth d = %a" % (numQubits[i], deptharr[i]), fontsize=10)
        plt.xticks(a[:-1], nodes[:-1])
        plt.xlabel('number of nodes')
        plt.ylabel('time [s]')
        plt.legend(loc='best')
    else:
        plt.plot(a, y[i], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="QuEST")
        plt.plot(a, y2[i], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7, label="Intel-QS")
        plt.title("Qubits n = %i" "\n" "depth d = %a" %(numQubits[i], deptharr[i]), size=10)
        plt.xticks(a, nodes)
        plt.xlabel('number of nodes')
        plt.ylabel('time [s]')
        plt.legend(loc='best')
plt.suptitle('RQC-algortihm, strong scaling')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('../plots/rqc-benchmark.pdf')
plt.show()