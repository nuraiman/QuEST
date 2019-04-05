import matplotlib.pyplot as plt
import numpy as np

xarr = [1,2,4,8,16,32] # number of threads
y1 = [814, 785, 852, 810, 811, 802]
y2 = [17581, 7311, 7172, 7248, 7316, 7280]
y3 = [10067, 9927, 9972, 9897, 9962, 9805]
y4 = [187620329790, 176207678913, 190598995717, 215412968615, 239985601002, 291893304694]
y5 = [11018, 11551, 10095, 10370, 10270, 10247]


y = [y2, y3, y4, y5]
lb = [203]
numQubits = 3*np.ceil(np.log2(lb))

plt.figure()
for i in range(len(y)):
    plt.subplot(2, 2, i+1)
    y[i] = [x*1e-6 for x in y[i]]
    plt.semilogx(xarr, y[i], linestyle='dashed', marker='.', markerfacecolor='black', markersize=7,
                 label="N = %a " "\n" "Qubits n = %i" %(lb[i], numQubits[i]))
    plt.xlabel('number of threads')
    plt.ylabel('time [ms]')
    plt.legend(loc='best')
plt.suptitle('benchmark for Shor algortihm')
plt.tight_layout()
plt.subplots_adjust(top=0.86)
plt.savefig('shor-benchmark')
plt.show()
