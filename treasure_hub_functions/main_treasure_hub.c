#include "treasure_hub_header.h"
#include "treasure_manager_header.h"
#include "treasure_hub.c"
#include <errno.h>
#include <dirent.h>

int main() {
    setvbuf(stdout, NULL, _IOLBF, 0);  // buffer-uim linia pentru safety
    setvbuf(stderr, NULL, _IOLBF, 0);  // la fel si pt erori

    char cmd[101];

    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        usleep(10000);
        printf(">> ");
        fflush(stdout);

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(cmd, "list_hunts") == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
                   
            unlink("cmd.txt");
            write_args_to_cmd(1, "--list_hunts");
            send_signal();
            read_pipe_output();
        } else if (strcmp(cmd, "list_treasures") == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
                 
            unlink("cmd.txt");
            char hunt[20];
            printf("Please enter the hunt: ");
            scanf("%19s", hunt); getchar();
            write_args_to_cmd(2, "--list", hunt);
            send_signal();
            read_pipe_output();
        } else if (strcmp(cmd, "view_treasure") == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
                    
            unlink("cmd.txt");
            char hunt_id[31], treasure_id[31];
            printf("Hunt ID: ");
            scanf("%30s", hunt_id); getchar();
            printf("Treasure ID: ");
            scanf("%30s", treasure_id); getchar();
            write_args_to_cmd(3, "--view", hunt_id, treasure_id);
            send_signal();
            read_pipe_output();
        } else if (strcmp(cmd, "calculate_score") == 0) {
            if (!monitor_running) {
                printf("Monitor not running.\n");
                continue;
            }
                      
            unlink("cmd.txt");
            write_args_to_cmd(1, "--calculate_score");
            send_signal();
            read_pipe_output();
        } else if (strcmp(cmd, "stop_monitor") == 0) {
            unlink("cmd.txt");
            stop_monitor();
        } else if (strcmp(cmd, "exit") == 0) {
            if (monitor_running) {
                printf("ERR: Monitor still running\n");
            } else {
                break;
            }
        } else if (strcmp(cmd, "clear") == 0) {
            system("clear");
        } else {
            printf("Unknown command\n");
        }
    }

    return 0;
}
