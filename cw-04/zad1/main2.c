#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>



pid_t child=-1;

void sigint_handler(int signum){
    printf("Odebrano sygnal SIGINT\n");
    if(child!=-1) kill(child,SIGKILL);
    exit(signum);
}

void sigtstp_handler(int signum){
    if(child!=-1){
        kill(child,SIGKILL);
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        child=-1;
    }else{
        child=fork();
    }

    if(child!=0) signal(SIGTSTP, sigtstp_handler);
    return;
}

int main() {

    struct sigaction act;
    act.sa_handler=sigtstp_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;

    signal(SIGINT,&sigint_handler);

    sigaction(SIGTSTP,&act,NULL);

    child=fork();

    while(child!=0){
        sleep(1);
    }

    if(child==0){
        execlp("./dateprint","./dateprint",NULL);
    }

    return 0;
}
