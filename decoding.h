#pragma once
#include "encoding.h"
#include "util.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void readDataColumn(char *filename, int disk_id, int file_id, size_t file_size, char *result);

size_t writeDataColumn(char *src, int disk_id, size_t offset, size_t size, FILE *stream,
                       bool *reset_ptr);

size_t writeRemain(char *src, size_t offset, size_t size, FILE *stream);

void readRemain(char *filename, int disk_id, int file_id, int p, size_t remain_size, char *result);

void block_xor(char *left, char *right, char *result, size_t block_size);

void block_xoreq(char *left, char *right, size_t block_size);

/*
 * 两个数据列`同一对角线元素`进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diag(char *left, char *right, int left_id, int right_id, int p, size_t block_size);

/*
 * left 和 diagonal parity 对应元素进行异或操作，结果保留在 left 中
 * 注意：left 的长度为 p*block_size
 */
void xoreq_diagparity(char *left, char *diag, int left_id, int p, size_t block_size);

void decode(int p, int failed_num, int* failed, char* filename, char* save_as, 
  size_t file_size, size_t remain_size, int file_id, FILE* output, size_t* write_file_offset);

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
void read1(char *filename, char *save_as);