import matplotlib.pyplot as plt
import numpy as np

nodes = [1, 2, 4, 8, 16, 32, 64, 128]  # number of threads
numQubits = [16, 19, 22, 24, 26, 27, 28, 29, 30, 31]  # input number n
numQubits = [x + 1 for x in numQubits]
a = np.arange(1, 1+len(nodes), 1)


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

        avg_times = [row.split(' ')[4] for row in temp if row.startswith("Avg")]
        avg_times = [k.split(',')[0] for k in avg_times]  # get rid of the commas in the list of avg_times

        avg_times = [float(num) for num in avg_times]
        lenThr = len(nodes)
        y = []
        for j in range(len(numQubits)-2):
            y.append(avg_times[j*lenThr:(j*lenThr + lenThr)])
        # print(y)
        # treat special cases (big examples N=30 and N=31)
        j = len(numQubits)-2
        start = j*lenThr
        end = start + (lenThr-2)
        y.append(avg_times[start:end])

        j = len(numQubits)-1
        start = j*lenThr - 2
        end = start + (lenThr-3)
        y.append(avg_times[start:end])

    return y


def plot_data(numq, y_quest, y_intel, strong=False, speedup=False, parEff=False):
    # if strong:
    #     plt.figure(figsize=(12, 8))
    #     end = len(y_quest)
    # elif speedup:
    #     plt.figure(figsize=(10, 10))
    #     end = len(y_quest)-2
    # elif parEff:
    #     plt.figure(figsize=(10, 10))
    #     end = len(y_quest)-2
    plt.figure(figsize=(12, 8))
    end = len(y_quest)
    dimx = int(np.round(np.sqrt(end)))
    dimy = dimx if dimx*dimx >= end else dimx+1
    for i in range(end):
        plt.subplot(dimx, dimy, i+1)
        y_quest[i], y_intel[i], time_label = scale_time(y_quest[i], y_intel[i])

        q_T1 = y_quest[i][0]
        q_speedup = [q_T1 / x for x in y_quest[i]]
        q_Eff = [q_speedup[k] / (2 ** k) for k in range(len(q_speedup))]

        i_T1 = y_intel[i][0]
        i_speedup = [i_T1 / x for x in y_intel[i]]
        i_Eff = [i_speedup[k] / (2 ** k) for k in range(len(i_speedup))]

        a_ideal = np.ones_like(a)

        if i == len(y_quest)-2:
            if strong:
                plt.plot(a[2:], y_quest[i],  marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a[2:], y_intel[i], marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.ylabel(time_label)
            elif speedup:
                nodes_plot = [int(x/4) for x in nodes]
                plt.semilogy(a[2:], q_speedup, basey=2, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.semilogy(a[2:], i_speedup, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.semilogy(a[2:], nodes_plot[2:], linestyle='dashed', color='grey', label="ideal")
                plt.yticks(nodes_plot[2:], nodes_plot[2:])
                plt.ylabel('speedup ratio')
            elif parEff:
                plt.plot(a[2:], q_Eff, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a[2:], i_Eff, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.plot(a[2:], a_ideal[2:], linestyle='dashed', color='grey', label="ideal")
                plt.ylabel('parallel efficiency')
            plt.title('Qubits n = %a ' % (numq[i]), fontsize=10)
            plt.xticks(a[2:], nodes[2:])
            plt.xlabel('number of nodes')
            plt.grid()
            plt.legend(loc='best')
            #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))

        elif i == len(y_quest)-1:
            if strong:
                plt.plot(a[3:], y_quest[i],  marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a[3:], y_intel[i], marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.ylabel(time_label)
            elif speedup:
                nodes_plot = [int(x/8) for x in nodes]
                plt.semilogy(a[3:], q_speedup, basey=2, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.semilogy(a[3:], i_speedup, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.semilogy(a[3:], nodes_plot[3:], linestyle='dashed', color='grey', label="ideal")
                plt.yticks(nodes_plot[3:], nodes_plot[3:])
                plt.ylabel('speedup ratio')
            elif parEff:
                plt.plot(a[3:], q_Eff, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a[3:], i_Eff, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.plot(a[3:], a_ideal[3:], linestyle='dashed', color='grey', label="ideal")
                plt.ylabel('parallel efficiency')
            plt.title('Qubits n = %a ' % (numq[i]), fontsize=10)
            plt.xticks(a[3:], nodes[3:])
            plt.xlabel('number of nodes')
            plt.grid()
            plt.legend(loc='best')
            #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))
        else:
            if strong:
                plt.plot(a, y_quest[i],  marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a, y_intel[i], marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.ylabel(time_label)
            elif speedup:
                plt.semilogy(a, q_speedup, basey=2, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.semilogy(a, i_speedup, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.semilogy(a, nodes, linestyle='dashed', color='grey', label="ideal")
                plt.yticks(nodes, nodes)
                plt.ylabel('speedup ratio')
            elif parEff:
                plt.plot(a, q_Eff, marker='.', markerfacecolor='black', markersize=10, label="QuEST")
                plt.plot(a, i_Eff, marker='.', markerfacecolor='black', markersize=10, label="Intel-QS")
                plt.plot(a, a_ideal, linestyle='dashed', color='grey', label="ideal")
                plt.ylabel('parallel efficiency')
            plt.title('Qubits n = %a ' % (numq[i]), fontsize=10)
            plt.xticks(a, nodes)
            plt.xlabel('number of nodes')
            plt.grid()
            plt.legend(loc='best')
            #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))

    plt.suptitle("DJ-algorithm, strong scaling \n (2 ranks/node, 18 threads)")
    plt.tight_layout()
    plt.subplots_adjust(top=0.86)
    if strong:
        plt.savefig('../dj-strong-new.pdf')
    elif speedup:
        plt.savefig('../dj-strong-new-speedup.pdf')
    elif parEff:
        plt.savefig('../dj-strong-new-parEff.pdf')
#plt.show()


times_quest = read_file("../files/dj_full_quest.txt")
times_intel = read_file("../files/dj_full_intel.txt")
times_quest = times_quest[1:]
times_intel = times_intel[1:]
numQ = numQubits[1:]

plot_data(numQ, times_quest, times_intel, strong=True)
plot_data(numQ, times_quest, times_intel, speedup=True)
plot_data(numQ, times_quest, times_intel, parEff=True)
