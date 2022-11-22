#include "decoding.h"
#include "encoding.h"
#include "repair.h"
#include "util/util.h"
#include <stdio.h>
#include <sys/time.h>
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

  // struct timeval start;
  // struct timeval end;
  // float time = 0;

  char *op = argv[1];
  if (strcmp(op, "write") == 0) {

    if (argc < 4) {
      usage();
      return -1;
    }
    char *file_path = argv[2];
    int p = atoi(argv[3]);
    RC error_code = encode(file_path, p);

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
    read1(argv[2], argv[3]);

  } else if (strcmp(op, "repair") == 0) {
    if (argc < 3) {
      usage();
      return -1;
    }
    int num_erasures = atoi(argv[2]);
    if (num_erasures < 0 || argc != num_erasures + 3) {
      usage();
      return -1;
    }
    if (num_erasures > 2) {
      printf("Too many corruptions!");
      return -1;
    }
    int disks[2];
    for (int i = 0; i < num_erasures; i++)
      disks[i] = atoi(argv[i + 3]); // assert disk_id is valid number

    repair(num_erasures, disks);
  } else {
    printf("Non-supported operations!\n");
  }
  return 0;
}
