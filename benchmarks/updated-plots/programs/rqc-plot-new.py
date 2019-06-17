import matplotlib.pyplot as plt
import numpy as np

nodes = [1, 2, 4, 8, 16, 32, 64, 128]  # number of threads
row = [4, 5]
depth = [20, 40, 60]
a = np.arange(1, 1+len(nodes), 1)

numQubits = []
deptharr = []
for num in row:
    for dep in depth:
        numQubits.append(num**2)
        numQubits.append(num*(num+1))
        deptharr.append(dep)
        deptharr.append(dep)


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
        for j in range(len(numQubits)):
            y.append(avg_times[j*lenThr:(j*lenThr + lenThr)])
        #print(y)

    return y


def plot_data(y_quest, y_intel, strong=False, speedup=False, parEff=False):

    plt.figure(figsize=(12, 10))
    end = len(y_quest)
    dimx = int(np.round(np.sqrt(end)))
    dimy = dimx if dimx*dimx >= end else dimx+1
    #order = [1, 2, 5, 6, 9, 10, 3, 4, 7, 8, 11, 12]  # hardcoded order for this specific data output
    order = [1, 4, 7, 2, 3, 5, 6, 8, 9]  # hardcoded order for this specific data output, without numQub = 16 plots
    for i in range(end):
        plt.subplot(dimx, dimy, order[i])
        y_quest[i], y_intel[i], time_label = scale_time(y_quest[i], y_intel[i])

        q_T1 = y_quest[i][0]
        q_speedup = [q_T1 / x for x in y_quest[i]]
        q_Eff = [q_speedup[k] / (2 ** k) for k in range(len(q_speedup))]

        i_T1 = y_intel[i][0]
        i_speedup = [i_T1 / x for x in y_intel[i]]
        i_Eff = [i_speedup[k] / (2 ** k) for k in range(len(i_speedup))]

        a_ideal = np.ones_like(a)

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
        plt.title("Qubits n = %i" "\n" "depth d = %a" %(numQubits[i], deptharr[i]), size=10)
        plt.xticks(a, nodes)
        plt.xlabel('number of nodes')
        plt.grid()
        plt.legend(loc='best')
        #plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05))

    plt.suptitle("RQC-algorithm, strong scaling \n (2 ranks/node, 18 threads)")
    plt.tight_layout()
    plt.subplots_adjust(top=0.86)
    if strong:
        plt.savefig('../rqc-strong-new.pdf')
    elif speedup:
        plt.savefig('../rqc-strong-new-speedup.pdf')
    elif parEff:
        plt.savefig('../rqc-strong-new-parEff.pdf')
#plt.show()


times_quest = read_file("../files/rqc_full_quest.txt")
times_intel = read_file("../files/rqc_full_intel.txt")

# delete data with numQub = 16
index = [0, 1, 2]
for ind in index:
    times_quest.pop(ind)
    times_intel.pop(ind)
    numQubits.pop(ind)
    deptharr.pop(ind)

plot_data(times_quest, times_intel, strong=True)
plot_data(times_quest, times_intel, speedup=True)
plot_data(times_quest, times_intel, parEff=True)
