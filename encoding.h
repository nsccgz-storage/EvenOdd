/*
 * 仅仅为了测试 decoding.h 中函数的正确性
 */

#ifndef ENCODING_H
#define ENCODING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PATH_MAX_LEN 60

int BLOCK_SIZE; 
void block_xor_1(char* left, char* right, char* result){
    for(int i = 0;i < BLOCK_SIZE;i++){
        result[i] = left[i] ^ right[i];
    }
}

// left ^= right
void block_xoreq_1(char* left, char* right){
    block_xor_1(left, right, left);
}

// 注意：在前 4 byte 写入 p，文件命名格式调整为 filename
void write1(char* file_path, int p){
    struct stat st = {0};

    if(p <= 2){
        // TODO: 缺少函数 isPrime() 来检查是否为大于 2 的质数
        printf("p should greater than 0 but p = %d\n", p);
        return ;
    }

    /* read input file */
    FILE* fptr = fopen(file_path, "rb");
    if(fptr == NULL){
        printf("can't find the file %s\n", file_path);
        return ;
    }
    fseek(fptr, 0, SEEK_END);
    size_t file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    
    int block_num = p * (p-1);
    if(file_size % block_num != 0){
        // TODO: padding
    }

    BLOCK_SIZE = file_size % block_num ? file_size / block_num + 1 : file_size / block_num;

    /* init */
    char** data = (char**)malloc(sizeof(char*) * p);
    for(int i = 0;i < p;i++){
        data[i] = (char*)malloc((p-1)*BLOCK_SIZE);
        memset(data[i], 0, (p-1)*BLOCK_SIZE);
    }
    char* row_parity = (char*)malloc((p-1)*BLOCK_SIZE);
    char* diagonal_parity = (char*)malloc((p-1)*BLOCK_SIZE);

    int num = 0;
    while(num < p && fread(data[num], BLOCK_SIZE, p-1, fptr)){
        num++;
    }
    fclose(fptr);

    // check input data
    // for(int i = 0;i < p;i++){
    //     printf("column %d:", i);
    //     for(int j = 0;j < (p-1)*BLOCK_SIZE;j++){
    //         if(j % BLOCK_SIZE == 0)printf("\n");
    //         printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(data[i][j]));
    //     }
    //     printf("\n");
    // }

    /* encoding */
    char* S = (char* ) malloc(BLOCK_SIZE);
    memset(S, 0, BLOCK_SIZE);
    memset(row_parity, 0, (p-1)*BLOCK_SIZE);
    memset(diagonal_parity, 0, (p-1)*BLOCK_SIZE);

    for(int i = 0;i < p;i++){
        for(int j = 0;j < p-1;j++){
            char* current = data[i]+j*BLOCK_SIZE;
            block_xoreq_1(row_parity+j*BLOCK_SIZE, current);
            // row_parity[j] ^= data[i][j]; 
            if(i + j == p-1){
                block_xoreq_1(S, current);
                // S ^= data[i][j];
            }else if(i + j < p-1){
                block_xoreq_1(diagonal_parity+(i+j)*BLOCK_SIZE, current);
                // diagonal_parity[i+j] ^= data[i][j];
            }else{
                block_xoreq_1(diagonal_parity+(i+j-p)*BLOCK_SIZE, current);
                // diagonal_parity[i+j-p] ^= data[i][j];
            }
        }
    }
    for(int i = 0;i < p-1;i++){
        block_xoreq_1(diagonal_parity+i*BLOCK_SIZE, S);
        // diagonal_parity[i] ^= S;
    }

    // just for test: output encoding column
    // printf("row parity:");
    // for(int i = 0;i < (p-1)*BLOCK_SIZE;i++){
    //     if(i % BLOCK_SIZE == 0)printf("\n");
    //     printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(row_parity[i]));
    // }
    // printf("\ndiagonal parity:");
    // for(int i = 0;i < (p-1)*BLOCK_SIZE;i++){
    //     if(i % BLOCK_SIZE == 0)printf("\n");
    //     printf(BYTE_TO_BINARY_PATTERN",", BYTE_TO_BINARY(diagonal_parity[i]));
    // }
    // printf("\n");


    /* write to disk */
    char* filename = basename(file_path);
    FILE* output;
    char output_path[PATH_MAX_LEN];
    size_t write_count = 0;
    
    for(int i = 0;i < p+2;i++){
        write_count = 0;

        // create directory if not exist
        sprintf(output_path, "./disk_%d", i);
        if(stat(output_path, &st) == -1){
            mkdir(output_path, 0700); // mode 0700 = read,write,execute only for owner
        }

        // open file
        sprintf(output_path, "./disk_%d/%s",i, filename); 
        output = fopen(output_path, "wb");
        if(!output){
            perror("Error in creating file");
            exit(1);
        }
        write_count += fwrite(&p, sizeof(int), 1, output); // write p into first 4 byte of file

        // write file
        char* ptr;
        if(i < p){
            ptr = data[i];
        }else if(i == p){
            ptr = row_parity;
        }else{
            ptr = diagonal_parity;
        }
        write_count += fwrite(ptr, BLOCK_SIZE, p-1, output);
        fclose(output);

        // printf("Totally write %ld elements to disk_%d\n", write_count, i); // 应该为 4 + 64 byte
    }
    
    

    for(int i = 0;i < p;i++)free(data[i]);
    free(data);
    free(row_parity);
    free(diagonal_parity);
    free(S);
}

#endif