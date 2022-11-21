#include <cstring>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("usage: ./gendate <file_size>\n");
    return -1;
  }
  srand(time(0));

  std::string file_name = "data_" + std::string(argv[1]) + "G";
  ofstream outF(file_name, ios::out | ios::binary);

  const int buf_size = 1 * 1024 * 1024; // 1MiB
  char buffer[buf_size];
  for (int i = 0; i < buf_size; i++) {
    buffer[i] = char(rand() % 64);
  }
  // g
  int size_g = atoi(argv[1]);
  for (int i = 0; i < size_g; i++) {
    for (int i = 0; i < 1024; i++) {
      outF.write(buffer, buf_size);
    }
  }

  outF.close();
  return 0;
}