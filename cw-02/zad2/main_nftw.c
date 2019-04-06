#define _XOPEN_SOURCE 500
#define _BSD_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <ftw.h>


time_t mod_time;
char *optio;
char *path_nftw;

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
    result= mktime(&time)+60*60;
    return result;
}
void nftw_show(const char *full_path,const struct stat *file_stats){

    char *type="unknown";
    if(S_ISREG(file_stats->st_mode))type="file";
    else if(S_ISDIR(file_stats->st_mode))type="dir";
    else if(S_ISCHR(file_stats->st_mode))type="char dev";
    else if(S_ISBLK(file_stats->st_mode))type="block dev";
    else if(S_ISFIFO(file_stats->st_mode))type="fifo";
    else if(S_ISLNK(file_stats->st_mode))type="slink";
    else if(S_ISSOCK(file_stats->st_mode))type="sock";


    printf("Sciezka bezwgledna do pliku: %s\nRodzaj pliku: %s\nRozmiar w bajtach: %d\n"
           "Data ostatniego dostepu: %s\nData ostatniej modyfikacji: %s\n",
           full_path,type,(int) file_stats->st_size,toString(file_stats->st_atime),toString(file_stats->st_mtime));
}


int nftw_options(const char *full_path,const struct stat *file_stats, int fd, struct FTW *flag){

    if(strcmp(full_path,path_nftw)==0)return 0;


    if(optio[0]=='>'){
        if(difftime(file_stats->st_mtime,mod_time)>0){
            nftw_show(full_path,file_stats);
        }
    }
    else if(optio[0]=='<'){
        if(difftime(file_stats->st_mtime,mod_time)<0){
            nftw_show(full_path,file_stats);
        }
    }
    else if(optio[0]=='='){
        if(difftime(file_stats->st_mtime,mod_time)==0){
            nftw_show(full_path,file_stats);
        }
    }
    return 0;
}
void nftw_search(char *dir_name){
    path_nftw=realpath(dir_name,NULL);
    nftw(path_nftw,nftw_options,10,FTW_PHYS);
}
int main(int argc, char **argv) {
    if(argc!=4){
        printf("Expected 3 arguments");
        exit(1);
    }
    optio=argv[2];
    mod_time=toTime(argv[3]);
    nftw_search(argv[1]);

    return 0;
}