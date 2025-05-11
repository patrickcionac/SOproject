#define _GNU_SOURCE
#include "treasure_hub.h"
#include "treasure_manager.h"
#include "treasure_manager.c"
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

pid_t monitor_pid = -1;
int monitor_running = 0;
int monitor_stopping = 0;

void send_arg(const char *cmd){
    FILE *f = fopen("cmd.txt" , "a");
    if(!f) exit(-1);
    fprintf(f , "%s\n" , cmd);
    fclose(f);
}

void send_signal(){
    if(monitor_pid != -1)  kill(monitor_pid , SIGUSR1); 
}


void monitor_connect(){
    struct sigaction sa_sigusr1;
    sa_sigusr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_sigusr1.sa_mask); 
    sa_sigusr1.sa_flags = 0; 

    sigaction(SIGUSR1, &sa_sigusr1, NULL);
    while(1){
        pause();
    }
}

void start_monitor(){
    if(monitor_running){
        printf("Monitor is already running.\n");
        return;
    }

    monitor_running = 1;
    monitor_pid = fork();

    
    if(monitor_pid == 0){
        monitor_connect();
        exit(-1);
    }
    else if(monitor_pid > 0){
        printf("Monitor started with PID %d.\n" , monitor_pid);
    }
    else{
        perror("Failed to start monitor.\n");
    }
}


void stop_monitor(){
    if(!monitor_running){
        printf("No monitor running.\n");
        return;
    }
    printf("Monitor will be stopped...\n");
    usleep(100000);
    kill(monitor_pid , SIGTERM);
    monitor_stopping = 1;
    while( monitor_pid != -1 ){
        char cmd[31];
        if(scanf("%s", cmd) == 1){
            printf("Command <%s> couldn't be processed while monitor is stopping\n", cmd);
        }
    }

    int status;
    waitpid(monitor_pid , &status , 0);
    if (WIFEXITED(status)) {
        printf("Monitor exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Monitor terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("Monitor exited abnormally\n");
    }
}

void handle_sigusr1(int sig) {
    FILE* cmdf = fopen("cmd.txt", "r");
    if (!cmdf) return;

    char args1[31];
    char *args2[5];
    int i = 0;
    args2[i] = malloc(strlen("treasure_manager") + 1);
    if( args2[i] == NULL ){
        perror("malloc failed");
        exit(-1);
    }

    strcpy(args2[i++], "treasure_manager");
    while(fgets(args1,sizeof(args1),cmdf) != NULL){
        args1[strcspn(args1, "\n")] = 0;
        args2[i] = malloc(strlen(args1) + 1);
        if( args2[i] == NULL ){
            perror("malloc failed");
            for( int j = i - 1; j >=0 ; j--){
                free(args2[j]);
            }
            exit(-1);
        }
        strcpy(args2[i++], args1);
    }   
    args2[i] = NULL;

    fclose(cmdf); 

    int pid = vfork();
    if(pid == 0 ){
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));

        char exec_path[1024];
        int written = snprintf(exec_path, sizeof(exec_path), "%s/treasure_manager",cwd);
        if (written < 0 || written >= sizeof(exec_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }

        execvp(exec_path,args2);
        perror("exec failed");
        exit(-1);
    }
    else if(pid > 0 ){
        int status;
        waitpid(pid,&status, 0);
    }
    else{
        perror("fork failed");
        exit(-1);
    }
    for (int i = 0; args2[i] != NULL; i++) {
        free(args2[i]);
    }
}

void handle_sigchld(int sig) {
    if(monitor_stopping){
        monitor_pid = -1;
        monitor_stopping = 0;
    }
}

