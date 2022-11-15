#pragma once

/*
 * Please repair failed disks. The number of failures is specified by
 * "num_erasures", and the index of disks are provided in the command
 * line parameters.
 * For example: Suppose "number_erasures" is 2, and the indices of
 * failed disks are "0" and "1". After the repair operation, the data
 * splits in folder "disk_0" and "disk_1" should be repaired.
 */
void repair(int num_erasures, int* disks);