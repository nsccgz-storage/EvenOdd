#pragma once
#include "util/util.h"
#include <stdio.h>
#include <vector>
enum RC {
  SUCCESS = 1,
  FILE_NOT_EXIST,
  CANNOT_CREAT_FILE,
  WRITE_COMPLETE,
  CANNOT_REPAIR,
  BUFFER_OVERFLOW,
};

void setBufferSize(off_t buffer_size_);
/*
 * caculte the xor value and save to lhs
 */
void symbolXor(char *lhs, const char *rhs, off_t symbol_size);

void symbolXor(const char *lhs, const char *rhs, char *dst, off_t symbol_size);

void caculateXor(char *pre_row_parity, char *pre_diag, char *cur_data,
                 off_t symbol_size, int p, int col_idx);

int write_remaining_file(const char *filename, int p, int i, char *buffer,
                         off_t last_size);

int write_col_file(const char *filename, int p, int i, char *buffer,
                   off_t write_size, bool flag);

/*
 * Please encode the input file with EVENODD code
 * and store the erasure-coded splits into corresponding disks
 * For example: Suppose "file_name" is "testfile", and "p" is 5. After your
 * encoding logic, there should be 7 splits, "testfile_0", "testfile_1",
 * ..., "testfile_6", stored in 7 diffrent disk folders from "disk_0" to
 * "disk_6".
 */
RC encode(const char *path, int p);

/*
 * just read from 0 ~ p-1 to recover file
 */
RC basicRead(const char *path, const char *save_as);

RC seqXor(const char *path, std::vector<int> &xor_idxs, int p, char *col_buffer,
          char *diag_buffer, int symbol_size);
/*
 * failIdx is increasing
 */
RC repairSingleFile(const char *filename, int *fail_idxs, int num, int p);