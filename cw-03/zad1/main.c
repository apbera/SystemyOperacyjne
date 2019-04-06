#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int ls_dir(char *dir_name){

    pid_t child;
    child = fork();

    if(child == 0){
        printf("Path: %s\nPID procesu: %d\n",dir_name,(int)getpid());
        execlp("/bin/ls", "ls", "-l", dir_name, NULL);
    }
    else {
        int status;
        wait(&status);
        if(status!=0) {
            return status;
        }
    }

    DIR *dir;
    if((dir=opendir(dir_name))==NULL){
        printf("Dir not found");
        exit(1);
    }
    struct dirent *curr;
    struct stat curr_stat;
    char new_path[4096];
    while ((curr = readdir(dir)) != NULL) {
        if (strcmp(curr->d_name, ".") == 0 || strcmp(curr->d_name, "..") == 0) {
            continue;
        }
        strcpy(new_path,dir_name);

        if(dir_name[strlen(dir_name)-1]!='/')strcat(new_path,"/");

        strcat(new_path,curr->d_name);

        if (lstat(new_path, &curr_stat) < 0) {
            printf("Problem with function lstat");
            exit(1);
        }

        if (S_ISDIR(curr_stat.st_mode)) {
            ls_dir(new_path);
        }

    }
    closedir(dir);
    return 0;
}

int main(int argc, char **argv) {

    if(argc != 2){
        printf("Not enough arguments");
        exit(1);
    }

    ls_dir(argv[1]);

    return 0;
}