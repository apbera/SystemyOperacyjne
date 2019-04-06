#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int stopped=0;

void sigint_handler(int signum){
    printf("Odebrano sygnal SIGINT\n");
    exit(signum);
}

void sigtstp_handler(int signum){
    if(!stopped)printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
    stopped=!stopped;
}

int main() {

    struct sigaction act;
    act.sa_handler=sigtstp_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;

    signal(SIGINT,sigint_handler);

    sigaction(SIGTSTP,&act,NULL);

    while(1){
        if(!stopped)
        system("date");

        sleep(1);
    }
    return 0;
}