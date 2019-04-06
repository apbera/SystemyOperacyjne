#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include "library.h"


int main(int argc, char *argv[]){

    struct timespec start,stop;
    double duration;
    static struct tms start_cpu;
    static struct tms end_cpu;

    if(clock_gettime(CLOCK_REALTIME, &start)==-1){
        printf("clock_gettime ended with error");
        return 1;
    }

    times(&start_cpu);

    if(argc<2){
        printf("Arguments not given");
        return 1;
    }

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"create_table")==0){
            if(i+1>=argc){
                printf("Not enough parameters for create_table");
                return 1;
            }
            else{
                char *ptr;
                createResult((size_t) strtol(argv[i+1],&ptr,0));
                i++;

            }

        }
        else if(strcmp(argv[i],"search_directory")==0){
            if(i+3>=argc){
                printf("Not enough parameters for search_directory");
                return 1;
            }
            else{
                setDir(argv[i+1]);
                setFileName(argv[i+2]);
                setTmpName(argv[i+3]);
                findAndSaveResults();
                i=i+3;

            }
        }
        else if(strcmp(argv[i],"remove_block")==0){
            if(i+1>=argc){
                printf("Not enough parameters for remove_block");
                return 1;
            }
            else{
                char *ptr;
                deleteBlock((int) strtol(argv[i+1],&ptr,0));
                i++;
            }
        }
        else if(strcmp(argv[i],"add_block")==0){
            tmpToResult();
        }


    }

    times(&end_cpu);

    if(clock_gettime(CLOCK_REALTIME, &stop)==-1){
        printf("clock_gettime ended with error");
        return 1;
    }

    duration=stop.tv_sec-start.tv_sec+(stop.tv_nsec-start.tv_nsec)*1.0/1000000000.0;
    long double user_duration=(long double) (end_cpu.tms_utime-start_cpu.tms_utime)/sysconf(_SC_CLK_TCK);
    long double system_duration=(long double) (end_cpu.tms_stime-start_cpu.tms_stime)/sysconf(_SC_CLK_TCK);
    printf("Real time: %lf s \n",duration);
    printf("User time: %Lf s \n",user_duration);
    printf("System time: %Lf s \n",user_duration);

    FILE *fp=fopen("results3a.txt", "a");
    fprintf(fp, "Real time: %lf s \nUser time: %Lf s \nSystem time: %Lf s\n",duration,user_duration,system_duration);
    fclose(fp);
    freeResult();

    return 0;
}

