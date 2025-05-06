#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include "treasure.h"

pid_t monitor_pid = -1;
int monitor_running = 0;

void send_signal(const char *cmd){
    FILE *f = fopen(CMD_FILE , "w");
    if(!f) return;
    fprintf(f , "%s\n" , cmd);
    fclose(f);
    kill(monitor_pid , SIGUSR1);
}

void start_monitor(){
    if(monitor_running){
        printf("Monitor is already running.\n");
        return;
    }

    monitor_pid = fork();

    
    if(monitor_pid == 0){
        execl("./monitor" , "monitor" , NULL);
        perror("Execution failed");
        exit(-1);
    }
    else{
        monitor_running = 1;
        printf("Monitor started with PID %d.\n" , monitor_pid);
    }
}

void stop_monitor(){
    if(!monitor_running){
        printf("No monitor running.\n");
        return;
    }

    kill(monitor_pid , SIGTERM);
    waitpid(monitor_pid , NULL , 0);
    monitor_running = 0;
    printf("Sent stop signal to monitor (%d) \n" , monitor_pid);
}


int main(int argc , char **argv){
    char command[128];
   

    while(1){
        printf("> ");
        fflush(stdout);

        if(!fgets(command , sizeof(command) , stdin)) {
            printf("Error reading input.\n");
            break;
        }

        command[strcspn(command , "\n")] = 0;

        if(strcmp(command , "start_monitor") == 0){
            if(monitor_running){
                printf("Monitor already running.\n");
            }
            else{
                start_monitor();
                printf("Monitor started.\n");
            }
        }
        else if (strncmp(command, "list_hunts", 10) == 0 ||
                 strncmp(command, "list_treasures", 14) == 0 ||
                 strncmp(command, "view_treasure", 13) == 0) {

             if (!monitor_running) {
              printf("Monitor not running.\n");
              continue;
            }
              send_signal(command);
        }
        else if (strcmp(command, "stop_monitor") == 0) {
            if (!monitor_running) {
                printf("Monitor is not running.\n");
            } else {
                // TODO: Signal monitor to stop
                printf("Monitor stopping...\n");
                stop_monitor();
            }
        }
        else if(strcmp(command , "exit") == 0){
            if(monitor_pid > 0){
                printf("Cannot exit: monitor still running.\n");
            }
            else{
                printf("Exiting hub.\n");
                break;
            }
        }
        else{
            printf("Command '%s' unknown." , command);
        }
    }
    return 0;
}