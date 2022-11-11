
#include "decoding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef PATH_MAX_LEN
#define PATH_MAX_LEN 512
#endif

static unsigned long block_size;

void readDataColumn(char *filename, int id, int file_size, char *result) {
  FILE *input;
  char file_path[PATH_MAX_LEN];
  sprintf(file_path, "disk_%d/%s", id, filename);
  input = fopen(file_path, "rb");
  fseek(input, sizeof(int), SEEK_SET);
  fread(result, file_size, 1, input);
  fclose(input);
}

size_t writeDataColumn(char *src, int id, size_t size, FILE *stream,
                       int *reset_ptr) {
  if (*reset_ptr) {
    fseek(stream, id * size, SEEK_SET); // 频繁 fseek 会带来很大开销吗？
    *reset_ptr = 0;
  }
  return fwrite(src, 1, size, stream);
}

size_t writeRemain(char *src, size_t size, FILE *stream) {
  fseek(stream, 0, SEEK_END);
  return fwrite(src, 1, size, stream);
}

void readRemain(char *filename, int id, int p, int remain_size, char *result) {
  FILE *input;
  char file_path[PATH_MAX_LEN];
  if (id == p - 1) {
    sprintf(file_path, "disk_%d/%s", id, filename);
    input = fopen(file_path, "rb");
    fseek(input, -remain_size, SEEK_END); // 倒数 remain_size
    fread(result, remain_size, 1, input);
  } else if (id == p || id == p + 1) {
    size_t file_size;
    sprintf(file_path, "disk_%d/%s.remaining", id, filename);
    input = fopen(file_path, "rb");
    fread(result, remain_size, 1, input);
  } else {
    printf("[Error] invalidly reading of remain block");
  }
  fclose(input);
}

void block_xor(char *left, char *right, char *result) {
  for (int i = 0; i < block_size; i++) {
    result[i] = left[i] ^ right[i];
  }
}

void block_xoreq(char *left, char *right) { block_xor(left, right, left); }

/*
 * 两个数据列`同一对角线元素`进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diag(char *left, char *right, int left_id, int right_id, int p) {
  for (int i = 0; i < p - 1; i++) {
    int x = (right_id + i - left_id + p) % p;
    block_xoreq(left + x * block_size, right + i * block_size);
  }
}

/*
 * left 和 diagonal parity 对应元素进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diagparity(char *left, char *diag, int left_id, int p) {
  xoreq_diag(left, diag, left_id, 0, p);
}

/*
 * 暂时假设每个 column 可以直接读入内存
 */
/*
 * Please read the file specified by "file_name", and store it as a file
 * named "save_as" in the local file system.
 * For example: Suppose "file_name" is "testfile" (which we have encoded
 * before), and "save_as" is "tmp_file". After the read operation, there
 * should be a file named "tmp_file", which is the same as "testfile".
 */
void read1(char *path, char *save_as) {
  struct stat st = {0};
  unsigned long file_size = 0;
  unsigned long remain_size =
      0; // the remain file size when source file size can not divide by p*(p-1)
  int p = 0;
  int failed_num = 0;
  int failed[2];
  char file_path[PATH_MAX_LEN];
  char dir_path[PATH_MAX_LEN];

  char *filename = basename(path);

  int disk_id = 0;
  while (p == 0 || disk_id <= p + 1) {
    sprintf(dir_path, "disk_%d", disk_id);
    if (stat(dir_path, &st) == 0 && S_ISDIR(st.st_mode)) { // directory exist
      sprintf(file_path, "disk_%d/%s", disk_id, filename);

      if (stat(file_path, &st) != 0) { // file not exist
        failed_num++;

        if (failed_num == 1) {
          failed[0] = disk_id;
        } else if (failed_num == 2) {
          failed[1] = disk_id;
        } else {
          break;
        }
      } else {
        if (p == 0) { // file exist but p is unknown
          FILE *file;
          file = fopen(file_path, "rb");
          fread(&p, sizeof(int), 1, file);
          fclose(file);
        }

        if (disk_id == p - 1) {
          remain_size = st.st_size;
        } else { // disk_id != p-1
          file_size = st.st_size;
        }
      }
    } else { // directory not exist
      failed_num++;

      if (failed_num == 1) {
        failed[0] = disk_id;
      } else if (failed_num == 2) {
        failed[1] = disk_id;
      } else {
        break;
      }
    }
    disk_id++;
  }

  if (p == 0) {
    printf("File does not exist!\n");
    exit(1);
  }

  if (failed_num > 2) {
    printf("File corrupted!\n");
    exit(1);
  }

  /* 计算 remain_size 真实值 */
  if (remain_size != 0) { // disk_{p-1} is not failed
    remain_size = remain_size - file_size;
  } else { // when disk_{p-1} is failed...
    // try reading disk_{p}/<filename>.remaining
    sprintf(file_path, "disk_%d/%s.remaining", p, filename);
    FILE *f = fopen(file_path, "rb");
    size_t size = 0;
    if (f) {
      fseek(f, 0, SEEK_END);
      remain_size = ftell(f);
      fclose(f);
    } else {
      // perror("remaining file in disk_p is not exist");

      sprintf(file_path, "disk_%d/%s.remaining", p + 1, filename);
      f = fopen(file_path, "rb");
      if (f) {
        fseek(f, 0, SEEK_END);
        remain_size = ftell(f);
        fclose(f);
      }
      // else remain size = 0
    }
  }
  file_size -= sizeof(int);
  block_size = file_size / (p - 1);
  printf("[Info]{%s}: file size = %ld, remain size = %ld, block size = %ld\n",
         __func__, file_size, remain_size, block_size);

  /* start decoding */
  if (failed_num == 0) {
    printf("read directly\n");
    FILE *output = fopen(save_as, "wb");
    if (!output) {
      perror("Error in writing file");
      exit(1);
    }

    char *buffer = (char *)malloc(file_size);
    for (int i = 0; i < p; i++) {
      readDataColumn(filename, i, file_size, buffer);
      fwrite(buffer, file_size, 1, output);
    }
    if (remain_size > 0) {
      readRemain(filename, p - 1, p, remain_size, buffer);
      writeRemain(buffer, remain_size, output);
    }
    fclose(output);
    free(buffer);
  } else if (failed_num == 1) {
    FILE *output = fopen(save_as, "wb");
    if (!output) {
      perror("Error in writing file");
      exit(1);
    }

    if (failed[0] == p || failed[0] == p + 1) { // diagonal_parity or row_parity
      printf("disk_%d failed, read directly\n", failed[0]);

      char *buffer = (char *)malloc(file_size);
      for (int i = 0; i < p; i++) {
        readDataColumn(filename, i, file_size, buffer);
        fwrite(buffer, file_size, 1, output);
      }
      if (remain_size > 0) {
        readRemain(filename, p - 1, p, remain_size, buffer);
        writeRemain(buffer, remain_size, output);
      }
      free(buffer);
    } else { // repair some data column
      printf("disk_%d failed, repair and read\n", failed[0]);

      char *buffer = (char *)malloc(file_size);
      char *missed_column = (char *)malloc(file_size);
      memset(missed_column, 0, file_size);

      int reset_ptr = 0; // false
      for (int i = 0; i < p; i++) {
        if (i != failed[0]) {
          readDataColumn(filename, i, file_size, buffer);
          for (int j = 0; j < p - 1; j++) {
            block_xoreq(missed_column + j * block_size,
                        buffer + j * block_size);
          }
          writeDataColumn(buffer, i, file_size, output, &reset_ptr);
        } else {
          reset_ptr = 1;
        }
      }
      readDataColumn(filename, p, file_size, buffer); // read row parity
      for (int j = 0; j < p - 1; j++) {
        block_xoreq(missed_column + j * block_size, buffer + j * block_size);
      }
      reset_ptr = 1;
      writeDataColumn(missed_column, failed[0], file_size, output, &reset_ptr);

      if (remain_size > 0) {
        readRemain(filename, p, p, remain_size, buffer);
        writeRemain(buffer, remain_size, output);
      }
      free(buffer);
      free(missed_column);
    }
    fclose(output);
  } else { // failed_num == 2
    FILE *output = fopen(save_as, "wb");
    if (!output) {
      perror("Error in writing file");
      exit(1);
    }

    if (failed[1] == p + 1) {
      if (failed[0] == p) { // case 1: diagonal_parity and row_parity
        printf("case 1: disk_%d and disk_%d failed, read directly\n", failed[0],
               failed[1]);
        char *buffer = (char *)malloc(file_size);
        for (int i = 0; i < p; i++) {
          readDataColumn(filename, i, file_size, buffer);
          fwrite(buffer, file_size, 1, output);
        }
        if (remain_size > 0) {
          readRemain(filename, p - 1, p, remain_size, buffer);
          writeRemain(buffer, remain_size, output);
        }
        free(buffer);
      } else { // case 2: diagonal_parity and data column
        printf("case 2: disk_%d and disk_%d failed, repair a data column and "
               "read\n",
               failed[0], failed[1]);
        char *buffer = (char *)malloc(file_size);
        char *missed_column = (char *)malloc(file_size);
        memset(missed_column, 0, file_size);

        int reset_ptr = 0; // false
        for (int i = 0; i < p; i++) {
          if (i != failed[0]) {
            readDataColumn(filename, i, file_size, buffer);
            for (int j = 0; j < p - 1; j++) {
              block_xoreq(missed_column + j * block_size,
                          buffer + j * block_size);
            }
            writeDataColumn(buffer, i, file_size, output, &reset_ptr);
          } else {
            reset_ptr = 1;
          }
        }
        readDataColumn(filename, p, file_size, buffer); // read row parity
        for (int j = 0; j < p - 1; j++) {
          block_xoreq(missed_column + j * block_size, buffer + j * block_size);
        }
        reset_ptr = 1;
        writeDataColumn(missed_column, failed[0], file_size, output,
                        &reset_ptr);

        if (remain_size > 0) {
          readRemain(filename, p, p, remain_size, buffer);
          writeRemain(buffer, remain_size, output);
        }
        free(buffer);
        free(missed_column);
      }
    } else if (failed[1] == p) { // case 3: row parity and data column
      printf(
          "case 3: disk_%d and disk_%d failed, repair a data column and read\n",
          failed[0], failed[1]);

      char *buffer = (char *)malloc(file_size);
      char *missed_column =
          (char *)malloc(file_size + block_size); // length = p
      memset(missed_column, 0, file_size + block_size);

      int reset_ptr = 0; // false
      for (int i = 0; i < p; i++) {
        if (i != failed[0]) {
          readDataColumn(filename, i, file_size, buffer);
          xoreq_diag(missed_column, buffer, failed[0], i, p);
          writeDataColumn(buffer, i, file_size, output, &reset_ptr);
        } else {
          reset_ptr = 1;
        }
      }

      readDataColumn(filename, p + 1, file_size, buffer);
      xoreq_diagparity(missed_column, buffer, failed[0], p);
      char *S = missed_column + (p - 1) * block_size;
      for (int i = 0; i < p - 1; i++) {
        block_xoreq(missed_column + i * block_size, S);
      }

      reset_ptr = 1;
      writeDataColumn(missed_column, failed[0], file_size, output, &reset_ptr);
      if (remain_size > 0) {
        readRemain(filename, p + 1, p, remain_size, buffer);
        writeRemain(buffer, remain_size, output);
      }
      free(buffer);
      free(missed_column);
    } else { // case 4: two data column
      printf("case 4: disk_%d and disk_%d failed, repair two data columns and "
             "read\n",
             failed[0], failed[1]);

      char *buffer = (char *)malloc(file_size);
      char *R = (char *)malloc(file_size +
                               block_size); // 缺失的两列的同一行元素异或值；
      char *D = (char *)malloc(
          file_size + block_size); // 缺失的两列的同一对角线元素异或值；
      readDataColumn(filename, p, file_size, R); // 初始化 R[0:p-1] = row_parity
      readDataColumn(filename, p + 1, file_size,
                     D); // 初始化 D[0:p-1] = diagonal_parity
      memset(R + (p - 1) * block_size, 0, block_size); // R[p-1] = 0
      char *S = D + (p - 1) * block_size; // 闲置空间拿出来放置 S
      memset(S, 0, block_size);           // S = 0

      // calculate S
      for (int k = 0; k < p - 1; k++) {
        block_xoreq(S, R + k * block_size);
        block_xoreq(S, D + k * block_size);
      }

      // D[k] ^= S，此时 D[p-1] 就无需与 S 异或
      for (int k = 0; k < p - 1; k++) {
        block_xoreq(D + k * block_size, S);
      }

      // calculate R and D, and write existing data column
      int reset_ptr = 0; // false
      for (int i = 0; i < p; i++) {
        if (i == failed[0] || i == failed[1]) {
          reset_ptr = 1;
        } else {
          readDataColumn(filename, i, file_size, buffer);
          for (int k = 0; k < p - 1; k++) {
            block_xoreq(R + k * block_size, buffer + k * block_size);
          }
          xoreq_diag(D, buffer, 0, i, p);
          writeDataColumn(buffer, i, file_size, output, &reset_ptr);
        }
      }

      // calculate the two missed data columns
      // 按照计算顺序 missed_1 可以复用 R 的空间
      int m = p - 1 - (failed[1] - failed[0]); //  0 <= m <= p-2
      char *missed_1 = R;
      char *missed_2 = (char *)malloc(file_size);

      do {
        char *D1 = D + (failed[1] + m) % p * block_size;
        char *R1 = R + m * block_size;
        char *cur_1 = missed_1 + m * block_size;
        char *cur_2 = missed_2 + m * block_size;

        // 第一次进入循环时其实执行 D1 ^ R[p-1] =
        // missed_2[m]，因此之前需要初始化 R[p-1] = 0
        block_xor(D1, missed_1 + (m + failed[1] - failed[0]) % p * block_size,
                  cur_2);
        block_xor(R1, cur_2, cur_1);
        m = (p + m - (failed[1] - failed[0])) %
            p; // p 为质数则可以保证p次迭代不重不漏地遍历[0, p-1]的每个整数
      } while (m != p - 1);

      // write two data columns to local file
      reset_ptr = 1;
      writeDataColumn(missed_1, failed[0], file_size, output, &reset_ptr);
      reset_ptr = 1;
      writeDataColumn(missed_2, failed[1], file_size, output, &reset_ptr);

      if (remain_size > 0) {
        readRemain(filename, p + 1, p, remain_size, buffer);
        writeRemain(buffer, remain_size, output);
      }

      // free space
      free(buffer);
      free(R);
      free(D);
      free(missed_2);
    }

    fclose(output);
  }
}
