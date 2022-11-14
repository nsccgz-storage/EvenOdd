#coding:utf-8


if __name__ == '__main__':
    filename = 'buffer.log'
    with open(filename, 'r', encoding='utf-8') as f:
        lines =  f.readlines()
    
    idx = 0
    bandwith_list = []
    buf_list = []
    bandwith = 0
    buf_size = -1
    cnt = 0

    while idx < len(lines):
        line = lines[idx].split()
        if len(line) == 0:
            if cnt != 0:
                bandwith_list.append(bandwith / cnt)
                buf_list.append(buf_size)
            break
        buf_size_ = eval(line[1])  # MiB
        
        if buf_size_ != buf_size and buf_size != -1:
            bandwith_list.append(bandwith / cnt)
            buf_list.append(buf_size)

            buf_size = buf_size_
            cnt = 0
            bandwith = 0
        buf_size = buf_size_
        fs_size = eval(line[4]) / 1024 # KiB
        line = lines[idx+2].split()
        bandwith += fs_size /1024 /eval(line[1])
        idx += 3
        cnt += 1
    
    from cProfile import label
    import matplotlib.pyplot as plt
    import numpy as np

    bar_width = 3
    index_first = 12 * np.arange(len(buf_list))

    plt.figure()

    plt.bar(index_first, height=bandwith_list, width=bar_width);
   
    # plt.legend()
    plt.xticks(index_first + bar_width/2, buf_list)
    plt.xlabel("buffer size: MiB")
    plt.ylabel('MiB/s')
    plt.savefig('buffer.pdf')