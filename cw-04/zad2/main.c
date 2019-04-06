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
#include <signal.h>

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

int killed=0;
int stopped=0;
pid_t *children_pid;
size_t children_size;

void kill_process(){
    for(int i=0;i<children_size;i++){
        kill(children_pid[i],SIGTSTP);
    }

    for(int i=0;i<children_size;i++){
        int st;
        waitpid(children_pid[i],&st,0);
        printf("PID: %i\tModifications: %i\n", children_pid[i], st/256);
    }
    free(children_pid);
    exit(0);
}

void sigusr1(int num){
    stopped=1;

}

void sigusr2(int num){
    stopped=0;

}

void sigtstp(int num){
    killed=1;
}

void sigint(int num){
    kill_process();
}

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

int monitor_mem(struct to_monitor watch){
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


    while(!killed) {
        time(&beg_check);
        if (!stopped) {
            if (lstat(watch.path, &curr_file_info) == -1) {
                perror(watch.path);
                exit(-1);
            }

            if (difftime(file_info.st_mtime, curr_file_info.st_mtime) < 0) {
                to_archive(watch.path, file_info, &CONTENT);
                file_info = curr_file_info;
                CONTENT = copy_mem(watch);
                copies++;
            }
        }
        time(&curr);
        sleep((unsigned)(watch.seconds - difftime(curr, beg_check)));
        time(&curr);
    }
    exit(copies);
}

int main(int argc, char **argv) {

    if(argc<2){
        printf("Expected 1 arguments\n");
        exit(1);
    }

    create_archive();

    struct all_files files_array = file_to_memory(argv[1]);

    children_size=files_array.size;
    children_pid=calloc(files_array.size, sizeof(pid_t));

    for(int i=0; i<files_array.size; i++){
        pid_t child=fork();
        children_pid[i]=child;
        if(child==0){
            signal(SIGUSR1,sigusr1);
            signal(SIGUSR2,sigusr2);
            signal(SIGTSTP,sigtstp);
            killed=0;
            stopped=0;

            monitor_mem(files_array.files[i]);
        }
        printf("%d %s\n",child,files_array.files->path);
    }

    signal(SIGINT,sigint);

    char *input=calloc(128, sizeof(char));

    while(1){
        scanf("%s",input);
        if(strcmp(input,"LIST")==0){
            for(int i=0;i<files_array.size;i++){
                printf("%d, ",children_pid[i]);
            }
            printf("\n");
        } else if(strcmp(input,"END")==0){
            kill_process();
        } else{
            if(strcmp(input,"STOP")==0){
                scanf("%s",input);
                if(strcmp(input,"ALL")==0){
                    for(int i=0;i<files_array.size;i++){
                        kill(children_pid[i],SIGUSR1);
                    }
                }else {
                    int num=atoi(input);
                    kill(num,SIGUSR1);
                }
            }
            else if(strcmp(input,"START")==0){
                scanf("%s",input);
                if(strcmp(input,"ALL")==0){
                    for(int i=0;i<files_array.size;i++){
                        kill(children_pid[i],SIGUSR2);
                    }
                }else {
                    int num=atoi(input);
                    kill(num,SIGUSR2);
                }
            }
        }
    }

    return 0;
}