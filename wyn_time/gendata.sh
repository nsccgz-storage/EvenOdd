cd ..
mkdir test_data
#文件大小为1MB
dd if=/dev/urandom of=./test_data/data_1MB bs=1M count=1 iflag=fullblock
#文件大小为50MB
dd if=/dev/urandom of=./test_data/data_50MB bs=50M count=1 iflag=fullblock
#文件大小为200MB
dd if=/dev/urandom of=./test_data/data_200MB bs=200M count=1 iflag=fullblock
#文件大小为400MB
dd if=/dev/urandom of=./test_data/data_400MB bs=400M count=1 iflag=fullblock
#文件大小为600MB
dd if=/dev/urandom of=./test_data/data_600MB bs=600M count=1 iflag=fullblock
#文件大小为800MB
dd if=/dev/urandom of=./test_data/data_800MB bs=800M count=1 iflag=fullblock
#文件大小为1GB
dd if=/dev/urandom of=./test_data/data_1GB bs=1G count=1 iflag=fullblock
#文件大小为2GB
dd if=/dev/urandom of=./test_data/data_2GB bs=2G count=1 iflag=fullblock
#文件大小为3GB
dd if=/dev/urandom of=./test_data/data_3GB bs=3G count=1 iflag=fullblock