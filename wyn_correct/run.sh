echo "正确性测试案例1"
cd ..
rm -rf build
mkdir build
cp evenodd build/
cd build
mkdir ./temp
echo "文件:data_50MB,素数:5"
../wyn_correct/correct.sh ../correct_data/data_50MB 5
echo "文件:data_200MB,素数:7"
../wyn_correct/correct.sh ../correct_data/data_200MB 7
echo "文件:data_400MB,素数:11"
../wyn_correct/correct.sh ../correct_data/data_400MB 11
echo "文件:data_600MB,素数:13"
../wyn_correct/correct.sh ../correct_data/data_600MB 13
echo "文件:data_3GB,素数:17"
../wyn_correct/correct.sh ../correct_data/data_3GB 17
echo "正确性测试案例2"
cd ..
rm -rf build
mkdir build
cp evenodd build/
cd build
mkdir ./temp
echo "文件:data_50MB,素数:17"
../wyn_correct/correct.sh ../correct_data/data_50MB 17
echo "文件:data_200MB,素数:13"
../wyn_correct/correct.sh ../correct_data/data_200MB 13
echo "文件:data_400MB,素数:11"
../wyn_correct/correct.sh ../correct_data/data_400MB 11
echo "文件:data_600MB,素数:7"
../wyn_correct/correct.sh ../correct_data/data_600MB 7
echo "文件:data_3GB,素数:5"
../wyn_correct/correct.sh ../correct_data/data_3GB 5
echo "正确性测试案例3"
cd ..
rm -rf build
mkdir build
cp evenodd build/
cd build
mkdir ./temp
echo "文件:data_50MB,素数:13"
../wyn_correct/correct.sh ../correct_data/data_50MB 13
echo "文件:data_200MB,素数:11"
../wyn_correct/correct.sh ../correct_data/data_200MB 11
echo "文件:data_400MB,素数:7"
../wyn_correct/correct.sh ../correct_data/data_400MB 7
echo "文件:data_600MB,素数:5"
../wyn_correct/correct.sh ../correct_data/data_600MB 5
echo "文件:data_3GB,素数:17"
../wyn_correct/correct.sh ../correct_data/data_3GB 17


