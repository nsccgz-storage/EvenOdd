# mv ../evenodd ./
# mv ../time_check ./

cd ../build
if [ $# != 2 ]; then
    echo "usage: bash correct.sh <file_name> <prime>"
    exit 1
fi
file_name=$1
index_1=0
index_2=0
prime=$2


#测试write时间
./time_check write $file_name $prime
echo ====================================

# # #测试read and repair时间
mkdir ./test_data
# ## test no filed failed
./time_check read $file_name ./test_data/data_read_20
diff $file_name ./test_data/data_read_20
rm -rf ./test_data/data_read_0
echo ====================================

## test two failed
### case1 行校验列和对角线校验列丢失
let "index_1=prime"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_21
diff $file_name ./test_data/data_read_21
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./test_data/data_read_21
echo ====================================

### case2 行校验列和数据列丢失
let "index_1=prime-1"
let "index_2=prime"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_22
diff $file_name ./test_data/data_read_22
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./test_data/data_read_22
echo ====================================

### case3 对角线校验列和数据列丢失
let "index_1=prime-2"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_23
diff $file_name ./test_data/data_read_23
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./test_data/data_read_23
echo ====================================

### case4 两列数据列丢失
let "index_1=prime-4"
let "index_2=prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_24
diff $file_name ./test_data/data_read_24
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./test_data/data_read_26
echo ====================================

### case5 两列数据列丢失
let "index_1=prime-2"
let "index_2=prime-3"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_25
diff $file_name ./test_data/data_read_25
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./test_data/data_read_25
echo ====================================

## test one failed
### case6 l列数据列丢失
let "index_1=prime-2"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_26
diff $file_name ./test_data/data_read_26
./time_check repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./test_data/data_read_26
echo ====================================
### case7 行校验列丢失
let "index_1=prime"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_27
diff $file_name ./test_data/data_read_27
./time_check repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./test_data/data_read_27
echo ====================================

### case8 对角线校验列丢失
let "index_1=prime+1"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_28
diff $file_name ./test_data/data_read_28
./time_check repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./test_data/data_read_28
echo ====================================

