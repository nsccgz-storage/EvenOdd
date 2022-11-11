#!/bin/bash
#!/bin/bash

#!/bin/bash  
  
if [ $# != 1 ]; then
    echo "usage: bash correct.sh <test_numbers>"
    exit 1
fi

test_number=$1
file_size=1
skip=1
for ((i=1;i<=test_number;i++))
do

    #生成测试文件
    let "file_size = skip * 1024"
    mkdir ./test_data_"$skip"_KB
    dd if=/dev/urandom of=./test_data_"$skip"_KB/data bs=$file_size count=1 iflag=fullblock
    
    
    ## test two files failed
    ### case 1:
    echo "============================"
    #rm -rf disk_*
    ./evenodd write ./test_data_"$skip"_KB/data 5
    rm disk_[5,6]/*
    ./evenodd read data data_read
    
    echo `diff ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read`

    # ### case 2:
    # echo "============================"
    # rm -rf disk_*
    # # ../bin/evenodd write ../data/$INPUT 5
    # ../bin/evenoddcpp write ../data/$INPUT 5
    # rm disk_[3,6]/*

    # ../bin/evenodd read $INPUT result/testread_22
    # diff ../data/$INPUT result/testread_22

    # ### case 3:
    # echo "============================"
    # rm -rf disk_*
    # # ../bin/evenodd write ../data/$INPUT 5
    # ../bin/evenoddcpp write ../data/$INPUT 5
    # rm disk_[4,5]/*

    # ../bin/evenodd read $INPUT result/testread_23
    # diff ../data/$INPUT result/testread_23

    # ### case 4:
    # echo "============================"
    # rm -rf disk_*
    # # ../bin/evenodd write ../data/$INPUT 5
    # ../bin/evenoddcpp write ../data/$INPUT 5
    # rm -r disk_[0,2]

    # ../bin/evenodd read $INPUT result/testread_24
    # diff ../data/$INPUT result/testread_24
    #     #先把上一次测试数据删除
    #     rm -rf disk_*
    #     ./evenodd write ./test_data_"$skip"_KB/data 5
    #     echo test_file_name:./test_data_"$skip"_KB/data
    #     echo test_file_size:$file_size
    #     echo test_prime:7
    let "skip = skip * 8"
done


