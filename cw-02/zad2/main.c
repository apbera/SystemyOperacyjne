#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

char *toString(time_t time) {
    char *result = malloc(30 * sizeof(char));
    strftime(result, 30, "%Y-%m-%d,%H:%M:%S", localtime(&time));
    return result;
}
time_t toTime(const char* date_str)
{
    time_t result;
    struct tm time;
    if(strptime(date_str,"%Y-%m-%d,%H:%M:%S",&time)==NULL)
    {
        printf("Failed to match the format");
        exit(1);
    }
    result= mktime(&time);
    return result;
}

void show_more(char *full_path,struct stat file_stats){

    char *type="unknown";
    if(S_ISREG(file_stats.st_mode))type="file";
    else if(S_ISDIR(file_stats.st_mode))type="dir";
    else if(S_ISCHR(file_stats.st_mode))type="char dev";
    else if(S_ISBLK(file_stats.st_mode))type="block dev";
    else if(S_ISFIFO(file_stats.st_mode))type="fifo";
    else if(S_ISLNK(file_stats.st_mode))type="slink";
    else if(S_ISSOCK(file_stats.st_mode))type="sock";


    printf("Sciezka bezwgledna do pliku: %s\nRodzaj pliku: %s\nRozmiar w bajtach: %d\n"
           "Data ostatniego dostepu: %s\nData ostatniej modyfikacji: %s\n",
            full_path,type,(int) file_stats.st_size,toString(file_stats.st_atime),toString(file_stats.st_mtime));
}


void search_dir(char *dir_name, char *opt, time_t time_str){

    char *full_path=realpath(dir_name,NULL);
    DIR *dir;
    if((dir=opendir(full_path))==NULL){
        printf("Dir not found");
        exit(1);
    }
    struct dirent *curr;
    struct stat curr_stat;
    char new_path[4096];
    while((curr=readdir(dir))!=NULL){

        if(strcmp(curr->d_name,".")==0 || strcmp(curr->d_name,"..")==0){
            continue;
        }

        strcpy(new_path,full_path);

        if(full_path[strlen(full_path)-1]!='/')strcat(new_path,"/");

        strcat(new_path,curr->d_name);
        if(lstat(new_path,&curr_stat)<0){
            printf("Problem with function lstat");
            exit(1);
        }


        if(opt[0]=='>'){
             if(difftime(curr_stat.st_mtime,time_str)>0){
                show_more(new_path,curr_stat);
            }
        }
        else if(opt[0]=='<'){
            if(difftime(curr_stat.st_mtime,time_str)<0){
                show_more(new_path,curr_stat);
            }
        }
        else if(opt[0]=='='){
            if(difftime(curr_stat.st_mtime,time_str)==0){
                show_more(new_path,curr_stat);
            }
        }

        if(S_ISDIR(curr_stat.st_mode)){
            search_dir(new_path,opt,time_str);
        }

    }
    closedir(dir);
}

int main(int argc, char **argv) {
    if(argc!=4){
        printf("Expected 3 arguments");
        exit(1);
    }
    time_t full_time;
    full_time=toTime(argv[3]);
    search_dir(argv[1],argv[2],full_time);

    return 0;
}