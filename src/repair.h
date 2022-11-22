#pragma once

#include <sys/types.h>
#include <string>

using std::string;

int getMinValidDisk(int num_erasures, int* disks);

bool parseFileName(const char* filename, char* result);

void getSize(const string& file, int p, int min_valid_disk, int file_per_disk, int num_erasures, int* disks, 
    size_t& file_size, size_t& remain_size, size_t& last_file_size, size_t& last_remain_size);

void repairToDisk(const char* filename, char* buffer, size_t size, int disk_id, int file_id, int p, 
                  char* remain_buffer=nullptr, size_t remain_size=0);

void repairFile(const string& filename, int failed_num, int* failed_disks, int p, int file_id, size_t file_size, size_t remain_size);

/*
 * Please repair failed disks. The number of failures is specified by
 * "num_erasures", and the index of disks are provided in the command
 * line parameters.
 * For example: Suppose "number_erasures" is 2, and the indices of
 * failed disks are "0" and "1". After the repair operation, the data
 * splits in folder "disk_0" and "disk_1" should be repaired.
 */
void repair(int num_erasures, int* disks);