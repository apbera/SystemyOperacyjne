#ifndef ZAD3_LIBRARY_H
#define ZAD3_LIBRARY_H

#include <stdio.h>

size_t result_size;
char **result;
char *directory;
char *file_name;
char *tmp_file_name;

void createResult(size_t size);
void setDir(char *dir_name);
void setFileName(char *file_name);
void setTmpName(char *tmpname);
void findAndSaveResults();
int tmpToResult();
void deleteBlock(int ind);
void freeResult();

#endif