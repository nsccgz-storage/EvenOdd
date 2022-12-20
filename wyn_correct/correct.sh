
if [ $# != 2 ]; then
    echo "usage: bash correct.sh <file_path> <prime>"
    exit 1
fi
file_path=$1
index_1=0
index_2=0
prime=$2

#测试write时间
./evenodd write $file_path $prime
#测试read and repair
echo "case1 no failed ================="
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
rm -rf ./temp/data

echo "case2 two files failed ================="
echo "case2.1 行校验列和列校验列丢失"
let "index_1=$prime"
let "index_2=$prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data

echo "case2.2 行校验列和数据列丢失 ================="
let "index_1=$prime"
let "index_2=$prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data

echo "case2.3 对角线校验列和数据列丢失 ================="
let "index_1=$prime+1"
let "index_2=$prime-2"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data

echo "case2.4 两列数据列丢失 ================="
let "index_1=$prime-3"
let "index_2=$prime-2"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data

echo "case3 one file failed ================="
echo "case3.1 行校验列丢失 ================="
let "index_1=$prime"
mv disk_$index_1 _disk_$index_1
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./temp/data

echo "case3.2 对角线校验列丢失 ================="
let "index_1=$prime+1"
mv disk_$index_1 _disk_$index_1
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./temp/data


echo "case3.3 1列数据列丢失 ================="
let "index_1=$prime-2"
mv disk_$index_1 _disk_$index_1
./evenodd read $file_path ./temp/data
diff $file_path ./temp/data
./evenodd repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./temp/data
