cd ..
rm -rf build
mkdir build
cp time_check build/
cd build
mkdir ./test_data
mkdir ./temp


#文件大小为1MB
dd if=/dev/urandom of=./test_data/data_1MB bs=1M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_1MB 3
#测试read and repair时间
##行校验列和对角线校验列丢失
index_1=0
index_2=0
let "index_1=3"
let "index_2=4"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_1MB ./temp/data_1MB
diff ./test_data/data_1MB ./temp/data_1MB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_1MB
rm -rf ./test_data/data_1MB
echo ====================================

#文件大小为50MB
dd if=/dev/urandom of=./test_data/data_50MB bs=50M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_50MB 5
#测试read and repair时间
##行校验列和数据列丢失
let "index_1=5"
let "index_2=3"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_50MB ./temp/data_50MB
diff ./test_data/data_50MB ./temp/data_50MB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_50MB
rm -rf ./test_data/data_50MB
echo ====================================

#文件大小为200MB
dd if=/dev/urandom of=./test_data/data_200MB bs=200M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_200MB 7
#测试read and repair时间
##对角线校验列和数据列丢失
let "index_1=8"
let "index_2=5"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_200MB ./temp/data_200MB
diff ./test_data/data_200MB ./temp/data_200MB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_200MB
rm -rf ./test_data/data_200MB
echo ====================================

#文件大小为400MB
dd if=/dev/urandom of=./test_data/data_400MB bs=400M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_400MB 7
#测试read and repair时间
##两列数据列丢失
let "index_1=3"
let "index_2=5"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_400MB ./temp/data_400MB
diff ./test_data/data_400MB ./temp/data_400MB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_400MB
rm -rf ./test_data/data_400MB
echo ====================================

#文件大小为600MB
dd if=/dev/urandom of=./test_data/data_600MB bs=600M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_600MB 13
#测试read and repair时间
##两列数据列丢失
let "index_1=11"
let "index_2=5"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_600MB ./temp/data_600MB
diff ./test_data/data_600MB ./temp/data_600MB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_600MB
rm -rf ./test_data/data_600MB
echo ====================================

#文件大小为800MB
dd if=/dev/urandom of=./test_data/data_800MB bs=800M count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_800MB 17
#测试read and repair时间
##l列数据列丢失
let "index_1=11"
mv disk_$index_1 _disk_$index_1
./time_check read ./test_data/data_800MB ./temp/data_800MB
diff ./test_data/data_800MB ./temp/data_800MB
./time_check repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./temp/data_800MB
rm -rf ./test_data/data_800MB
echo ====================================

#文件大小为1GB
dd if=/dev/urandom of=./test_data/data_1GB bs=1G count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_1GB 13
#测试read and repair时间
##两列数据列丢失
let "index_1=11"
let "index_2=6"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_1GB ./temp/data_1GB
diff ./test_data/data_1GB ./temp/data_1GB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_1GB
rm -rf ./test_data/data_1GB
echo ====================================

# #文件大小为1GB
# dd if=/dev/urandom of=./test_data/data_1GB bs=1G count=1 iflag=fullblock
# #测试write时间
# ./time_check write ./test_data/data_1GB 23
# #测试read and repair时间
# ##行校验列丢失
# let "index_1=23"
# mv disk_$index_1 _disk_$index_1
# ./time_check read ./test_data/data_1GB ./temp/data_1GB
# diff ./test_data/data_1GB ./temp/data_1GB
# ./time_check repair 1 $index_1
# diff -Naru ./disk_$index_1/ ./_disk_$index_1/
# rm -rf _disk_*
# rm -rf ./temp/data_1GB
# rm -rf ./test_data/data_1GB
# echo ====================================

#文件大小为2GB
dd if=/dev/urandom of=./test_data/data_2GB bs=2G count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_2GB 29
#测试read and repair时间
##对角线校验列丢失
let "index_1=30"
mv disk_$index_1 _disk_$index_1
./time_check read ./test_data/data_2GB ./temp/data_2GB
diff ./test_data/data_2GB ./temp/data_2GB
./time_check repair 1 $index_1
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
rm -rf _disk_*
rm -rf ./temp/data_2GB
rm -rf ./test_data/data_2GB
echo ====================================

#文件大小为1GB
dd if=/dev/urandom of=./test_data/data_3GB bs=3G count=1 iflag=fullblock
#测试write时间
./time_check write ./test_data/data_3GB 17
#测试read and repair时间
##两列数据列丢失
let "index_1=13"
let "index_2=7"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data_3GB ./temp/data_3GB
diff ./test_data/data_3GB ./temp/data_3GB
./time_check repair 2 $index_1 $index_2
diff -Naru ./disk_$index_1/ ./_disk_$index_1/
diff -Naru ./disk_$index_2/ ./_disk_$index_2/
rm -rf _disk_*
rm -rf ./temp/data_3GB
rm -rf ./test_data/data_3GB
echo ====================================

g++ ../wyn_time/sum_time.cpp -o ../wyn_time/sum_time
../wyn_time/sum_time
rm -rf ../wyn_time/write_time.txt
rm -rf ../wyn_time/read_time.txt
