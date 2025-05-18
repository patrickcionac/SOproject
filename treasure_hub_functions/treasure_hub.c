#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>

pid_t monitor_pid = -1;
int monitor_running = 0;
int monitor_stopping = 0;
int pipe_fd[2]; //am declarat pipe-ul global pt a fi folosit in integralitatea procesului.
int sigusr1_received = 0;

char *args2[10];

void parse_args_from_cmd() { //am facut o functie auxiliara care preia comenzile din cmd.txt.
    FILE *f = fopen("cmd.txt", "r");
    if (!f) {
        perror("cmd.txt open failed");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), f) && i < 9) {
        line[strcspn(line, "\n")] = 0;
        args2[i] = strdup(line);
        i++;
    }
    args2[i] = NULL;
    fclose(f);
}

void send_arg(const char *cmd) { //pana la urma nu am mai folosit aceasta functie , am pastrat o versiune mai potrivita mai jos , si anume write_args_to_cmd.
    FILE *f = fopen("cmd.txt", "w");
    if (!f) exit(EXIT_FAILURE);
    fprintf(f, "%s\n", cmd);
    fclose(f);
}

void write_args_to_cmd(int count, ...) {
    FILE* f = fopen("cmd.txt", "w");
    if (!f) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        char* arg = va_arg(args, char*);
        fprintf(f, "%s\n", arg);
    }
    va_end(args);
    fclose(f);
}

void monitor_connect() {
    parse_args_from_cmd();  
    for (int i = 0; args2[i]; i++) {
        printf("args2[%d] = %s\n", i, args2[i]); //in procesul de debugging am facut for ul asta sa verific ca mi pune argumentele cum trebuie.
    }

    int local_pipe[2];
    if (pipe(local_pipe) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0) { //procesul "copil".
        close(local_pipe[0]);
        dup2(local_pipe[1], STDOUT_FILENO);
        dup2(local_pipe[1], STDERR_FILENO);
        close(local_pipe[1]);

        char *tm_args[11]; 
        tm_args[0] = "./tm";
        int i = 0;
        while (i < 9 && args2[i]) {
            tm_args[i + 1] = args2[i];
            i++;
        }
        tm_args[i + 1] = NULL;
    
        execv("./tm", tm_args);
        perror("exec failed");

        dprintf(STDOUT_FILENO, "execv failed for ./tm\n@@END@@\n");
        exit(1);
    } else if (pid < 0) {
        perror("fork failed");
        return;
    }

    // aici "parinte"
    close(local_pipe[1]);

    char buffer[1024];
    ssize_t n;


    waitpid(pid, NULL, 0);

    while ((n = read(local_pipe[0], buffer, sizeof(buffer))) > 0) {
        if (write(pipe_fd[1] , buffer, n) == -1) {
            perror("write to pipe_fd failed");
            break;
        }
    }
    close(local_pipe[0]);

    const char *end_marker = "@@END@@\n"; // aici am facut un fel de debug , ca sa verific ca se scrie in pipe prin mesajul asta.
    write(pipe_fd[1], end_marker, strlen(end_marker));
}


void sigusr1_handler(int sig) {
    sigusr1_received = 1;  
}

void handle_sigchld(int sig) {
    int saved_errno = errno;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            if (monitor_stopping) {
                monitor_stopping = 0;
            }
            monitor_pid = -1;
        }
    }

    errno = saved_errno;
}


void start_monitor() {
    if (monitor_running) {
        printf("Monitor is already running.\n");
        return;
    }

    if (pipe(pipe_fd) == -1) { //aici creez pipe-ul
        perror("pipe failed");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid == 0) {
        signal(SIGUSR1, sigusr1_handler); 
        while (1) {
            pause();  
            if (sigusr1_received) {
                sigusr1_received = 0;
                monitor_connect();
            }
        }
    } else if (monitor_pid > 0) {
        monitor_running = 1;
        printf("Monitor started with PID %d.\n", monitor_pid);
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

void stop_monitor() {
    if (!monitor_running) {
        printf("No monitor running.\n");
        return;
    }
    printf("Monitor will be stopped...\n");
    usleep(100000);

    monitor_stopping = 1;
    kill(monitor_pid, SIGTERM);
    waitpid(monitor_pid, NULL , 0);
    monitor_running = 0;

    close(pipe_fd[0]);
    close(pipe_fd[1]);
   printf("Monitor stopped.\n");

}

void send_signal() {
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGUSR1);
    }
}

void read_pipe_output() {
    char buffer[1024];
    ssize_t n;
    

    while ((n = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        if (strstr(buffer, "@@END@@") != NULL) { //mesajul anterior : nu intru in conditie daca il gasesc.
            char *end_pos = strstr(buffer, "@@END@@");
            *end_pos = '\0';  
            printf("%s", buffer);
            break;
        }
        printf("%s", buffer);
    }

}



