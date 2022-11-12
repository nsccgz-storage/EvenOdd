#include "decoding.h"
#include "encoding.h"
#include <sys/time.h>       
#include <stdio.h>     
#include <string.h> 
#include <stdlib.h>

void usage() {
  printf("./evenodd write <file_name> <p>\n");
  printf("./evenodd read <file_name> <save_as>\n");
  printf("./evenodd repair <number_erasures> <idx0> ...\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
    return -1;
  }

  char *op = argv[1];
  if (strcmp(op, "write") == 0) {

    if (argc < 4) {
      usage();
      return -1;
    }
    char *file_path = argv[2];
    int p = atoi(argv[3]);
    struct timeval start;
    struct timeval end;
    float time = 0;
    float average_time = 0;
    for(int i = 0; i < 1; i++){
        gettimeofday(&start,NULL);
        RC error_code = encode(file_path, p);
        gettimeofday(&end,NULL);
        time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
        printf("write数据消耗时间:%f s\n ",time);

        //average_time += time;

    }
    
  } else if (strcmp(op, "read") == 0) {
    /*
     * Please read the file specified by "file_name", and store it as a file
     * named "save_as" in the local file system.
     * For example: Suppose "file_name" is "testfile" (which we have encoded
     * before), and "save_as" is "tmp_file". After the read operation, there
     * should be a file named "tmp_file", which is the same as "testfile".
     */
    if (argc < 4) {
      usage();
      return -1;
    }
    // myRead(argv[2], argv[3]);
    struct timeval start;
    struct timeval end;
    float time = 0;
    float average_time = 0;
    
    for(int i = 0; i < 1; i++){
        gettimeofday(&start,NULL);
        read1(argv[2], argv[3]);
        gettimeofday(&end,NULL);
        time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
        printf("read数据消耗时间:%f s\n ",time);

        //average_time += time;

    }
    
  } else if (strcmp(op, "repair") == 0) {
    /*
     * Please repair failed disks. The number of failures is specified by
     * "num_erasures", and the index of disks are provided in the command
     * line parameters.
     * For example: Suppose "number_erasures" is 2, and the indices of
     * failed disks are "0" and "1". After the repair operation, the data
     * splits in folder "disk_0" and "disk_1" should be repaired.
     */

  } else {
    printf("Non-supported operations!\n");
  }
  return 0;
}

