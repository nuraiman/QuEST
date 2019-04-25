import matplotlib.pyplot as plt
import numpy as np

nodes = [1, 2, 4, 8, 16, 32]  # number of threads
numQubits = [28, 29, 30, 31, 32, 33]  # input number n
a = [1, 2, 3, 4, 5, 6]  # needed for plotting

# readout data from textfile
filenames = ["../output-files/weak-dj-bench-part2.txt"]
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
    # lenThr = len(nodes)
    # y = []
    # for j in range(len(numQubits)):
    #     y.append(avg_times[j*lenThr:(j*lenThr + lenThr)])
    # print(y)

plt.figure(figsize=(10, 10))
# dimx = int(np.round(np.sqrt(len(y))))
# dimy = dimx if dimx*dimx >= len(y) else dimx+1

# plt.subplot(dimx, dimy, i+1)
# y[i] = [x*1e-6 for x in y[i]]
plt.plot(a, avg_times, linestyle='dashed', marker='.', markerfacecolor='black', markersize=7)
# plt.title('n = %a ' % (numQubits[i]), size=10)
plt.xticks(a, nodes)
plt.xlabel('number of nodes')
plt.ylabel('time [ms]')
#plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))
plt.suptitle('QuEST weak scaling for DJ algortihm')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('../plots/weak-dj-bench.pdf')
plt.savefig('../plots/weak-dj-bench')
plt.show()
