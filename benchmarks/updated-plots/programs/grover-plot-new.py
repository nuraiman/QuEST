import matplotlib.pyplot as plt
import numpy as np

nodes = [1, 2, 4, 8, 16, 32, 64, 128]  # number of threads
a = np.arange(1, 1+len(nodes), 1)


def scale_time(time1):
    new_time1 = time1
    label = "time [ms]"
    if len(time1) > 0:
        if time1[0] >= 500:

            label = "time [s]"
            new_time1 = [1.0 * x / 1000.0 for x in time1]

    return new_time1, label


def read_file(f_name):
    # readout data from textfile
    filenames = [f_name]
    master_data = []
    data = [master_data]

    for i, filename in enumerate(filenames):
        f = open(filename, 'r')
        data[i] = f.read()
        f.close()

        temp = data[i].split('\n')

        inputN = [row.split(' ')[3] for row in temp if (row.startswith("CONFIG") and row.endswith("nodes = 1"))]
        inputN = [k.split(',')[0] for k in inputN]
        inputN = [int(num) for num in inputN]

        num_qubits = np.ceil(np.log2(inputN))
        numIt = np.round(np.pi*0.25*np.sqrt(2**num_qubits)-0.5)

        avg_times = [row.split(' ')[4] for row in temp if row.startswith("Avg")]
        avg_times = [k.split(',')[0] for k in avg_times]  # get rid of the commas in the list of avg_times

        avg_times = [float(num) for num in avg_times]
        lenThr = len(nodes)
        y = []
        for j in range(len(inputN)):
            y.append(avg_times[j*lenThr:(j*lenThr + lenThr)])

    return inputN, y, num_qubits, numIt


def plot_data(inputN, numq, numIt, y_quest):

    plt.figure(figsize=(12, 8))
    end = len(y_quest)
    dimx = int(np.round(np.sqrt(end)))
    dimy = dimx if dimx*dimx >= end else dimx+1
    for i in range(end):
        plt.subplot(dimx, dimy, i+1)
        y_quest[i], time_label = scale_time(y_quest[i])

        if i == end-1:
            plt.plot(a[2:], y_quest[i],  marker='.', markerfacecolor='black', markersize=10)
            plt.title("N = %a " "\n" "Qubits n = %i" "\n" "Iterations T = %i" %(inputN[i], numq[i], numIt[i]), size=10)
            plt.ylabel(time_label)
            plt.xticks(a[2:], nodes[2:])
            plt.xlabel('number of nodes')
            plt.grid()
            #plt.legend(loc='best')
            #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))
        else:
            plt.plot(a, y_quest[i],  marker='.', markerfacecolor='black', markersize=10)
            plt.title("N = %a " "\n" "Qubits n = %i" "\n" "Iterations T = %i" %(inputN[i], numq[i], numIt[i]), size=10)
            plt.ylabel(time_label)
            plt.xticks(a, nodes)
            plt.xlabel('number of nodes')
            plt.grid()
            #plt.legend(loc='best')
            #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))

    plt.suptitle("GROVER-algorithm, strong scaling (QuEST) \n (2 ranks/node, 18 threads)")
    plt.tight_layout()
    plt.subplots_adjust(top=0.86)
    plt.savefig('../grover-strong-new.pdf')
    #plt.show()


input_q, times_q, numQub, numT = read_file("../files/grover_full_quest.txt")

plot_data(input_q, numQub, numT, times_q)

