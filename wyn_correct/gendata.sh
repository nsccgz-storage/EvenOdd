cd ..
mkdir correct_data
#文件大小为50MB
dd if=/dev/urandom of=./correct_data/data_50MB bs=50M count=1 iflag=fullblock
#文件大小为200MB
dd if=/dev/urandom of=./correct_data/data_200MB bs=200M count=1 iflag=fullblock
#文件大小为400MB
dd if=/dev/urandom of=./correct_data/data_400MB bs=400M count=1 iflag=fullblock
#文件大小为600MB
dd if=/dev/urandom of=./correct_data/data_600MB bs=600M count=1 iflag=fullblock
