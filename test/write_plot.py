#coding:utf-8
file_name_list = [0,2,4]
x_datas = []
y_datas = []
for idx in file_name_list:
    file_name = 'test_write_' + str(idx) + '.log'
    with open(file_name, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    idx = 0
    fs_list = []
    t_list = []
    while idx < len(lines) and idx + 1 < len(lines):

        file_size = eval(lines[idx].split() [2]) / 1024 / 1024 # MiB
        t = eval(lines[idx + 1].split() [1])

        fs_list.append(file_size)
        t_list.append(t)
        idx += 2
    fs_list1 = fs_list[-8:]
    t_list1 = t_list[-8:]
    x_datas.append(fs_list1)
    y_datas.append(t_list1)

from cProfile import label
import matplotlib.pyplot as plt
import numpy as np

bar_width = 10
index_first = 50 * np.arange(len(x_datas[0]))

plt.figure()

for idx, yy in enumerate(y_datas):
    plt.bar(index_first + idx * bar_width, height=yy, width=bar_width, label='threadpool=' + str(file_name_list[idx]))
plt.legend()
plt.xticks(index_first + bar_width/2, x_datas[0])
plt.xlabel("file size: MiB")
plt.ylabel('latency/s')
plt.savefig('write_3.pdf')