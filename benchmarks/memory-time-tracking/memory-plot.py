import matplotlib.pyplot as plt
import numpy as np

nodes=[1, 2, 4, 8, 16, 32, 64, 128]
ranks=[2*x for x in nodes]
start = 28
stop = start + len(nodes)
qubits = np.arange(start,stop,1)
a = np.arange(1,1+len(nodes),1)

memory = [3.01, 3.22, 4.07, 4.3, 4.73, 4.94, 4.3, 4.51] # memory 10^10 bytes
time = [9.45, 11.68, 16.81, 17.28, 19.32, 19.97, 21.24, 21.01] # time [s]

fig = plt.figure(figsize=(10, 6))
fig.suptitle("DJ: Tracing memory and time per rank (QuEST) \n (using 2 ranks/node)")

ax = fig.add_subplot(1, 2, 1)
ax.set_title("time tracking")
ax.plot(a, time, marker='.', markerfacecolor='black', markersize=10)
ax.set_xticks(a)
ax.set_xticklabels(nodes)
ax.set_ylabel("time/node [s]")
ax.set_xlabel("number of nodes")
ax2 = ax.twiny()
ax2.set_xlabel("number of qubits")
ax2.set_xticks(a)
ax2.set_xticklabels(qubits)
ax2.set_xlim(ax.get_xlim())
ax.grid()


ax = fig.add_subplot(1, 2, 2)
ax.set_title("memory tracking")
ax.plot(a, memory, marker='.', markerfacecolor='black', markersize=10)
ax.set_xticks(a)
ax.set_xticklabels(nodes)
ax.set_ylabel("memory/node [10 GB]")
ax.set_xlabel("number of nodes")
ax2 = ax.twiny()
ax2.set_xlabel("number of qubits")
ax2.set_xticks(a)
ax2.set_xticklabels(qubits)
ax2.set_xlim(ax.get_xlim())
ax.grid()


fig.tight_layout()
fig.subplots_adjust(top=0.86)
fig.savefig("dj-memory-time.pdf")