#include <fstream>
#include <iostream>
#include <string>
using namespace std;
 
 

 
int main() {

  ifstream ifs;
  ifs.open("read_time.txt") ;
  if (!ifs.is_open()) {
    cout << "文件打开失败" << endl;
    return 0;
  }

  double read_time = 0;
  string temp;
  while (getline(ifs,temp)) {
    double time = stof(temp);
    read_time += time;
  }
  ifs.close();

  ifs.open("write_time.txt") ;
  if (!ifs.is_open()) {
    cout << "文件打开失败" << endl;
    return 0;
  }

  double write_time = 0;
  while (getline(ifs,temp)) {
    double time = stof(temp);
    write_time += time;
  }
  ifs.close();
   double a = 4723/write_time;
   double b = 4723/read_time;
   cout<<"write time:"<<write_time<<endl;
   cout<<"read time:"<<read_time<<endl;
   cout<<"write带宽:"<<a<<"MB/s"<<endl;
   cout<<"read带宽:"<<b<<"MB/s"<<endl;
  return 0;
}