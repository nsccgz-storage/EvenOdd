#ifndef DECODING_H
#define DECODING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

static unsigned int block_size;

// 从 disk 目录下读取对应的数据列
void readDataColumn(char* filename, int id, int file_size, char* result){
    FILE* input;
    char file_path[PATH_MAX_LEN];
    sprintf(file_path, "disk_%d/%s", id, filename);
    input = fopen(file_path, "rb");
    fseek(input, sizeof(int), SEEK_SET);
    fread(result, file_size, 1, input);
    fclose(input);
}


void block_xor(char* left, char* right, char* result){
    for(int i = 0;i < block_size;i++){
        result[i] = left[i] ^ right[i];
    }
}

void block_xoreq(char* left, char*right){
    block_xor(left, right, left);
}

void printBlock(char* data){
    for(int i = 0;i < block_size;i++){
        printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(data[i]));
    }
    printf("\n");
}

void printColumn(char* data, int p){
    for(int i = 0;i < (p-1)*block_size;i++){
        if(i % block_size == 0)printf("\n");
        printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(data[i]));
    }
    printf("\n");
}

int startWithDisk(char* str, int len){
    if(len <= 5)return 0; // false
    const char* pattern = "disk_";
    for(int i = 0;i < 5;i++){
        if(str[i] != pattern[i])return 0;
    }
    return 1; // true
}
/* 
 * 先实现 malloc 版本
 * 之后再实现 手动管理内存缓冲区版本
 */
void read1(char* filename, char* save_as){
    // 确定 p
    DIR* d = opendir(".");
    if(!d){
        perror("Error in myRead");
        exit(1);
    }

    struct dirent *dir;
    struct stat st = {0};
    unsigned int file_size = 0;
    int p = 0;
    int failed_num = 0; 
    int failed[2];
    char file_path[PATH_MAX_LEN];

    while( (dir=readdir(d)) != NULL){
        if(dir->d_name[0] == '.')continue;
        if(!startWithDisk(dir->d_name, dir->d_reclen))continue;

        int disk_id;
        sscanf(dir->d_name, "disk_%d", &disk_id);
        sprintf(file_path, "disk_%d/%s", disk_id, filename);

        // could not find the file
        if(stat(file_path, &st) != 0){
            failed_num++;

            if(failed_num == 1){
                failed[0] = disk_id;
            }else if(failed_num == 2){
                // ensure failed[0] < failed[1]
                if(disk_id < failed[0]){
                    failed[1] = failed[0];
                    failed[0] = disk_id;
                }
                else failed[1] = disk_id;
            }
        }
        else if(p == 0){ // determine the p value
            file_size = st.st_size - sizeof(int); // 确定每个文件的大小
            FILE* file;
            file = fopen(file_path, "rb");
            fread(&p, sizeof(int), 1, file);
            fclose(file);

            printf("[Debug]: p = %d, file size = %d\n", p, file_size);
        }
    }
    closedir(d);

    if(p == 0){
        printf("File does not exist!\n");
        exit(1);
    }

    if(failed_num > 2){
        printf("File corrupted!\n");
        exit(1);
    }

    /* start decoding */
    // assert file_size % (p-1) == 0
    block_size = file_size / (p-1);

    if(failed_num == 0){
        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }
        // TODO: buffer size 的限制
        char* buffer = (char*)malloc(file_size);
        for(int i = 0;i < p;i++){ 
            readDataColumn(filename, i, file_size, buffer);
            fwrite(buffer, file_size, 1, output);
        }
        fclose(output);
        free(buffer);
    }
    else if(failed_num == 1){
        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }

        if(failed[0] == p || failed[0] == p+1){ // diagonal_parity or row_parity
            printf("disk_%d failed, read directly\n", failed[0]);
            // TODO: the limit of buffer size
            char* buffer = (char*)malloc(file_size);
            for(int i = 0;i < p;i++){ 
                readDataColumn(filename, i, file_size, buffer);
                fwrite(buffer, file_size, 1, output);
            }
            free(buffer);
        }else{ // repair some data column
            printf("disk_%d failed, repair and read\n", failed[0]);
            // TODO: the limit of buffer size
            char* buffer = (char*)malloc(file_size * (p+1));
            char* missed_column = buffer + file_size * failed[0];
            memset(missed_column, 0, file_size);

            for(int i = 0;i < p+1;i++){
                if(i != failed[0]){
                    char* current = buffer+file_size*i;
                    readDataColumn(filename, i, file_size, current);
                    for(int j = 0;j < p-1;j++){
                        block_xoreq(missed_column+j*block_size, current+j*block_size);
                    }
                }
            }

            for(int i = 0;i < p;i++){ 
                fwrite(buffer+file_size*i, file_size, 1, output);
            }

            free(buffer);
        }
        fclose(output);
    }
    else{ // failed_num == 2
        FILE* output = fopen(save_as, "wb");
        if(!output){
            perror("Error in writing file");
            exit(1);
        }

        if(failed[1] == p+1){
            if(failed[0] == p){ // case 1: diagonal_parity and row_parity
                printf("case 1: disk_%d and disk_%d failed, read directly\n", failed[0], failed[1]);

                // TODO: the limit of buffer size
                char* buffer = (char*)malloc(file_size);
                for(int i = 0;i < p;i++){ 
                    readDataColumn(filename, i, file_size, buffer);
                    fwrite(buffer, file_size, 1, output);
                }
                free(buffer);
            }
            else{ // case 2: diagonal_parity and data column
                printf("case 2: disk_%d and disk_%d failed, repair a data column and read\n", failed[0], failed[1]);
                // TODO: the limit of buffer size
                char* buffer = (char*)malloc(file_size * (p+1));
                char* missed_column = buffer + file_size * failed[0];
                memset(missed_column, 0, file_size);

                for(int i = 0;i < p+1;i++){
                    if(i != failed[0]){
                        char* current = buffer+file_size*i;
                        readDataColumn(filename, i, file_size, current);

                        for(int j = 0;j < p-1;j++){
                            block_xoreq(missed_column+j*block_size, current+j*block_size);
                        }
                    }
                }

                for(int i = 0;i < p;i++){ 
                    fwrite(buffer+file_size*i, file_size, 1, output);
                }

                free(buffer);
            }
        }else if(failed[1] == p){ // case 3: row parity and data column
            printf("case 3: disk_%d and disk_%d failed, repair a data column and read\n", failed[0], failed[1]);

            // TODO: buffer size 的限制
            char* buffer = (char*)malloc(file_size * p);
            char* missed_column = buffer + file_size * failed[0];
            char* diagonal_parity = (char*)malloc(file_size);
            memset(missed_column, 0, file_size);

            // calculate S
            char* S = (char* ) malloc(block_size);
            memset(S, 0, block_size);
            for(int i = 0;i < p;i++){
                if(i == failed[0])continue;
                char* current = buffer+file_size*i;
                readDataColumn(filename, i, file_size, current);
                block_xoreq(S, current+((p+failed[0]-i-1)%p)*block_size);
            }
            readDataColumn(filename, p+1, file_size, diagonal_parity);
            if(failed[0] != 0){
                block_xoreq(S, diagonal_parity+(failed[0] - 1)*block_size);
            }


            // repair missed column with S
            for(int k = 0;k < p-1;k++){
                char* current = missed_column + k*block_size;
                memcpy(current, S, block_size);
                if((failed[0] + k) % p != p-1){
                    block_xoreq(current, diagonal_parity + (failed[0]+k)%p*block_size);
                }
                for(int i = 0;i < p;i++){
                    if(i == failed[0])continue;
                    if((k+failed[0]-i+p)%p == p-1)continue;

                    block_xoreq(current, buffer + i*file_size + (k+failed[0]-i+p)%p*block_size);
                }
            }

            // write to local file
            for(int i = 0;i < p;i++){ 
                fwrite(buffer+file_size*i, file_size, 1, output);
            }
            
            // free space
            free(S);
            free(buffer);
            free(diagonal_parity);
            
        }else{ // case 4: two data column
            printf("case 4: disk_%d and disk_%d failed, repair two data columns and read\n", failed[0], failed[1]);

            char* S = (char* ) malloc(block_size);
            memset(S, 0, block_size); // S = 0

            // TODO: buffer size 的限制
            char* buffer = (char*)malloc(file_size * p); // 无需读 row parity
            char* row_parity = (char*)malloc(file_size);
            char* diagonal_parity = (char*)malloc(file_size);
            for(int i = 0;i < p;i++){
                if(i == failed[0] || i == failed[1]){
                    memset(buffer+file_size*i, 0, file_size);
                    continue;
                }
                readDataColumn(filename, i, file_size, buffer+file_size*i);
            }
            readDataColumn(filename, p, file_size, row_parity);
            readDataColumn(filename, p+1, file_size, diagonal_parity);

            // calculate S
            for(int k = 0;k < p-1;k++){
                block_xoreq(S, row_parity+k*block_size);
                block_xoreq(S, diagonal_parity+k*block_size);
            }

            char* R = (char*)malloc(file_size+block_size); // 缺失的两列的同一行元素异或值；
            char* D = (char*)malloc(file_size+block_size); // 缺失的两列的同一对角线元素异或值；
            memcpy(R, row_parity, file_size); // R[0, ..., p-1] = row_parity
            memcpy(D, diagonal_parity, file_size); // D[0. ..., p-1] = diagonal_parity
            memset(D+(p-1)*block_size, 0, block_size); // D[p-1] = 0; 
            // R[p] = 0; 
            
            for(int k = 0;k < p-1;k++){
                for(int i = 0;i < p;i++){
                    if(i == failed[0] || i == failed[1])continue;
                    block_xoreq(R+k*block_size, buffer+i*file_size+k*block_size);
                }
            }
            
            for(int k = 0;k < p;k++){
                block_xoreq(D+k*block_size, S); // D[k] ^= S;
                
                for(int i = 0;i < p;i++){
                    if(i == failed[0] || i == failed[1] || (p+k-i)%p == p-1)continue;
                    block_xoreq(D+k*block_size, buffer+i*file_size+(p+k-i)%p*block_size); // D[k] ^= data[i][(p+k-i)%p];
                }
            }

            // calculate the two missed data columns 
            int m = p-1-(failed[1]-failed[0]); //  0 <= m <= p-2
            char* missed_1 = buffer + failed[0]*file_size;
            char* missed_2 = buffer + failed[1]*file_size;
            do{
                char* D1 = D + (failed[1] + m)%p * block_size;
                char* R1 = R + m * block_size;
                char* cur_1 = missed_1 + m * block_size;
                char* cur_2 = missed_2 + m * block_size;

                if((m+failed[1]-failed[0])%p != p-1){
                    // data[failed[1]][m] = D[(failed[1]+m)%p] ^ data[failed[0]][(m+failed[1]-failed[0])%p];
                    block_xor(D1, missed_1+(m+failed[1]-failed[0])%p*block_size, cur_2);
                }else{
                    // data[failed[1]][m] = D[(failed[1]+m)%p];
                    memcpy(cur_2, D1, block_size);
                }
                // data[failed[0]][m] = R[m] ^ data[failed[1]][m];
                block_xor(R1, cur_2, cur_1);
                m = (p + m - (failed[1]-failed[0]))%p; // p 为质数则可以保证p次迭代不重不漏地遍历[0, p-1]的每个整数
            }while(m != p-1);

            // write to local file
            for(int i = 0;i < p;i++){ 
                fwrite(buffer+file_size*i, file_size, 1, output);
            }

            // free space
            free(buffer);
            free(row_parity);
            free(diagonal_parity);
            free(S);
            free(R);
            free(D);
        }

        fclose(output);
    }
}
#endif