#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){

    if(argc<5){
        printf("Not enough arguments\n");
        return 1;
    }

    char *filename=argv[1];
    int pmin=(int)strtol(argv[2],NULL,10);
    int pmax=(int)strtol(argv[3],NULL,10);
    int bytes=(int)strtol(argv[4],NULL,10);
    char *new_line;
    int clock;
    time_t tt;
    clock=time(&tt);
    srand(clock);

    while(1){
        unsigned duration=rand()%(pmax-pmin+1) + pmin;
        sleep(duration);

        char pid[12];
        sprintf(pid,"%d",getpid());

        char dur[12];
        sprintf(dur,"%d",duration);

        char time_s[30];
        time_t curr_time;
        time(&curr_time);
        strftime(time_s,29,"%Y-%m-%d_%H-%M-%S",localtime(&curr_time));

        new_line=malloc(sizeof(char)*(bytes+100));
        strcpy(new_line,pid);
        strcat(new_line," ");
        strcat(new_line,dur);
        strcat(new_line," ");
        strcat(new_line,time_s);
        strcat(new_line," ");
        char n_bytes[bytes+1];
        for(int i=0; i<bytes; i++){
            n_bytes[i]=(char)(rand()%76+48);
        }
        n_bytes[bytes]='\0';
        strcat(new_line,n_bytes);
        strcat(new_line,"\n");

        FILE *fp=fopen(filename,"a");
        if(fp==NULL){
            printf("File not found");
            return 1;
        }

        fwrite(new_line, sizeof(char),strlen(new_line),fp);

        fclose(fp);

        free(new_line);

        printf("Line added\n");
    }


    return 0;
}
