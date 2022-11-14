#include "decoding.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef PATH_MAX_LEN
#define PATH_MAX_LEN 512
#endif

void readDataColumn(char *filename, int disk_id, int file_id, size_t file_size, char *result) {
  FILE *input;
  char file_path[PATH_MAX_LEN];
  sprintf(file_path, "disk_%d/%s.%d", disk_id, filename, file_id);
  input = fopen(file_path, "rb");
  fseek(input, sizeof(int), SEEK_SET);
  fread(result, file_size, 1, input);
  fclose(input);
}

size_t writeDataColumn(char *src, int disk_id, size_t offset, size_t size, FILE *stream,
                       bool *reset_ptr) {
  if (*reset_ptr) {
    fseek(stream, offset + size * disk_id, SEEK_SET); // 频繁 fseek 会带来很大开销吗？
    *reset_ptr = false;
  }
  return fwrite(src, 1, size, stream);
}

/* require: write to file end */
size_t writeRemain(char *src, size_t offset, size_t size, FILE *stream) {
  // fseek(stream, 0, SEEK_END);
  fseek(stream, offset, SEEK_SET);
  return fwrite(src, 1, size, stream);
}

void readRemain(char *filename, int disk_id, int file_id, int p, size_t remain_size, char *result) {
  FILE *input;
  char file_path[PATH_MAX_LEN];
  if (disk_id == p - 1) {
    sprintf(file_path, "disk_%d/%s.%d", disk_id, filename, file_id);
    input = fopen(file_path, "rb");
    fseek(input, -remain_size, SEEK_END); // 倒数 remain_size
    fread(result, remain_size, 1, input);
  } else if (disk_id == p || disk_id == p + 1) {
    sprintf(file_path, "disk_%d/%s.%d.remaining", disk_id, filename, file_id);
    input = fopen(file_path, "rb");
    fread(result, remain_size, 1, input);
  } else {
    LOG_ERROR("invalidly reading of remain block");
  }
  fclose(input);
}

void block_xor(char *left, char *right, char *result, size_t block_size) {
  for (int i = 0; i < block_size; i++) {
    result[i] = left[i] ^ right[i];
  }
}

void block_xoreq(char *left, char *right, size_t block_size) { 
  block_xor(left, right, left, block_size); 
}

/*
 * 两个数据列`同一对角线元素`进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diag(char *left, char *right, int left_id, int right_id, int p, size_t block_size) {
  for (int i = 0; i < p - 1; i++) {
    int x = (right_id + i - left_id + p) % p;
    block_xoreq(left + x * block_size, right + i * block_size, block_size);
  }
}

/*
 * left 和 diagonal parity 对应元素进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diagparity(char *left, char *diag, int left_id, int p, size_t block_size) {
  xoreq_diag(left, diag, left_id, 0, p, block_size);
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
  size_t file_size = 0;
  size_t last_file_size = 0;
  size_t remain_size = 0;
  size_t last_remain_size = 0;
  int p = 0;
  int failed_num = 0;
  int failed[2];
  char file_path[PATH_MAX_LEN];
  char dir_path[PATH_MAX_LEN];

  char *filename = basename(path);

  int disk_id = 0;
  int file_per_disk = 0;
  while(p == 0 || disk_id <= p+1){
    int file_id = 0;
    sprintf(dir_path, "disk_%d", disk_id);
    if (stat(dir_path, &st) == 0 && S_ISDIR(st.st_mode)) { // directory exist
      sprintf(file_path, "disk_%d/%s.%d", disk_id, filename, file_id);
      
      if(stat(file_path, &st) != 0){ // file not exist
        failed_num++;
        if (failed_num == 1) {
          failed[0] = disk_id;
        } else if (failed_num == 2) {
          failed[1] = disk_id;
        } else {
          break;
        }
      } else {
        // file exist but p is unknown
        if (p == 0) { 
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

        // determine file num per disk
        if(file_per_disk == 0){ 
          do{
            file_per_disk++;
            sprintf(file_path, "disk_%d/%s.%d", disk_id, filename, file_per_disk);
            last_file_size = st.st_size;
          }while(stat(file_path, &st) == 0);
        }
        sprintf(file_path, "disk_%d/%s.%d", disk_id, filename, file_per_disk-1);
        stat(file_path, &st);
        if (disk_id == p - 1) {
          last_remain_size = st.st_size;
        } else { 
          last_file_size = st.st_size;
        }
      }
    }
    else{ // directory not exist
      failed_num++;

      if (failed_num == 1) {
        failed[0] = disk_id;
      } else if (failed_num == 2) {
        failed[1] = disk_id;
      } else { // failed_num > 2
        break;
      }
    }
    disk_id++;
  }

  if (p == 0 || failed_num > 2) {
    LOG_INFO("File corrupted!\n");
    exit(1);
  }

  /* 计算 remain_size 真实值 */
  if (remain_size != 0) { // disk_{p-1} is not failed
    remain_size = remain_size - file_size;
  } else { // when disk_{p-1} is failed...
    // try reading disk_{p}/<filename>.remaining
    sprintf(file_path, "disk_%d/%s.%d.remaining", p, filename, 0);
    FILE *f = fopen(file_path, "rb");
    size_t size = 0;
    if (f) {
      fseek(f, 0, SEEK_END);
      remain_size = ftell(f);
      fclose(f);
    } else {
      // perror("remaining file in disk_p is not exist");
      sprintf(file_path, "disk_%d/%s.%d.remaining", p + 1, filename, 0);
      f = fopen(file_path, "rb");
      if (f) {
        fseek(f, 0, SEEK_END);
        remain_size = ftell(f);
        fclose(f);
      }
      // else remain size = 0
    }
  }

  /* 计算 last_remain_size 真实值 */
  if (last_remain_size != 0) { // disk_{p-1} is not failed
    last_remain_size = last_remain_size - last_file_size;
  } else { // when disk_{p-1} is failed...
    sprintf(file_path, "disk_%d/%s.%d.remaining", p, filename, file_per_disk-1);
    FILE *f = fopen(file_path, "rb");
    size_t size = 0;
    if (f) {
      fseek(f, 0, SEEK_END);
      last_remain_size = ftell(f);
      fclose(f);
    } else {
      sprintf(file_path, "disk_%d/%s.%d.remaining", p + 1, filename, file_per_disk-1);
      f = fopen(file_path, "rb");
      if (f) {
        fseek(f, 0, SEEK_END);
        last_remain_size = ftell(f);
        fclose(f);
      }
    }
  }

  file_size -= sizeof(int);
  last_file_size -= sizeof(int);
  LOG_INFO("file size = %ld, remain size = %ld, file_per_disk = %d, "
    "last_file_size = %ld, last_remain_size = %ld",
    file_size, remain_size, file_per_disk, last_file_size, last_remain_size);

  /* start decoding */
  FILE *output_stream = fopen(save_as, "wb");
  if (!output_stream) {
    perror("Error in writing file");
    exit(1);
  }

  size_t write_file_offset = 0;
  for(int file_id = 0;file_id < file_per_disk;file_id++){
    fseek(output_stream, write_file_offset, SEEK_SET);
    if(file_id != file_per_disk-1){
      decode(p, failed_num, failed, filename, save_as, file_size, remain_size, file_id, output_stream, &write_file_offset);
    }
    else{
      decode(p, failed_num, failed, filename, save_as, last_file_size, last_remain_size, file_id, output_stream, &write_file_offset);
    }
  }
  fclose(output_stream);
}



void decode(int p, int failed_num, int* failed, char* filename, char* save_as, 
  size_t file_size, size_t remain_size, int file_id, FILE* output_stream, size_t* write_file_offset){
  
  size_t offset = *write_file_offset;
  size_t block_size = file_size / (p-1);
  if (failed_num == 0) {
    LOG_DEBUG("read directly\n");
    bool reset_ptr = false;

    char *buffer = (char *)malloc(file_size);
    for (int i = 0; i < p; i++) {
      readDataColumn(filename, i, file_id, file_size, buffer);
      writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
    }
    if (remain_size > 0) {
      if(remain_size <= file_size){
        readRemain(filename, p-1, file_id, p, remain_size, buffer);
        writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
      }else{
        char* remain_buffer = (char*) malloc(remain_size);
        readRemain(filename, p-1, file_id, p, remain_size, remain_buffer);
        writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
        free(remain_buffer);
      }
    }

    free(buffer);
  } else if (failed_num == 1) {
    if (failed[0] == p || failed[0] == p + 1) { // diagonal_parity or row_parity
      LOG_DEBUG("disk_%d failed, read directly\n", failed[0]);
      bool reset_ptr = false;

      char *buffer = (char *)malloc(file_size);
      for (int i = 0; i < p; i++) {
        readDataColumn(filename, i, file_id, file_size, buffer);
        writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
      }
      if (remain_size > 0) {
        if(remain_size <= file_size){
          readRemain(filename, p-1, file_id, p, remain_size, buffer);
          writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
        }else{
          char* remain_buffer = (char*) malloc(remain_size);
          readRemain(filename, p-1, file_id, p, remain_size, remain_buffer);
          writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
          free(remain_buffer);
        }
      }
      free(buffer);
    } else { // repair some data column
      LOG_DEBUG("disk_%d failed, repair and read\n", failed[0]);

      char *buffer = (char *)malloc(file_size);
      char *missed_column = (char *)malloc(file_size);
      memset(missed_column, 0, file_size);

      bool reset_ptr = false;
      for (int i = 0; i < p; i++) {
        if (i != failed[0]) {
          readDataColumn(filename, i, file_id, file_size, buffer);
          for (int j = 0; j < p - 1; j++) {
            block_xoreq(missed_column + j * block_size, buffer + j * block_size, block_size);
          }
          writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
        } else {
          reset_ptr = true;
        }
      }
      readDataColumn(filename, p, file_id, file_size, buffer); // read row parity
      for (int j = 0; j < p - 1; j++) {
        block_xoreq(missed_column + j * block_size, buffer + j * block_size, block_size);
      }
      reset_ptr = true;
      writeDataColumn(missed_column, failed[0], offset, file_size, output_stream, &reset_ptr);

      if (remain_size > 0) {
        if(remain_size <= file_size){
          readRemain(filename, p, file_id, p, remain_size, buffer);
          writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
        }else{
          char* remain_buffer = (char*) malloc(remain_size);
          readRemain(filename, p, file_id, p, remain_size, remain_buffer);
          writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
          free(remain_buffer);
        }
      }
      free(buffer);
      free(missed_column);
    }
  } else { // failed_num == 2
    if (failed[1] == p + 1) {
      if (failed[0] == p) { // case 1: diagonal_parity and row_parity
        LOG_DEBUG("case 1: disk_%d and disk_%d failed, read directly\n", failed[0],
               failed[1]);
        bool reset_ptr = false;
        char *buffer = (char *)malloc(file_size);
        for (int i = 0; i < p; i++) {
          readDataColumn(filename, i, file_id, file_size, buffer);
          writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
        }
        if (remain_size > 0) {
          if(remain_size <= file_size){
            readRemain(filename, p-1, file_id, p, remain_size, buffer);
            writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
          }else{
            char* remain_buffer = (char*) malloc(remain_size);
            readRemain(filename, p-1, file_id, p, remain_size, remain_buffer);
            writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
            free(remain_buffer);
          }
        }
        free(buffer);
      } else { // case 2: diagonal_parity and data column
        LOG_DEBUG("case 2: disk_%d and disk_%d failed, repair a data column and "
               "read\n",
               failed[0], failed[1]);
        char *buffer = (char *)malloc(file_size);
        char *missed_column = (char *)malloc(file_size);
        memset(missed_column, 0, file_size);

        bool reset_ptr = false;
        for (int i = 0; i < p; i++) {
          if (i != failed[0]) {
            readDataColumn(filename, i, file_id, file_size, buffer);
            for (int j = 0; j < p - 1; j++) {
              block_xoreq(missed_column + j * block_size, buffer + j * block_size, block_size);
            }
            writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
          } else {
            reset_ptr = true;
          }
        }
        readDataColumn(filename, p, file_id, file_size, buffer); // read row parity
        for (int j = 0; j < p - 1; j++) {
          block_xoreq(missed_column + j * block_size, buffer + j * block_size, block_size);
        }
        reset_ptr = true;
        writeDataColumn(missed_column, failed[0], offset, file_size, output_stream, &reset_ptr);

        if (remain_size > 0) {
          if(remain_size <= file_size){
            readRemain(filename, p, file_id, p, remain_size, buffer);
            writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
          }else{
            char* remain_buffer = (char*) malloc(remain_size);
            readRemain(filename, p, file_id, p, remain_size, remain_buffer);
            writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
            free(remain_buffer);
          }
        }
        free(buffer);
        free(missed_column);
      }
    } else if (failed[1] == p) { // case 3: row parity and data column
      LOG_DEBUG("case 3: disk_%d and disk_%d failed, repair a data column and read\n", failed[0], failed[1]);

      char *buffer = (char *)malloc(file_size);
      char *missed_column =
          (char *)malloc(file_size + block_size); // length = p
      memset(missed_column, 0, file_size + block_size);

      bool reset_ptr = false;
      for (int i = 0; i < p; i++) {
        if (i != failed[0]) {
          readDataColumn(filename, i, file_id, file_size, buffer);
          xoreq_diag(missed_column, buffer, failed[0], i, p, block_size);
          writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
        } else {
          reset_ptr = true;
        }
      }

      readDataColumn(filename, p + 1, file_id, file_size, buffer);
      xoreq_diagparity(missed_column, buffer, failed[0], p, block_size);
      char *S = missed_column + (p - 1) * block_size;
      for (int i = 0; i < p - 1; i++) {
        block_xoreq(missed_column + i * block_size, S, block_size);
      }

      reset_ptr = true;
      writeDataColumn(missed_column, failed[0], offset, file_size, output_stream, &reset_ptr);
      if (remain_size > 0) {
        if(remain_size <= file_size){
          readRemain(filename, p+1, file_id, p, remain_size, buffer);
          writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
        }else{
          char* remain_buffer = (char*) malloc(remain_size);
          readRemain(filename, p+1, file_id, p, remain_size, remain_buffer);
          writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
          free(remain_buffer);
        }
      }
      free(buffer);
      free(missed_column);
    } else { // case 4: two data column
      LOG_DEBUG("case 4: disk_%d and disk_%d failed, repair two data columns and read\n", failed[0], failed[1]);

      char *buffer = (char *)malloc(file_size);
      char *R = (char *)malloc(file_size +
                               block_size); // 缺失的两列的同一行元素异或值；
      char *D = (char *)malloc(
          file_size + block_size); // 缺失的两列的同一对角线元素异或值；
      readDataColumn(filename, p, file_id, file_size, R); // 初始化 R[0:p-1] = row_parity
      readDataColumn(filename, p + 1, file_id, file_size,
                     D); // 初始化 D[0:p-1] = diagonal_parity
      memset(R + (p - 1) * block_size, 0, block_size); // R[p-1] = 0
      char *S = D + (p - 1) * block_size; // 闲置空间拿出来放置 S
      memset(S, 0, block_size);           // S = 0

      // calculate S
      for (int k = 0; k < p - 1; k++) {
        block_xoreq(S, R + k * block_size, block_size);
        block_xoreq(S, D + k * block_size, block_size);
      }

      // D[k] ^= S，此时 D[p-1] 就无需与 S 异或
      for (int k = 0; k < p - 1; k++) {
        block_xoreq(D + k * block_size, S, block_size);
      }

      // calculate R and D, and write existing data column
      bool reset_ptr = false;
      for (int i = 0; i < p; i++) {
        if (i == failed[0] || i == failed[1]) {
          reset_ptr = true;
        } else {
          readDataColumn(filename, i, file_id, file_size, buffer);
          for (int k = 0; k < p - 1; k++) {
            block_xoreq(R + k * block_size, buffer + k * block_size, block_size);
          }
          xoreq_diag(D, buffer, 0, i, p, block_size);
          writeDataColumn(buffer, i, offset, file_size, output_stream, &reset_ptr);
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
        block_xor(D1, missed_1 + (m + failed[1] - failed[0]) % p * block_size, cur_2, block_size);
        block_xor(R1, cur_2, cur_1, block_size);
        m = (p + m - (failed[1] - failed[0])) %
            p; // p 为质数则可以保证p次迭代不重不漏地遍历[0, p-1]的每个整数
      } while (m != p - 1);

      // write two data columns to local file
      reset_ptr = true;
      writeDataColumn(missed_1, failed[0], offset, file_size, output_stream, &reset_ptr);
      reset_ptr = true;
      writeDataColumn(missed_2, failed[1], offset, file_size, output_stream, &reset_ptr);

      if (remain_size > 0) {
        if(remain_size <= file_size){
          readRemain(filename, p+1, file_id, p, remain_size, buffer);
          writeRemain(buffer, offset+file_size*p, remain_size, output_stream);
        }else{
          char* remain_buffer = (char*) malloc(remain_size);
          readRemain(filename, p+1, file_id, p, remain_size, remain_buffer);
          writeRemain(remain_buffer, offset+file_size*p, remain_size, output_stream);
          free(remain_buffer);
        }
      }

      // free space
      free(buffer);
      free(R);
      free(D);
      free(missed_2);
    }
  }

  *write_file_offset += file_size * p + remain_size;
}
