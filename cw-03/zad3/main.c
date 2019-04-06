#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <libgen.h>
#include <sys/resource.h>

enum cp_type{
    CP,
    MEM
};
struct to_monitor{
    char path[4096];
    int seconds;
};

struct all_files{
    struct to_monitor *files;
    size_t size;
};

struct file_content{
    char *content;
    size_t size;
};

struct monitor_results{
    pid_t pid;
    int copies;
};
size_t numberOfLines(char *list_path){
    FILE *fp=fopen(list_path,"r");

    if(fp==NULL){
        printf("Couldn't find file to open");
        exit(1);
    }

    char *line=NULL;
    size_t size;
    size_t lines=0;

    while(getline(&line,&size,fp)!=-1){
        lines++;
    }
    fclose(fp);
    return lines;
}

struct to_monitor parsed_line(char *line){

    struct to_monitor parsed;
    char *tmp;
    char *seconds;
    tmp=strtok(line,",");
    seconds=strtok(NULL,",");
    strcpy(parsed.path,tmp);
    parsed.seconds=(int)strtol(seconds,NULL,10);

    if(parsed.seconds<0){
        printf("Time should be positive");
        exit(1);
    }

    return parsed;
}

struct all_files file_to_memory(char *list_path){

    FILE *fp=fopen(list_path,"r");

    if(fp==NULL){
        printf("Couldn't find file to open");
        exit(1);
    }

    struct all_files files_array;
    files_array.size=numberOfLines(list_path);
    files_array.files=malloc(sizeof(struct to_monitor) * files_array.size);

    char *line=NULL;
    size_t size;
    int i=0;
    while(getline(&line,&size,fp)!=-1){
        files_array.files[i]=parsed_line(line);
        i++;
    }

    return files_array;
}

void create_archive(){
    struct stat tmp;
    if (!(stat("archiwum", &tmp) == 0 && S_ISDIR(tmp.st_mode))) {
        errno = 0;
        mkdir("archiwum", ACCESSPERMS);
        if (errno != 0) {
            printf("Problem with creating dir");
            exit(1);
        }
    }
}

void to_archive(char *path, struct stat file_info, struct file_content *Content){
    char archiwum_path[4096];
    char date[30];

    strftime(date,29,"_%Y-%m-%d_%H-%M-%S",localtime(&file_info.st_mtime));

    strcpy(archiwum_path,"archiwum/");
    strcat(archiwum_path,basename(path));
    strcat(archiwum_path,date);

    FILE *s_file=fopen(archiwum_path,"w");
    if(s_file==NULL){
        printf("Failed to make archive file");
        exit(1);
    }

    fwrite(Content->content, sizeof(char), Content->size, s_file);

    fclose(s_file);

    free(Content->content);
    Content->content=NULL;
}

struct file_content copy_mem(struct to_monitor watch){
    FILE *to_copy=fopen(watch.path,"r");
    if(to_copy==NULL){
        printf("Problem with opening the file");
        exit(1);
    }

    if(fseek(to_copy,0,SEEK_END)!=0){
        printf("Failed to seek");
        exit(-1);
    }
    size_t size=(size_t)ftell(to_copy);

    fseek(to_copy,0,SEEK_SET);

    struct file_content tmp;

    tmp.size=size;

    tmp.content=malloc(sizeof(char)*size);

    fread(tmp.content, sizeof(char),size,to_copy);

    return  tmp;
}

void copy_cp(char *path, struct stat file_info){
    pid_t child=fork();
    if(child==0){
        char archiwum_path[4096];
        char date[30];

        strftime(date,29,"_%Y-%m-%d_%H-%M-%S",localtime(&file_info.st_mtime));
        strcpy(archiwum_path,"archiwum/");
        strcat(archiwum_path,basename(path));
        strcat(archiwum_path,date);

        execlp("cp","cp",path,archiwum_path,NULL);

    } else{
        int res;
        wait(&res);
    }
}

int monitor_mem(struct to_monitor watch, time_t max_time){
    time_t start;
    time_t curr;
    time(&start);
    time(&curr);
    time_t beg_check=0;
    struct stat file_info;
    lstat(watch.path,&file_info);
    struct stat curr_file_info;
    int copies=0;
    struct file_content CONTENT=copy_mem(watch);

    while(difftime(curr,start)<max_time){
        time(&beg_check);
        if(lstat(watch.path,&curr_file_info)==-1){
            perror(watch.path);
            exit(-1);
        }

        if(difftime(file_info.st_mtime,curr_file_info.st_mtime)<0){
            to_archive(watch.path,file_info,&CONTENT);
            file_info=curr_file_info;
            CONTENT=copy_mem(watch);
            copies++;
        }

        time(&curr);
        sleep((unsigned)(watch.seconds - difftime(curr, beg_check)));
        time(&curr);
    }
    return copies;
}

int monitor_cp(struct to_monitor watch, time_t max_time){
    time_t start;
    time_t curr;
    time(&start);
    time(&curr);
    time_t beg_check=0;
    struct stat file_info;
    lstat(watch.path,&file_info);
    struct stat curr_file_info;
    int copies=0;

    while(difftime(curr,start)<max_time){
        time(&beg_check);
        if(lstat(watch.path,&curr_file_info)==-1){
            perror(watch.path);
            exit(-1);
        }

        if(difftime(file_info.st_mtime,curr_file_info.st_mtime)<0){
            copy_cp(watch.path,file_info);
            file_info=curr_file_info;
            copies++;
        }

        time(&curr);
        sleep((unsigned)(watch.seconds - difftime(curr, beg_check)));
        time(&curr);
    }
    return copies;
}

int main(int argc, char **argv) {

    if(argc<6){
        printf("Expected 5 arguments\n");
        exit(1);
    }

    create_archive();

    time_t max_time=(int)strtol(argv[2],NULL,10);

    if(max_time<0){
        printf("Time should be positive");
        return 1;
    }

    rlim_t cpu_limit=(rlim_t)strtol(argv[4],NULL,10);

    rlim_t mem_limit=(rlim_t)strtol(argv[5],NULL,10);

    if(cpu_limit<0){
        printf("Cpu limit should be positive");
        return 1;
    }

    if(mem_limit<0){
        printf("Memory limit should be positive");
        return 1;
    }

    struct rlimit cpu;
    cpu.rlim_cur=cpu_limit;
    cpu.rlim_max=cpu_limit;
    struct rlimit mem;
    mem.rlim_cur=mem_limit*1024*1024;
    mem.rlim_max=mem_limit*1024*1024;

    struct all_files files_array = file_to_memory(argv[1]);

    if(strcmp(argv[3],"mem")==0){
        for(int i=0; i<files_array.size; i++){
            pid_t child=fork();

            if(child==0){
                setrlimit(RLIMIT_AS,&mem);
                setrlimit(RLIMIT_CPU,&cpu);

                return monitor_mem(files_array.files[i],max_time);
            }
        }
    }
    else if(strcmp(argv[3],"cp")==0){
        for(int i=0; i<files_array.size; i++){
            pid_t child=fork();

            if(child==0){
                setrlimit(RLIMIT_AS,&mem);
                setrlimit(RLIMIT_CPU,&cpu);
                return monitor_cp(files_array.files[i],max_time);
            }
        }
    }
    else {
        printf("expected 3rd argument: cp or mem");
        return 0;
    }

    struct monitor_results results[files_array.size];

    for (int i=0; i < files_array.size; i++) {
        struct rusage usageBefore;
        struct rusage usageAfter;

        if(getrusage(RUSAGE_CHILDREN,&usageBefore)==-1){
            printf("Problem with getrusage");
            exit(-1);
        }

        results[i].pid = wait(&results[i].copies);
        results[i].copies = WEXITSTATUS(results[i].copies);

        if(getrusage(RUSAGE_CHILDREN,&usageAfter)==-1){
            printf("Problem with getrusage");
            exit(-1);
        }

        if (results[i].copies == -1)printf("PID: %i\tError\n", results[i].pid);
        else {
            printf("PID: %i\tModifications: %i\t"
                   "User Time: %ld.%06ld\tSystem Time: %ld.%06ld\n", results[i].pid, results[i].copies,
                   usageAfter.ru_utime.tv_sec - usageBefore.ru_utime.tv_sec,
                   usageAfter.ru_utime.tv_usec - usageBefore.ru_utime.tv_usec,
                   usageAfter.ru_stime.tv_sec - usageBefore.ru_stime.tv_sec,
                   usageAfter.ru_stime.tv_usec - usageBefore.ru_stime.tv_usec);
        }
    }


    return 0;
}