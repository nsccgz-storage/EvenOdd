#include <cstdlib>
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
    buffer[rand() % buf_size] = char(rand() % 64);
    for (int j = 0; j < 1024; j++) {
      buffer[(i * j) % buf_size] = char(rand() % 64);
      buffer[rand() % buf_size] = char(rand() % 64);
      buffer[rand() % (buf_size - 8)] = char(rand() % 64);
      outF.write(buffer, buf_size);
    }
    buffer[rand() % buf_size] = char(rand() % 64);
  }
  outF.close();
  return 0;
}