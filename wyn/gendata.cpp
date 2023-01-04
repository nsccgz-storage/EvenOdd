#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdlib.h>
using namespace std;

int main(int argc, char **argv){
	srand(time(0));
    ofstream outF("data_50MB", ios::out | ios::binary);
    long long data;
    long long size = (long long)50 * 1024 * 1024 / 8;
    for(long long i = 0; i < size; i++)
    {
        long long data = (long long) rand();
        outF.write(reinterpret_cast<char*>(&data), sizeof(data));
    }
    
    outF.close();
	return 0;
}