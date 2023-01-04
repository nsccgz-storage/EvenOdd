# echo "第一轮测试"
# echo "case1"
# ./run.sh ../wyn/data_50MB 5
# echo "case2"
# ./run.sh ../wyn/data_100MB 7
# echo "case3"
# ./run.sh ../wyn/data_512MB 13
# echo "case4"
# ./run.sh ../wyn/data_700MB 17

cd ..
./pre.sh
cd wyn
echo "第二轮测试"
echo "case1"
./run.sh ../wyn/data_50MB 17
echo "case2"
./run.sh ../wyn/data_100MB 13
echo "case3"
./run.sh ../wyn/data_512MB 7
echo "case4"
./run.sh ../wyn/data_700MB 5

# cd ..
# ./pre.sh
# cd wyn
# echo "第三轮测试"
# echo "case1"
# ./run.sh ../wyn/data_50MB 7
# echo "case2"
# ./run.sh ../wyn/data_100MB 5
# echo "case3"
# ./run.sh ../wyn/data_512MB 13
# echo "case4"
# ./run.sh ../wyn/data_700MB 17





