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
    #print(y2)



plt.figure(figsize=(12, 10))
dimx = int(np.round(np.sqrt(len(y))))
dimy = dimx if dimx*dimx >= len(y) else dimx+1
for i in range(len(y)):
    y[i] = [x*1e-3 for x in y[i]]
    y2[i] = [x*1e-3 for x in y2[i]]
    plt.subplot(dimx, dimy, i+1)

    q_T1 = y[i][0]
    q_speedup = [q_T1 / x for x in y[i]]
    #q_Eff = [q_speedup[k] / (2 ** k) for k in range(len(q_speedup))]

    i_T1 = y2[i][0]
    i_speedup = [i_T1 / x for x in y2[i]]
    #i_Eff = [i_speedup[k] / (2 ** k) for k in range(len(i_speedup))]

    #a_ideal = np.ones_like(a)

    if i == 0:
        plt.plot(nodes[:-1], q_speedup[:-1], marker='.', markerfacecolor='black', markersize=10, label="QuEST")
        plt.plot(nodes[:-1], i_speedup[:-1], marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
        plt.plot(nodes[:-1], nodes[:-1], linestyle='dashed', color='grey', label="ideal")
        plt.title("Qubits n = %a" "\n" "depth d = %a" % (numQubits[i], deptharr[i]), fontsize=10)
        plt.xticks(nodes[:-1])
        plt.yticks([1,2,4,6,8])
        plt.xlabel('number of nodes')
        plt.ylabel('speedup ratio')
        plt.grid()
        plt.legend(loc='best')
    else:
        plt.plot(nodes, q_speedup, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
        plt.plot(nodes, i_speedup, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
        plt.plot(nodes, nodes, linestyle='dashed', color='grey', label="ideal")
        plt.title("Qubits n = %i" "\n" "depth d = %a" %(numQubits[i], deptharr[i]), size=10)
        plt.xticks(nodes)
        plt.yticks([1,4,8,12,16])
        plt.xlabel('number of nodes')
        plt.ylabel('speedup ratio')
        plt.grid()
        plt.legend(loc='best')
plt.suptitle('RQC-algortihm, strong scaling')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('../plots/rqc-strong-speedup.pdf')

plt.figure(figsize=(12, 10))
dimx = int(np.round(np.sqrt(len(y))))
dimy = dimx if dimx*dimx >= len(y) else dimx+1
for i in range(len(y)):
    y[i] = [x*1e-3 for x in y[i]]
    y2[i] = [x*1e-3 for x in y2[i]]
    plt.subplot(dimx, dimy, i+1)

    q_T1 = y[i][0]
    q_speedup = [q_T1 / x for x in y[i]]
    q_Eff = [q_speedup[k] / (2 ** k) for k in range(len(q_speedup))]

    i_T1 = y2[i][0]
    i_speedup = [i_T1 / x for x in y2[i]]
    i_Eff = [i_speedup[k] / (2 ** k) for k in range(len(i_speedup))]

    a_ideal = np.ones_like(a)

    if i == 0:
        plt.plot(nodes[:-1], q_Eff[:-1], marker='.', markerfacecolor='black', markersize=10, label="QuEST")
        plt.plot(nodes[:-1], i_Eff[:-1], marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
        plt.plot(nodes[:-1], a_ideal[:-1], linestyle='dashed', color='grey', label="ideal")
        plt.title("Qubits n = %a" "\n" "depth d = %a" % (numQubits[i], deptharr[i]), fontsize=10)
        plt.xticks(nodes[:-1])
        #plt.yticks([1,2,4,6,8])
        plt.xlabel('number of nodes')
        plt.ylabel('parallel efficiency')
        plt.grid()
        plt.legend(loc='best')
    else:
        plt.plot(nodes, q_Eff, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
        plt.plot(nodes, i_Eff, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
        plt.plot(nodes, a_ideal, linestyle='dashed', color='grey', label="ideal")
        plt.title("Qubits n = %i" "\n" "depth d = %a" %(numQubits[i], deptharr[i]), size=10)
        plt.xticks(nodes)
        #plt.yticks([1,4,8,12,16])
        plt.xlabel('number of nodes')
        plt.ylabel('parallel efficiency')
        plt.grid()
        plt.legend(loc='best')
plt.suptitle('RQC-algortihm, strong scaling')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('../plots/rqc-strong-parallelEff.pdf')