#include "decoding.h"
#include "util/util.h"
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

#include "repair.h"

int getMinValidDisk(int num_erasures, int *disks) {
  if (num_erasures == 1) {
    return disks[0] == 0 ? 1 : 0;
  }
  // else num erasures == 2
  if (disks[0] == 0) {
    return disks[1] == 1 ? 2 : 1;
  } else
    return 0;
}

bool parseFileName(const char *filename, char *result) {
  size_t len = strlen(filename);
  if (len < 3)
    return false;

  if (filename[len - 1] == '0' && filename[len - 2] == '.') {
    memcpy(result, filename, len - 2);
    result[len - 2] = '\0';
    return true;
  }
  return false;
}

void getSize(const string &file, int p, int min_valid_disk, int file_per_disk,
             int num_erasures, int *disks, size_t &file_size,
             size_t &remain_size, size_t &last_file_size,
             size_t &last_remain_size) {

  char file_name[PATH_MAX_LEN];
  struct stat st;

  sprintf(file_name, "disk_%d/%s.0", min_valid_disk, file.c_str());
  stat(file_name, &st);
  file_size = st.st_size - sizeof(int);

  sprintf(file_name, "disk_%d/%s.%d", min_valid_disk, file.c_str(),
          file_per_disk - 1);
  stat(file_name, &st);
  last_file_size = st.st_size - sizeof(int);

  if (min_valid_disk != p - 1) {
    int d;
    if (num_erasures == 1) {
      d = disks[0] == p ? p + 1 : p;
    } else
      d = disks[1] == p + 1 ? (disks[0] == p ? p - 1 : p) : p + 1;

    if (d == p - 1) {
      sprintf(file_name, "disk_%d/%s.%d", d, file.c_str(), 0);
      if (stat(file_name, &st) == 0) {
        remain_size = st.st_size - file_size - sizeof(int);
      } else {
        LOG_ERROR("get size error");
      }

      sprintf(file_name, "disk_%d/%s.%d", d, file.c_str(), file_per_disk - 1);
      if (stat(file_name, &st) == 0) {
        last_remain_size = st.st_size - last_file_size - sizeof(int);
      } else {
        LOG_ERROR("get size error");
      }
    } else {
      sprintf(file_name, "disk_%d/%s.%d.remaining", d, file.c_str(), 0);
      if (stat(file_name, &st) == 0) {
        remain_size = st.st_size;
      } else { // 没有 remaining 文件说明 remain_size = 0
        remain_size = 0;
      }

      sprintf(file_name, "disk_%d/%s.%d.remaining", d, file.c_str(),
              file_per_disk - 1);
      if (stat(file_name, &st) == 0) {
        last_remain_size = st.st_size;
      } else {
        last_remain_size = 0;
      }
    }
  } else { // p = 3, and failed[0] = 0, failed[1] = 1
    sprintf(file_name, "disk_%d/%s.%d.remaining", p, file.c_str(), 0);
    if (stat(file_name, &st) == 0) {
      remain_size = st.st_size;
      file_size -= st.st_size;
    } else {
      remain_size = 0;
    }

    sprintf(file_name, "disk_%d/%s.%d.remaining", p, file.c_str(),
            file_per_disk - 1);
    if (stat(file_name, &st) == 0) {
      last_remain_size = st.st_size;
      last_file_size -= st.st_size;
    } else {
      last_remain_size = 0;
    }
  }
}

void setFailedDisk(int p, int failed_num, int* disks, int* ret_failed_num, int* ret_disks){
  // 0 - {p+1}
  if(failed_num == 1){
    if(disks[0] > p+1){
      *ret_failed_num = 0;
    }else{
      *ret_failed_num = 1;
      ret_disks[0] = disks[0];
    }
  }
  else if(failed_num == 2){
    if(disks[0] > p+1){
      *ret_failed_num = 0;
    }else if(disks[1] > p+1){
      *ret_failed_num = 1;
      ret_disks[0] = disks[0];
    }else{
      *ret_failed_num = 2;
      ret_disks[0] = disks[0];
      ret_disks[1] = disks[1];
    }
  }
}

void repairToDisk(const char *filename, char *buffer, size_t size, int disk_id,
                  int file_id, int p, char *remain_buffer, size_t remain_size) {
  char output_name[PATH_MAX_LEN];
  sprintf(output_name, "disk_%d/%s.%d", disk_id, filename, file_id);
  int fd = open(output_name, O_CREAT | O_WRONLY, S_IRWXU);
  write(fd, &p, sizeof(int));
  write(fd, buffer, size);
  if (disk_id == p - 1) {
    write(fd, remain_buffer, remain_size);
  }
  close(fd);

  if (disk_id == p || disk_id == p + 1) {
    sprintf(output_name, "disk_%d/%s.%d.remaining", disk_id, filename, file_id);
    fd = open(output_name, O_CREAT | O_WRONLY, S_IRWXU);
    write(fd, remain_buffer, remain_size);
    close(fd);
  }
}

void repairFile(const string &filename, int num, int *disks,
                int p, int file_id, size_t file_size, size_t remain_size) {
  size_t block_size = file_size / (p - 1);
  
  int failed_num;
  int failed_disks[2];
  setFailedDisk(p, num, disks, &failed_num, failed_disks);
  printf("filename: %s, p: %d, failed_num: %d, failed = %d, %d", filename.c_str(), p, failed_num, failed_disks[0], failed_disks[1]);

  if(failed_num == 0)return ;
  if (failed_num == 1) {
    if (failed_disks[0] == p) {
      /* encode row parity */
      char *buffer = new char[file_size];
      char *row_parity = new char[file_size + block_size];

      encodeRowDiagonalParity(filename.c_str(), buffer, p, file_id, file_size,
                              true, row_parity);
      char *remain_buffer = new char[remain_size];
      readRemain(filename.c_str(), p - 1, file_id, p, remain_size,
                 remain_buffer);
      repairToDisk(filename.c_str(), row_parity, file_size, p, file_id, p,
                   remain_buffer, remain_size);

      delete[] remain_buffer;
      delete[] buffer;
      delete[] row_parity;
    } else if (failed_disks[0] == p + 1) {
      /* encode diagonal parity */
      char *buffer = new char[file_size];
      char *diagonal_parity = new char[file_size + block_size];

      encodeRowDiagonalParity(filename.c_str(), buffer, p, file_id, file_size,
                              false, nullptr, true, diagonal_parity);
      char *remain_buffer = new char[remain_size];
      readRemain(filename.c_str(), p - 1, file_id, p, remain_size,
                 remain_buffer);
      repairToDisk(filename.c_str(), diagonal_parity, file_size, p + 1, file_id,
                   p, remain_buffer, remain_size);

      delete[] remain_buffer;
      delete[] buffer;
      delete[] diagonal_parity;
    } else {
      /* repair by row parity */
      char *buffer = new char[file_size];
      char *missed_column = new char[file_size + block_size]; // length = p

      repairMixed(filename.c_str(), failed_disks, buffer, missed_column, p,
                  file_id, file_size);

      // repairByRowParity(filename.c_str(), failed_disks, buffer,
      // missed_column, p, file_id, file_size);

      if (failed_disks[0] == p - 1) {
        char *remain_buffer = new char[remain_size];
        readRemain(filename.c_str(), p, file_id, p, remain_size, remain_buffer);
        repairToDisk(filename.c_str(), missed_column, file_size,
                     failed_disks[0], file_id, p, remain_buffer, remain_size);
        delete[] remain_buffer;
      } else {
        repairToDisk(filename.c_str(), missed_column, file_size,
                     failed_disks[0], file_id, p);
      }

      delete[] buffer;
      delete[] missed_column;
    }
  } else { // failed_num == 2
    if (failed_disks[1] == p + 1) {
      if (failed_disks[0] == p) {
        /* encode row and diagonal parity */
        char *buffer = new char[file_size];
        char *row_parity = new char[file_size + block_size];
        char *diagonal_parity = new char[file_size + block_size];

        encodeRowDiagonalParity(filename.c_str(), buffer, p, file_id, file_size,
                                true, row_parity, true, diagonal_parity);
        char *remain_buffer = new char[remain_size];
        readRemain(filename.c_str(), p - 1, file_id, p, remain_size,
                   remain_buffer);
        repairToDisk(filename.c_str(), row_parity, file_size, p, file_id, p,
                     remain_buffer, remain_size);
        repairToDisk(filename.c_str(), diagonal_parity, file_size, p + 1,
                     file_id, p, remain_buffer, remain_size);

        delete[] remain_buffer;
        delete[] buffer;
        delete[] row_parity;
        delete[] diagonal_parity;
      } else {
        /* repair by row parity and enocde diagonal parity */
        char *buffer = new char[file_size];
        char *missed_column = new char[file_size + block_size]; // length = p

        char *diagonal_parity = new char[file_size + block_size];

        repairByRowParity(filename.c_str(), failed_disks, buffer, missed_column,
                          p, file_id, file_size, true, diagonal_parity);

        char *remain_buffer = new char[remain_size];
        readRemain(filename.c_str(), p, file_id, p, remain_size, remain_buffer);

        if (failed_disks[0] == p - 1) {
          repairToDisk(filename.c_str(), missed_column, file_size,
                       failed_disks[0], file_id, p, remain_buffer, remain_size);
        } else {
          repairToDisk(filename.c_str(), missed_column, file_size,
                       failed_disks[0], file_id, p);
        }
        repairToDisk(filename.c_str(), diagonal_parity, file_size, p + 1,
                     file_id, p, remain_buffer, remain_size);
        delete[] remain_buffer;

        delete[] buffer;
        delete[] missed_column;
        delete[] diagonal_parity;
      }
    } else if (failed_disks[1] == p) {
      /* repair by diagonal parity and enocde row parity */
      char *buffer = new char[file_size];
      char *missed_column = new char[file_size + block_size]; // length = p

      char *row_parity = new char[file_size + block_size];

      repairByDiagonalParity(filename.c_str(), failed_disks, buffer,
                             missed_column, p, file_id, file_size, true,
                             row_parity);

      char *remain_buffer = new char[remain_size]; // <= p * (p-1)
      readRemain(filename.c_str(), p + 1, file_id, p, remain_size,
                 remain_buffer);

      if (failed_disks[0] == p - 1) {
        repairToDisk(filename.c_str(), missed_column, file_size,
                     failed_disks[0], file_id, p, remain_buffer, remain_size);
      } else {
        repairToDisk(filename.c_str(), missed_column, file_size,
                     failed_disks[0], file_id, p);
      }
      repairToDisk(filename.c_str(), row_parity, file_size, p, file_id, p,
                   remain_buffer, remain_size);
      delete[] remain_buffer;

      delete[] buffer;
      delete[] missed_column;
      delete[] row_parity;

    } else {
      /* repair by row and diagonal parity */
      char *buffer = new char[file_size];
      char *R =
          new char[file_size + block_size]; // 缺失的两列的同一行元素异或值；
      char *D = new char[file_size +
                         block_size]; // 缺失的两列的同一对角线元素异或值；
      readDataColumn(filename.c_str(), p, file_id, file_size,
                     R); // 初始化 R[0:p-1] = row_parity
      readDataColumn(filename.c_str(), p + 1, file_id, file_size,
                     D); // 初始化 D[0:p-1] = diagonal_parity

      char *missed_1, *missed_2;
      repairByRowDiagonalParity(filename.c_str(), failed_disks, buffer, R, D, p,
                                file_id, file_size, &missed_1, &missed_2);

      repairToDisk(filename.c_str(), missed_1, file_size, failed_disks[0],
                   file_id, p);
      if (failed_disks[1] == p - 1) {
        char *remain_buffer = new char[remain_size]; // <= p * (p-1)
        readRemain(filename.c_str(), p, file_id, p, remain_size, remain_buffer);
        repairToDisk(filename.c_str(), missed_2, file_size, failed_disks[1],
                     file_id, p, remain_buffer, remain_size);
        delete[] remain_buffer;
      } else
        repairToDisk(filename.c_str(), missed_2, file_size, failed_disks[1],
                     file_id, p);

      delete[] buffer;
      delete[] R;
      delete[] D;
      delete[] missed_2;
    }
  }
}

void repair(int num_erasures, int *disks) {
  if (num_erasures == 0)
    return;

  // ensure disks[0] < disks[1]
  if (num_erasures == 2 && disks[0] > disks[1]) {
    int tmp = disks[0];
    disks[0] = disks[1];
    disks[1] = tmp;
  }

  // 能保证所有编码文件在该磁盘下出现
  int min_valid_disk = getMinValidDisk(num_erasures, disks);

  char disk_name[PATH_MAX_LEN];
  sprintf(disk_name, "disk_%d", min_valid_disk);
  struct dirent *entry = nullptr;
  DIR *dir_ptr = opendir(disk_name);
  if (dir_ptr == nullptr) {
    LOG_ERROR("open disk failed");
    exit(-1);
  }
  std::vector<std::string> files;
  char file_name[PATH_MAX_LEN];
  while ((entry = readdir(dir_ptr))) {
    if (entry->d_name[0] == '.')
      continue; // skip . and .. ; require: 测试文件命名不以 . 开头
    if (parseFileName(entry->d_name, file_name)) {
      files.emplace_back(string(file_name));
    }
  }
  closedir(dir_ptr);

  struct stat st;
  // mkdir if directory is not exist
  sprintf(disk_name, "disk_%d", disks[0]);
  if (stat(disk_name, &st) == -1) {
    mkdir(disk_name, 0700);
  }
  sprintf(disk_name, "disk_%d", disks[1]);
  if (num_erasures == 2 && stat(disk_name, &st) == -1) {
    mkdir(disk_name, 0700);
  }

  /* process all the file in disk  */
  for (string &file : files) {
    int p;
    sprintf(file_name, "disk_%d/%s.0", min_valid_disk, file.c_str());
    int fd = open(file_name, O_RDONLY);
    read(fd, &p, sizeof(int));
    close(fd);

    int file_per_disk = 0;
    do {
      file_per_disk++;
      sprintf(file_name, "disk_%d/%s.%d", min_valid_disk, file.c_str(),
              file_per_disk);
    } while (stat(file_name, &st) == 0);

    size_t file_size, remain_size, last_file_size, last_remain_size;
    getSize(file, p, min_valid_disk, file_per_disk, num_erasures, disks,
            file_size, remain_size, last_file_size, last_remain_size);
    // printf("file_size = %ld, remain size = %ld, last file size = %ld, last "
    //        "remain size = %ld\n",
    //        file_size, remain_size, last_file_size, last_remain_size);

    for (int i = 0; i < file_per_disk; i++) {
      // TODO: last i should last_file_size and last_remain_size
      if (i != file_per_disk - 1)
        repairFile(file, num_erasures, disks, p, i, file_size, remain_size);
      else
        repairFile(file, num_erasures, disks, p, i, last_file_size,
                   last_remain_size);
    }
  }
}
