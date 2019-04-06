#include "library.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t result_size=0;

void createResult(size_t size){
    if(size<1){
        printf("Size of Array must be greater than 0");
    }
    else{
        result_size=size;
        result=calloc(result_size, sizeof(char*));
    }
}

void setDir(char *dirname) {
    directory = dirname;
}

void setFileName(char *filename){
    file_name=filename;
}

void setTmpName(char *tmpname){
    tmp_file_name=tmpname;
}

void findAndSaveResults(){
    char *command=calloc(strlen(directory)+strlen(file_name)+strlen(tmp_file_name)+16, sizeof(char));
    strcpy(command,"find ");
    strcat(command, directory);
    strcat(command, " -name \"");
    strcat(command,file_name);
    strcat(command, "\" > ");
    strcat(command, tmp_file_name);
    system(command);
    free(command);
}

int tmpToResult(){
    if(result_size==0){
        printf("Array doesn't exist");
        return -1;
    }
    FILE *file=fopen(tmp_file_name,"r");
    if(file==NULL){
        printf("Temporary file was not found");
        return -1;
    }
    size_t file_size;
    fseek(file,0,SEEK_END);
    file_size=(size_t)ftell(file);
    rewind(file);
    char *block=(char*) calloc(file_size, sizeof(char));

    char tmp=(char) getc(file);
    int i=0;
    while(tmp!=EOF){
        block[i]=tmp;
        i++;
        tmp=(char) getc(file);
    }

    for(i=0;i<result_size;i++){
        if(result[i]==NULL){
            result[i]=block;
            return i;
        }
    }

    return -1;
}

void deleteBlock(int ind){
    if(result[ind]==NULL){
        printf("Block with index %d doesn't exist",ind);
        return;
    }
    free(result[ind]);
    result[ind]=NULL;
}

void freeResult(){
    if(result_size<1){
        return;
    }
    for(int i=0; i<result_size; i++){
        if(result[i]!=NULL){
            free(result[i]);
        }
    }
    free(result);
}