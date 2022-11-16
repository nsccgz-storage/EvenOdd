#coding:utf-8

file_list = [16, 32, 64, 128, 256, 512, 1024, 2048]

x_list = []
y_list = []
for num in file_list:
    file_name = 'decoding_' + str(num) + '.log'

    diff = []
    lantency = []
    with open(file_name, 'r', encoding='utf-8') as f:
        lines = f.readlines()

        idx = 0

        while idx < len(lines) and idx + 1 < len(lines):

            if lines[idx + 1].split()[0].find("========") != -1:
                t = eval(lines[idx].split()[1])
                lantency.append(t)
            idx += 1

    y_list.append(lantency)

from cProfile import label
import matplotlib.pyplot as plt
import numpy as np

bar_width = 1
index_first = (len(file_list) + 1) * bar_width *  np.arange(len(y_list[0])) 

plt.figure()
x_data = ['cond.' + str(i) for i in range(len(y_list[0]))]
for idx, yy in enumerate(y_list):
    plt.bar(index_first + idx * bar_width, height=yy, width=bar_width, label='size:' + str(file_list[idx]) + 'MiB')
plt.legend()
plt.xticks(index_first + bar_width * len(file_list) / 2, x_data)
plt.xlabel("recover read in different situation")
plt.ylabel('latency/s')
plt.savefig('read.pdf')