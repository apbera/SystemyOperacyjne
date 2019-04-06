#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>

void generate(char *fileName,int recordsNum,size_t size){
    char *block=malloc(size);
    int out, clock;
    time_t tt;
    clock=time(&tt);
    srand(clock);

    out=open(fileName,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(errno!=0){
        perror("Opening file failed");
    }
    for(int i=0;i<recordsNum; i++){
        for(int j=0;j<size;j++){
            block[j]=(char) (rand()%76+48);
        }
        write(out,block, size);
    }
    free(block);
}
void sort_sys(char *fileName,int recordsNum,size_t size){
    char *record1=malloc(size);
    char *record2=malloc(size);
    char c;
    char current;
    int min;

    int file=open(fileName,O_RDWR);
    if(errno!=0){
        perror("Opening file failed");
    }

    int length=lseek(file,0,SEEK_END);
    if(errno!=0){
        perror("Seeking end of file failed");
    }

    if(length-(recordsNum*size)<0){
        printf("File is too small");
        exit(1);
    }

    for(int i=0; i<recordsNum; i++){
        min=i;
        if(lseek(file,i*size,SEEK_SET)==-1){
            printf("Seeking file failed");
            exit(1);
        }
        if(read(file,&c, sizeof(char))==-1){
            printf("Reading file failed");
            exit(1);
        }
        for(int j=i+1; j<recordsNum; j++){
            if(lseek(file,j*size,SEEK_SET)==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(read(file,&current, sizeof(char))==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(current<c){
                min=j;
                c=current;
            }
        }
        if(min!=i){
            if(lseek(file,i*size,SEEK_SET)==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(read(file,record1,size)==-1){
                printf("Seeking file failed");
                exit(1);
            }

            if(lseek(file,min*size,SEEK_SET)==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(read(file,record2,size)==-1){
                printf("Seeking file failed");
                exit(1);
            }

            if(lseek(file,i*size,SEEK_SET)==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(write(file,record2,size)==-1){
                printf("Seeking file failed");
                exit(1);
            }

            if(lseek(file,min*size,SEEK_SET)==-1){
                printf("Seeking file failed");
                exit(1);
            }
            if(write(file,record1,size)==-1){
                printf("Seeking file failed");
                exit(1);
            }
        }
    }
    free(record1);
    free(record2);
    close(file);
}
void sort_lib(char *fileName,int recordsNum,size_t size){
    char *record1=malloc(size);
    char *record2=malloc(size);
    char c;
    char current;
    int min;

    FILE *file=fopen(fileName,"r+");
    if(file==NULL){
        printf("File not found");
        exit(1);
    }
    fseek(file,0,SEEK_END);

    if(ftell(file)-(recordsNum*size)<0){
        printf("File is too small");
        exit(1);
    }

    for(int i=0; i<recordsNum; i++){
        min=i;
        fseek(file,i*size,SEEK_SET);
        c=(char) getc(file);
        for(int j=i+1; j<recordsNum; j++){
            fseek(file,j*size,SEEK_SET);
            current=(char) getc(file);
            if(current<c){
                min=j;
                c=current;
            }
        }
        if(min!=i){
            fseek(file,i*size,SEEK_SET);
            fread(record1, sizeof(char),size,file);

            fseek(file,min*size,SEEK_SET);
            fread(record2, sizeof(char),size,file);

            fseek(file,i*size,SEEK_SET);
            fwrite(record2, sizeof(char),size,file);

            fseek(file,min*size,SEEK_SET);
            fwrite(record1, sizeof(char),size,file);
        }
    }
    free(record1);
    free(record2);
    fclose(file);
}
void copy_sys(char *from, char *to, int recordsNum, size_t size){
    char *block=malloc(size);
    int in, out, licz;

    in=open(from,O_RDONLY);
    if(errno!=0){
        perror("Opening file failed");
    }
    out=open(to,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(errno!=0){
        perror("Opening file failed");
    }

    int i=0;

    while((licz=read(in,block, size))>0 && i<recordsNum){
    write(out,block,licz);
    i++;
    }
    free(block);
    close(in);
    close(out);
}
void copy_lib(char *from, char *to, int recordsNum, size_t size){
    char *block=malloc(size);
    FILE *copied=fopen(from,"r");
    FILE *newFile=fopen(to, "w");
    int licz;
    int i=0;

    while((licz=fread(block, sizeof(char),size,copied))>0 && i<recordsNum){
        fwrite(block, sizeof(char),licz,newFile);
        i++;
    }
    free(block);
    fclose(copied);
    fclose(newFile);
}

void tests(){
    struct timespec start, stop;
    double duration;
    long double user_duration;
    long double system_duration;
    static struct tms start_test;
    static struct tms end_test;
    FILE *fp=fopen("wyniki.txt", "a");
    int recordsNum[]={1000,4000};
    size_t sizes[]={1, 4, 512, 1024, 4096, 8192};

    for(int i=0;i<6;i++){
        for(int j=0;j<2;j++){

            generate("test1.txt",recordsNum[j],sizes[i]);

            if(clock_gettime(CLOCK_REALTIME, &start)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&start_test);
            copy_sys("test1.txt","copied1.txt",recordsNum[j],sizes[i]);
            if(clock_gettime(CLOCK_REALTIME, &stop)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&end_test);
            duration=stop.tv_sec-start.tv_sec+(stop.tv_nsec-start.tv_nsec)*1.0/1000000000.0;
            user_duration=(long double) (end_test.tms_utime-start_test.tms_utime)/sysconf(_SC_CLK_TCK);
            system_duration=(long double) (end_test.tms_stime-start_test.tms_stime)/sysconf(_SC_CLK_TCK);
            fprintf(fp, "COPY_SYS: Liczba rekordow = %d, Rozmiar rekordu = %d\nReal time: %lf s \nUser time: %Lf s \nSystem time: "
                        "%Lf s\n",recordsNum[j],(int) sizes[i],duration,user_duration,system_duration);

            if(clock_gettime(CLOCK_REALTIME, &start)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&start_test);
            copy_lib("test1.txt","copied2.txt",recordsNum[j],sizes[i]);
            if(clock_gettime(CLOCK_REALTIME, &stop)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&end_test);
            duration=stop.tv_sec-start.tv_sec+(stop.tv_nsec-start.tv_nsec)*1.0/1000000000.0;
            user_duration=(long double) (end_test.tms_utime-start_test.tms_utime)/sysconf(_SC_CLK_TCK);
            system_duration=(long double) (end_test.tms_stime-start_test.tms_stime)/sysconf(_SC_CLK_TCK);
            fprintf(fp, "COPY_LIB: Liczba rekordow = %d, Rozmiar rekordu = %d\nReal time: %lf s \nUser time: %Lf s \nSystem time: "
                        "%Lf s\n",recordsNum[j],(int) sizes[i],duration,user_duration,system_duration);

            if(clock_gettime(CLOCK_REALTIME, &start)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&start_test);
            sort_sys("copied1.txt",recordsNum[j],sizes[i]);
            if(clock_gettime(CLOCK_REALTIME, &stop)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&end_test);
            duration=stop.tv_sec-start.tv_sec+(stop.tv_nsec-start.tv_nsec)*1.0/1000000000.0;
            user_duration=(long double) (end_test.tms_utime-start_test.tms_utime)/sysconf(_SC_CLK_TCK);
            system_duration=(long double) (end_test.tms_stime-start_test.tms_stime)/sysconf(_SC_CLK_TCK);
            fprintf(fp, "SORT_SYS: Liczba rekordow = %d, Rozmiar rekordu = %d\nReal time: %lf s \nUser time: %Lf s \nSystem time: "
                        "%Lf s\n",recordsNum[j],(int) sizes[i],duration,user_duration,system_duration);

            if(clock_gettime(CLOCK_REALTIME, &start)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&start_test);
            sort_lib("copied2.txt",recordsNum[j],sizes[i]);
            if(clock_gettime(CLOCK_REALTIME, &stop)==-1){
                printf("clock_gettime ended with error");
                exit(1);
            }
            times(&end_test);
            duration=stop.tv_sec-start.tv_sec+(stop.tv_nsec-start.tv_nsec)*1.0/1000000000.0;
            user_duration=(long double) (end_test.tms_utime-start_test.tms_utime)/sysconf(_SC_CLK_TCK);
            system_duration=(long double) (end_test.tms_stime-start_test.tms_stime)/sysconf(_SC_CLK_TCK);
            fprintf(fp, "SORT_LIB: Liczba rekordow = %d, Rozmiar rekordu = %d\nReal time: %lf s \nUser time: %Lf s \nSystem time: "
                        "%Lf s\n",recordsNum[j],(int) sizes[i],duration,user_duration,system_duration);
        }
    }
    fclose(fp);
}

int main(int argc, char **argv) {

    if(argc<2){
        printf("Not enough arguments");
        exit(1);
    }

    int recordsNum;
    size_t size;
    char *tmp;

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"generate")==0){
            if(i+3>=argc){
                printf("Not enough arguments for generate command");
                exit(1);
            }

            recordsNum=(int) strtol(argv[i+2],&tmp,0);
            size=(size_t) strtol(argv[i+3],&tmp,0);

            generate(argv[i+1],recordsNum,size);

            i+=3;
        }
        else if(strcmp(argv[i],"sort")==0){
            if(i+4>=argc){
                printf("Not enough arguments for sort command");
                exit(1);
            }

            recordsNum=(int) strtol(argv[i+2],&tmp,0);
            size=(size_t) strtol(argv[i+3],&tmp,0);

            if(strcmp(argv[i+4],"sys")==0){
                sort_sys(argv[i+1],recordsNum,size);
            }
            else if(strcmp(argv[i+4],"lib")==0){
                sort_lib(argv[i+1],recordsNum,size);
            }

        }
        else if(strcmp(argv[i],"copy")==0){
            if(i+5>=argc){
                printf("Not enough arguments for copy command");
                exit(1);
            }

            recordsNum=(int) strtol(argv[i+3],&tmp,0);
            size=(size_t) strtol(argv[i+4],&tmp,0);

            if(strcmp(argv[i+5],"sys")==0){

                copy_sys(argv[i+1],argv[i+2],recordsNum,size);
            }
            else if(strcmp(argv[i+5],"lib")==0){
                copy_lib(argv[i+1],argv[i+2],recordsNum,size);
            }

        }
        else if(strcmp(argv[i],"test")==0){
            tests();
        }
    }

    return 0;
}