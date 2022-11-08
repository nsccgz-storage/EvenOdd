#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK "./disk"
#define PATH_MAX_LEN 60

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

struct stat st = {0};

void usage() {
    printf("./evenodd write <file_name> <p>\n");
    printf("./evenodd read <file_name> <save_as>\n");
    printf("./evenodd repair <number_erasures> <idx0> ...\n");
}

/* 
 * Please encode the input file with EVENODD code
 * and store the erasure-coded splits into corresponding disks
 * For example: Suppose "file_name" is "testfile", and "p" is 5. After your
 * encoding logic, there should be 7 splits, "testfile_0", "testfile_1",
 * ..., "testfile_6", stored in 7 diffrent disk folders from "disk_0" to
 * "disk_6".
 */
void myWrite(char* file_path, int p){
    if(p <= 2){
        // TODO: 缺少函数 isPrime() 来检查是否为大于 2 的质数
        printf("p should greater than 0 but p = %d\n", p);
        return ;
    }

    /* init */
    // 假定输入为大小 (p-1)*p, 即有 p 列，每列有 p-1 个元素
    char** data = (char**)malloc(sizeof(char*) * p);
    for(int i = 0;i < p;i++){
        data[i] = (char*)malloc(sizeof(char) * (p-1));
        memset(data[i], 0, p-1);
    }
    char* row_parity = (char*)malloc(sizeof(char) * (p-1));
    char* diagonal_parity = (char*)malloc(sizeof(char) * (p-1));

    /* read input file */
    FILE* input = fopen(file_path, "rb");
    if(input == NULL){
        printf("can't find the file %s\n", file_path);
        return ;
    }
    int num = 0;
    while(num < p && fread(data[num], sizeof(char), p-1, input)){
        num++;
    }
    fclose(input);

    // check input data
    // for(int i = 0;i < p;i++){
    //     printf("column %d:", i);
    //     for(int j = 0;j < p-1;j++){
    //         printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(data[i][j]));
    //     }
    //     printf("\n");
    // }

    /* encoding */
    int S = 0;
    for(int i = 0;i < p;i++){
        for(int j = 0;j < p-1;j++){
            row_parity[j] ^= data[i][j]; 
            if(i + j == p-1){
                S ^= data[i][j];
            }else if(i + j < p-1){
                diagonal_parity[i+j] ^= data[i][j];
            }else{
                diagonal_parity[i+j-p] ^= data[i][j];
            }
        }
    }
    for(int i = 0;i < p-1;i++){
        diagonal_parity[i] ^= S;
    }

    // just for test: output encoding column
    printf("row parity:");
    for(int i = 0;i < p-1;i++){
        printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(row_parity[i]));
    }
    printf("\ndiagonal parity:");
    for(int i = 0;i < p-1;i++){
        printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(diagonal_parity[i]));
    }
    printf("\n");


    /* write to disk */
    char* filename = basename(file_path);
    FILE* output;
    char output_path[PATH_MAX_LEN];
    size_t write_count = 0;
    for(int i = 0;i < p+2;i++){
        // create directory if not exist
        sprintf(output_path, DISK"/disk_%d", i);
        if(stat(output_path, &st) == -1){
            mkdir(output_path, 0700); // mode 0700 = read,write,execute only for owner
        }

        // open file
        sprintf(output_path, DISK"/disk_%d/%s_%d",i, filename, i); 
        output = fopen(output_path, "wb");
        if(!output){
            perror("Error in creating file");
            exit(1);
        }

        // write file
        char* ptr;
        if(i < p){
            ptr = data[i];
        }else if(i == p){
            ptr = row_parity;
        }else{
            ptr = diagonal_parity;
        }
        write_count += fwrite(ptr, sizeof(char), p-1, output);
        fclose(output);
    }
    
    printf("Totally write %ld elements\n", write_count);

    for(int i = 0;i < p;i++)free(data[i]);
    free(data);
    free(row_parity);
    free(diagonal_parity);
}

void readDataColumn(char* filename, int id, int file_size, char* result);

void repairDataColumn(int id, char** data, int p);



/* 
 * Please read the file specified by "file_name", and store it as a file
 * named "save_as" in the local file system.
 * For example: Suppose "file_name" is "testfile" (which we have encoded 
 * before), and "save_as" is "tmp_file". After the read operation, there 
 * should be a file named "tmp_file", which is the same as "testfile".
 */
void myRead(char* filename, char* save_as){

    // TODO: 如何确定 p,目前考虑通过 (disk_x 文件夹个数 - 2) 来确定

    // check disk
    DIR* d = opendir(DISK);
    if(!d){
        perror("Error in myRead");
        exit(1);
    }
    struct dirent *dir;
    int disk_num = 0;
    int failed_num = 0;
    int id = -1;
    char file_path[PATH_MAX_LEN];
    int failed_1 = -1, failed_2 = -1;
    int file_size = 0;
    while((dir = readdir(d)) != NULL){
        if(dir->d_name[0] == '.')continue;
        disk_num++;
        sscanf(dir->d_name, "disk_%d", &id);
        sprintf(file_path, DISK"/disk_%d/%s_%d", id, filename, id);

        // printf("id = %d, file path = %s\n", id, file_path);
        if(stat(file_path, &st) != 0){
            failed_num++;
            // printf("disk_%d failed, ", id);
            if(failed_num == 1){
                failed_1 = id;
            }else if(failed_num == 2){
                if(id < failed_1){
                    failed_2 = failed_1;
                    failed_1 = id;
                }
                else failed_2 = id;
            }
        }else{
            if(file_size == 0){
                file_size = st.st_size;// 假定所有文件大小一样
            }
        }
    }
    closedir(d);

    // TODO: 题意理解？是不是 disk 文件夹下没有文件块时表示“要读的文件不存在”
    if(disk_num == failed_num){ 
        printf("File does not exist!\n");
        exit(1);
    }

    if(failed_num > 2){
        printf("File corrupted!\n");
        exit(1);
    }else if(failed_num == 0){
        printf("no disk fails, read directly\n");

        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }
        char* buffer = (char*)malloc(file_size);
        for(int i = 0;i < disk_num-2;i++){ // 读前 p 个文件即可
            readDataColumn(filename, i, file_size, buffer);
            fwrite(buffer, file_size, 1, output);
        }
        fclose(output);
        free(buffer);
        // test ......
    }else if(failed_num == 1){ 
        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }

        if(failed_1 == disk_num - 2 || failed_1 == disk_num - 1){ // diagonal_parity or row_parity
            printf("disk_%d failed, read directly\n", failed_1);

            char* buffer = (char*)malloc(file_size);
            for(int i = 0;i < disk_num-2;i++){ 
                readDataColumn(filename, i, file_size, buffer);
                fwrite(buffer, file_size, 1, output);
            }
            free(buffer);
        }else{ // repair some data column
            printf("disk_%d failed, repair and read\n", failed_1);

            char** data = (char**) malloc(sizeof(char*) * disk_num-1);
            for(int i = 0;i < disk_num-1;i++){ // 可以不读取 diagonal parity
                data[i] = (char*) malloc(sizeof(char) * file_size);
                if(i != failed_1){
                    readDataColumn(filename, i, file_size, data[i]);
                }
            }
            repairDataColumn(failed_1, data, disk_num-2);
            for(int i = 0;i < disk_num-2;i++){ 
                fwrite(data[i], file_size, 1, output);
            }

            // free space
            for(int i = 0;i < disk_num-1;i++)free(data[i]);
            free(data);
        }
        fclose(output);
        // TODO: testing
    }else{ // failed number = 2
        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }

        if(failed_2 == disk_num - 1){
            if(failed_1 == disk_num -2){ // case 1: diagonal_parity and row_parity，无需修复
                printf("case 1: disk_%d and disk_%d failed, read directly\n", failed_1, failed_2);

                char* buffer = (char*)malloc(file_size);
                for(int i = 0;i < disk_num-2;i++){ // 读前 p 个文件即可
                    readDataColumn(filename, i, file_size, buffer);
                    fwrite(buffer, file_size, 1, output);
                }
                free(buffer);
            }
            else{ // case 2: diagonal_parity and data column
                printf("case 2: disk_%d and disk_%d failed, repair a data column and read\n", failed_1, failed_2);
                char** data = (char**) malloc(sizeof(char*) * disk_num-1);
                for(int i = 0;i < disk_num-1;i++){ // 可以不读取 diagonal parity
                    data[i] = (char*) malloc(sizeof(char) * file_size);
                    if(i != failed_1){
                        readDataColumn(filename, i, file_size, data[i]);
                    }
                }
                repairDataColumn(failed_1, data, disk_num-2);
                for(int i = 0;i < disk_num-2;i++){ 
                    fwrite(data[i], file_size, 1, output);
                }

                // free space
                for(int i = 0;i < disk_num-1;i++)free(data[i]);
                free(data);
            }
        }else if(failed_2 == disk_num - 2){ // case 3: row parity and data column
            printf("case 3: disk_%d and disk_%d failed, repair a data column and read\n", failed_1, failed_2);

            char** data = (char**) malloc(sizeof(char*) * disk_num);
            for(int i = 0;i < disk_num;i++){ 
                data[i] = (char*) malloc(sizeof(char) * file_size);
                if(i != failed_1 && i != failed_2){
                    readDataColumn(filename, i, file_size, data[i]);
                }else{
                    memset(data[i], 0, disk_num-2);
                }
            }
            // 求 S
            int S = 0, p = disk_num - 2;
            for(int i = 0;i < p;i++){ 
                if(i == failed_1)continue;
                S ^= data[i][(p+failed_1-i-1)%p];
            }
            if(failed_1 != 0){
                S ^= data[p+1][failed_1-1];
            }

            // repair missed column with S
            for(int k = 0;k < p-1;k++){
                data[failed_1][k] = S;
                if((failed_1 + k) % p != p-1)data[failed_1][k] ^= data[p+1][(failed_1+k)%p];
                for(int i = 0;i < p;i++){
                    if(i == failed_1)continue;
                    if((k+failed_1-i+p)%p == p-1)continue;

                    data[failed_1][k] ^= data[i][(k+failed_1-i+p)%p];
                }
            }

            // write to local file
            for(int i = 0;i < p;i++){ 
                fwrite(data[i], file_size, 1, output);
            }

            // free space
            for(int i = 0;i < disk_num;i++)free(data[i]);
            free(data);
        }else{ // case 4: two data column
            printf("case 4: disk_%d and disk_%d failed, repair two data columns and read\n", failed_1, failed_2);

            int S = 0, p = disk_num - 2;

            char** data = (char**) malloc(sizeof(char*) * disk_num);
            for(int i = 0;i < disk_num;i++){ 
                data[i] = (char*) malloc(sizeof(char) * file_size);
                if(i != failed_1 && i != failed_2){
                    readDataColumn(filename, i, file_size, data[i]);
                }else{
                    memset(data[i], 0, disk_num-2);
                }
            }

            // 求解 S
            for(int k = 0;k < p-1;k++){
                S ^= data[p][k] ^ data[p+1][k];
            }

            char* R = (char*)malloc(sizeof(char) * p); // 缺失的两列的同一行元素异或值；
            char* D = (char*)malloc(sizeof(char) * p); // 缺失的两列的同一对角线元素异或值；
            memcpy(R, data[p], p-1); 
            memcpy(D, data[p+1], p-1);
            R[p-1] = 0; 
            for(int k = 0;k < p-1;k++){
                for(int i = 0;i < p;i++){
                    if(i == failed_1 || i == failed_2)continue;
                    R[k] ^= data[i][k];
                }
            }
            D[p-1] = 0;
            for(int k = 0;k < p;k++){
                D[k] ^= S;
                for(int i = 0;i < p;i++){
                    if(i == failed_1 || i == failed_2 || (p+k-i)%p == p-1)continue;
                    D[k] ^= data[i][(p+k-i)%p];
                }
            }

            // 求解缺失的数据列
            int m = p-1-(failed_2-failed_1); //  0 <= m <= p-2
            do{
                if((m+failed_2-failed_1)%p != p-1){
                    data[failed_2][m] = D[(failed_2+m)%p] ^ data[failed_1][(m+failed_2-failed_1)%p];
                }else{
                    data[failed_2][m] = D[(failed_2+m)%p];
                }
                data[failed_1][m] = R[m] ^ data[failed_2][m];
                m = (p + m - (failed_2-failed_1))%p; // p 为质数则可以保证p次迭代不重不漏地遍历[0, p-1]的每个整数
            }while(m != p-1);

            // write to local file
            for(int i = 0;i < p;i++){ 
                fwrite(data[i], file_size, 1, output);
            }

            // free space
            for(int i = 0;i < disk_num;i++)free(data[i]);
            free(data);
            free(R);
            free(D);
        }

        fclose(output);
    }
}

/*
 * Please repair failed disks. The number of failures is specified by
 * "num_erasures", and the index of disks are provided in the command
 * line parameters.
 * For example: Suppose "number_erasures" is 2, and the indices of
 * failed disks are "0" and "1". After the repair operation, the data
 * splits in folder "disk_0" and "disk_1" should be repaired.
 */
void myRepair(int num, int* indices){
    if(num > 2){
        printf("Too many corruptions!\n");
        exit(-1);
    }
    if(num == 1){

    }else if(num == 2){
        
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return -1;
    }

    char* op = argv[1];
    if (strcmp(op, "write") == 0) {

        if(argc < 4){
            usage();
            return -1;
        }
        char* file_path = argv[2];
        int p = atoi(argv[3]);
        myWrite(file_path, p);
        
    } else if (strcmp(op, "read") == 0) {
        if(argc < 4){
            usage();
            return -1;
        }
        myRead(argv[2], argv[3]);
    } else if (strcmp(op, "repair") == 0) {
        if(argc < 4){
            usage();
            return -1;
        }
        int erasures_num = atoi(argv[2]);
        if(argc - 3 != erasures_num){
            printf("require %d disk, but get %d\n", erasures_num, argc - 3);
            usage();
            return -1;
        }
        int* indices = (int*)malloc(sizeof(int) * erasures_num);
        printf("num erasures: %d, ", erasures_num);
        for(int i = 0;i < erasures_num;i++){
            indices[i] = atoi(argv[i + 3]);
            printf("disk %d, ", indices[i]);
        }
        printf("\n");
        
        myRepair(erasures_num, indices);
        free(indices);
    } else {
        printf("Non-supported operations!\n");
    }
    return 0;
}

// 从 disk 目录下读取对应的数据列
void readDataColumn(char* filename, int id, int file_size, char* result){
    FILE* input;
    char file_path[PATH_MAX_LEN];
    sprintf(file_path, DISK"/disk_%d/%s_%d", id, filename, id);
    input = fopen(file_path, "rb");
    fread(result, file_size, 1, input);
    fclose(input);
}

// 修复数据列
void repairDataColumn(int id, char** data, int p){
    memset(data[id], 0, p-1);
    
    for(int i = 0;i < p+1;i++){
        if(i == id)continue;
        for(int j = 0;j < p-1;j++){
            data[id][j] ^= data[i][j];
        }
    }
}