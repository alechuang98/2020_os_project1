import os
import sys
import matplotlib.pyplot as plt

test_data = 'test_data'
output = 'output'
time_file = "TIME_MEASUREMENT_dmesg.txt"
log_file = 'log_files'
png = 'png'

def get_unit():
    tot_time = 0
    with open(os.path.join(output, time_file), "r") as f:
        for line in f:
            line = line.strip("\n").split(" ")
            print(line)
            start = float(line[3])
            end = float(line[4][:-1])
            tot_time += end - start
            print("{:.12f}, {:.12f}".format(start, end))
    return tot_time / 5000

def get_unit_new():
    tot_time = 0
    min_time = 1e12
    max_time = 0
    with open(os.path.join(output, time_file), "r") as f:
        for line in f:
            line = line.strip("\n").split(" ")
            print(line)
            min_time = min(min_time, float(line[3]))
            max_time = max(max_time, float(line[4]))
    return (max_time - min_time) / 9500

def draw(unit):
    for file_name in os.listdir(test_data):
        dmesg = os.path.join(output, file_name.split('.')[0] + "_dmesg.txt")
        stdout = os.path.join(output, file_name.split('.')[0] + "_stdout.txt")
        data = os.path.join(test_data, file_name)
        log = os.path.join(log_file, file_name.split('.')[0] + "_stdout.txt")
        
        print(dmesg, stdout, data, log)

        pid_to_name = {}
        exp = {}
        real = {}
        with open(stdout, "r") as f:
            for line in f:
                line = line.strip("\n").split(" ")
                pid_to_name[line[1]] = line[0]
        
        mn_time = float(1e18)
        with open(dmesg, "r") as f:
            for line in f:
                line = line.split(" ")
                print(line)
                start = float(line[3]) / unit
                end = float(line[4][:-1]) / unit
                mn_time = min(start, mn_time)
                exp[pid_to_name[line[2]]] = [start, end]
            for k, v in exp.items():
                exp[k][0] -= mn_time
                exp[k][1] -= mn_time
        mn_time = int(1e18)
        real_arr = {}
        with open(data, "r") as f:
            for line in f:
                line = line.strip("\n").split(" ")
                if len(line) != 3:
                    continue
                mn_time = min(mn_time, int(line[1]))
                real_arr[line[0]] = int(line[1])
                real[line[0]] = []
            for k, v in real_arr.items():
                real_arr[k] -= mn_time
        with open(log, "r") as f:
            cur_time = 0
            first_task = 0
            for line in f:
                if not line.startswith("["):
                    continue
                line = line.split(" ")
                name = line[0][1:-1]
                if name == '$':
                    if first_task == 1:
                        cur_time += int(line[1])
                else:
                    first_task = 1
                    time = int(line[1])
                    real[name].append((cur_time, time))
                    cur_time += time

        width = cur_time / 150
        # width = 1
        y_label = []
        y_tick = []
        fig, ax = plt.subplots()
        y = 1
        ax.broken_barh([(0, 0)], (0, 0), color='tab:blue', label='theoretic execute')
        ax.broken_barh([(0, 0)], (0, 0), color='tab:orange', label='theoretic arrival')
        ax.broken_barh([(0, 0)], (0, 0), color='tab:green', label='real time arrival')
        ax.broken_barh([(0, 0)], (0, 0), color='tab:red', label='real time finish')
        for k, v in real.items():
            ax.broken_barh(v, (y, 9), color='tab:blue')
            y_label
            ax.broken_barh([(real_arr[k], width)], (y, 9), color='tab:orange')
            ax.broken_barh([(exp[k][0], width), (exp[k][1], width)], (y, 9), color=('tab:green', 'tab:red'))
            y_label.append(k)
            y_tick.append(y + 9 / 2)
            y += 10
        ax.set_yticks(y_tick)
        ax.set_yticklabels(y_label)
        plt.title(file_name.split(".")[0], loc='right')
        ax.legend(ncol=2, bbox_to_anchor=(0, 1),loc='lower left', fontsize='small')
        plt.savefig(os.path.join(png, file_name.split(".")[0] + ".png"))
        plt.close()


print(get_unit())
print(get_unit_new())
draw(get_unit_new())

