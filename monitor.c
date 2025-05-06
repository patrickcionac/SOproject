#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "treasure.h"

volatile sig_atomic_t stop = 0;

void handle_sigusr1(int sig) {
    FILE* cmdf = fopen(CMD_FILE, "r");
    if (!cmdf) return;

    char cmd[64], arg1[64], arg2[64];
    fscanf(cmdf, "%s %s %s", cmd, arg1, arg2);
    fclose(cmdf);

    if (strcmp(cmd, "list_hunts") == 0) {
        system("ls hunts/");
    } else if (strcmp(cmd, "list_treasures") == 0) {
        list_treasures(arg1);
    } else if (strcmp(cmd, "view_treasure") == 0) {
        // am refolosit lista aici
        char path[128];
        get_treasure_file_path(arg1, path);
        int fd = open(path, O_RDONLY);
        treasure t;
        while (read(fd, &t, sizeof(treasure)) == sizeof(treasure)) {
            if (t.id == atoi(arg2)) {
                print_treasure(&t);
                break;
            }
        }
        close(fd);
    }
    usleep(1000000);
}

void handle_sigterm(int sig) {
    stop = 1;
}

int main() {
    struct sigaction sa1 = {0};
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa2, NULL);

    printf("Monitor started. Waiting for signals...\n");
    while (!stop) {
        pause();
    }
    printf("Monitor exiting.\n");
    return 0;
}
