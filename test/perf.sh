# ! /bin/bash

test_data=`realpath ../test_data/data_600MB`

echo $test_data
evenodd=`realpath ../evenodd`

mkdir -p tmp_test
pushd .
cd tmp_test
$evenodd write $test_data 13 &
sudo perf record -g -e cpu-clock -p `pidof evenodd`
sudo perf script -i perf.data &> perf.unfold
~/pkgs/FlameGraph/stackcollapse-perf.pl perf.unfold &> perf.folded
~/pkgs/FlameGraph/flamegraph.pl perf.folded > perf.svg

popd
