import matplotlib.pyplot as plt
import numpy as np

# readout data from textfile
def read_file(name):
    qubits = []
    nodes = []
    gates = []
    time = []
    position = []
    startPos = 25

    f = open(name, "r")
    for line in f.readlines():
        if "CONFIGURATION: " in line:
            split_line = line.split()
            N = int(split_line[3][:-1])
            n = int(split_line[6])
            n_ranks = 2 * nodes
        if "Gate " in line:
            split_line = line.split()
            g = split_line[1][0]
        if "Avg time [ms] = " in line:
            split_line = line.split()
            t = float(split_line[4][:-1])
            k = int(split_line[-1][:-1])
            qubits.insert(len(qubits), N)
            nodes.insert(len(nodes), n)
            gates.insert(len(gates), g)
            time.insert(len(time), t)
            position.insert(len(position), k+startPos)
    f.close()
    return qubits, nodes, gates, time, position

def scale_time(time1, time2):
    new_time1 = time1
    new_time2 = time2
    label = "time [ms]"
    if len(time1) > 0:
        if time1[0] >= 500:
            if len(time2) == 0 or (len(time2) > 0 and time2[0] >= 500):
                label = "time [s]"
                new_time1 = [1.0 * x / 1000.0 for x in time1]
                new_time2 = [1.0 * x / 1000.0 for x in time2]

    return new_time1, new_time2, label

(i_qubits, i_nodes, i_gates, i_time, i_position) = read_file("../output-files/weak-single-qubit-gates-1-36_intel.txt")
(q_qubits, q_nodes, q_gates, q_time, q_position) = read_file("../output-files/weak-single-qubit-gates-1-36_quest.txt")

lenData = len(q_time)

# plotting
i_time = [1.0 * x / 1000.0 for x in i_time]
q_time = [1.0 * x / 1000.0 for x in q_time]
time_label = "time [s]"

fig = plt.figure(figsize=(10, 10))
fig.suptitle("Single-qubit-gates, weak scaling \n (1 rank/node, 36 threads)")
up = 9
down = 0

q_axH = fig.add_subplot(3, 2, 1)
q_axH.set_title("QuEST \n \n Gate H")
q_axH.set_ylabel(time_label)
q_axH.set_ylim(down, up)

i_axH = fig.add_subplot(3, 2, 2)
i_axH.set_title("Intel-QS \n \n Gate H")
i_axH.set_ylim(down, up)

q_axX = fig.add_subplot(3, 2, 3)
q_axX.set_title("Gate X")
q_axX.set_ylabel(time_label)
q_axX.set_ylim(down, up)

i_axX = fig.add_subplot(3, 2, 4)
i_axX.set_title("Gate X")
i_axX.set_ylim(down, up)

q_axT = fig.add_subplot(3, 2, 5)
q_axT.set_title("Gate T")
q_axT.set_xlabel("qubit position")
q_axT.set_ylabel(time_label)
q_axT.set_ylim(down, up)

i_axT = fig.add_subplot(3, 2, 6)
i_axT.set_title("Gate T")
i_axT.set_xlabel("qubit position")
i_axT.set_ylim(down, up)

i = 0

while i < lenData:
    # plot Gate H
    steps = 0
    gate = q_gates[i]
    assert gate == "H", "Error: Gate is not equal to H, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
    q_axH.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label= "%i" % (q_qubits[i]))
    q_axH.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axH.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label= "%i" % (i_qubits[i]))
    i_axH.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

    # plot Gate X
    steps = 0
    gate = q_gates[i]
    assert gate == "X", "Error: Gate is not equal to X, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
    q_axX.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label="%i" % (q_qubits[i]))
    q_axX.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axX.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label="%i" % (i_qubits[i]))
    i_axX.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

    # plot Gate T
    steps = 0
    gate = q_gates[i]
    assert gate == "T", "Error: Gate is not equal to T, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
        if i+steps >= lenData: break
    q_axT.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label="%i" % (q_qubits[i]))
    q_axT.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axT.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label="%i" % (i_qubits[i]))
    i_axT.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

q_axH.legend(loc='upper left', title='qubits')
q_axH.grid()
i_axH.legend(loc='upper left', title='qubits')
i_axH.grid()
q_axX.legend(loc='upper left', title='qubits')
q_axX.grid()
i_axX.legend(loc='upper left', title='qubits')
i_axX.grid()
q_axT.legend(loc='upper left', title='qubits')
q_axT.grid()
i_axT.legend(loc='upper left', title='qubits')
i_axT.grid()

fig.tight_layout()
fig.subplots_adjust(top=0.86)
fig.savefig("../plots/weak-sqg-1-36.pdf")

#######################################################################
#######################################################################

(i_qubits, i_nodes, i_gates, i_time, i_position) = read_file("../output-files/weak-single-qubit-gates-2-18_intel.txt")
(q_qubits, q_nodes, q_gates, q_time, q_position) = read_file("../output-files/weak-single-qubit-gates-2-18_quest.txt")

lenData = len(q_time)

# plotting
i_time = [1.0 * x / 1000.0 for x in i_time]
q_time = [1.0 * x / 1000.0 for x in q_time]
time_label = "time [s]"

fig = plt.figure(figsize=(10, 10))
fig.suptitle("Single-qubit-gates, weak scaling \n (2 ranks/node, 18 threads)")

q_axH = fig.add_subplot(3, 2, 1)
q_axH.set_title("QuEST \n \n Gate H")
q_axH.set_ylabel(time_label)
q_axH.set_ylim(down, up)

i_axH = fig.add_subplot(3, 2, 2)
i_axH.set_title("Intel-QS \n \n Gate H")
i_axH.set_ylim(down, up)

q_axX = fig.add_subplot(3, 2, 3)
q_axX.set_title("Gate X")
q_axX.set_ylabel(time_label)
q_axX.set_ylim(down, up)

i_axX = fig.add_subplot(3, 2, 4)
i_axX.set_title("Gate X")
i_axX.set_ylim(down, up)

q_axT = fig.add_subplot(3, 2, 5)
q_axT.set_title("Gate T")
q_axT.set_xlabel("qubit position")
q_axT.set_ylabel(time_label)
q_axT.set_ylim(down, up)

i_axT = fig.add_subplot(3, 2, 6)
i_axT.set_title("Gate T")
i_axT.set_xlabel("qubit position")
i_axT.set_ylim(down, up)

i = 0

while i < lenData:
    # plot Gate H
    steps = 0
    gate = q_gates[i]
    assert gate == "H", "Error: Gate is not equal to H, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
    q_axH.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label= "%i" % (q_qubits[i]))
    q_axH.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axH.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label= "%i" % (i_qubits[i]))
    i_axH.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

    # plot Gate X
    steps = 0
    gate = q_gates[i]
    assert gate == "X", "Error: Gate is not equal to X, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
    q_axX.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label="%i" % (q_qubits[i]))
    q_axX.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axX.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label="%i" % (i_qubits[i]))
    i_axX.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

    # plot Gate T
    steps = 0
    gate = q_gates[i]
    assert gate == "T", "Error: Gate is not equal to T, as expected."
    while q_gates[i+steps] == gate:
        steps += 1  # get the end-index of the corresponding data chunk
        if i+steps >= lenData: break
    q_axT.plot(q_position[i:(i+steps)], q_time[i:(i+steps)], label="%i" % (q_qubits[i]))
    q_axT.scatter(q_position[i+steps-1], q_time[i+steps-1])
    i_axT.plot(i_position[i:(i+steps)], i_time[i:(i+steps)], label="%i" % (i_qubits[i]))
    i_axT.scatter(i_position[i+steps-1], i_time[i+steps-1])
    i += steps

q_axH.legend(loc='upper left', title='qubits')
q_axH.grid()
i_axH.legend(loc='upper left', title='qubits')
i_axH.grid()
q_axX.legend(loc='upper left', title='qubits')
q_axX.grid()
i_axX.legend(loc='upper left', title='qubits')
i_axX.grid()
q_axT.legend(loc='upper left', title='qubits')
q_axT.grid()
i_axT.legend(loc='upper left', title='qubits')
i_axT.grid()

fig.tight_layout()
fig.subplots_adjust(top=0.86)
fig.savefig("../plots/weak-sqg-2-18.pdf")
