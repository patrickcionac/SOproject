#include "treasure_manager_header.h"
#include "treasure_manager.c"


void get_hunt_path(const char *hunt_name, char *path) {
    snprintf(path, 128, "Hunts/%s", hunt_name);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: treasure_manager --option [args...]\n");
        return 1;
    }

    if (strcmp(argv[1], "--add") == 0) {
        if (argc < 3) {
            printf("Usage: --add <huntID>\n");
            return 1;
        }

        char path[128];
        get_hunt_path(argv[2], path);
        struct stat path_stat;
        if (stat(path, &path_stat) != 0) {
            mkdir(path, 0777);
        }
        create(argv[2]);

    } else if (strcmp(argv[1], "--list") == 0) {
        if (argc < 3) {
            printf("Usage: --list <huntID>\n");
            return 1;
        }

        char path[128];
        get_hunt_path(argv[2], path);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            list(argv[2]);
        } else {
            printf("Hunt '%s' does not exist.\n", argv[2]);
        }

    } else if (strcmp(argv[1], "--view") == 0) {
        if (argc < 4) {
            printf("Usage: --view <huntID> <treasureID>\n");
            return 1;
        }

        char path[128];
        get_hunt_path(argv[2], path);
        view(argv[2], argv[3]);

    } else if (strcmp(argv[1], "--remove_treasure") == 0) {
        if (argc < 4) {
            printf("Usage: --remove_treasure <huntID> <treasureID>\n");
            return 1;
        }

        char path[128];
        get_hunt_path(argv[2], path);
        remove_treasure(argv[2], argv[3]);

    } else if (strcmp(argv[1], "--remove_hunt") == 0) {
        if (argc < 3) {
            printf("Usage: --remove_hunt <huntID>\n");
            return 1;
        }

        char path[128];
        get_hunt_path(argv[2], path);
        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
            remove_hunt(argv[2]);
        } else {
            printf("Hunt '%s' does not exist.\n", argv[2]);
        }

    } else if (strcmp(argv[1], "--list_hunts") == 0) {
        char hunts_path[256];
        getcwd(hunts_path , sizeof(hunts_path)); //luam current working directory-ul aici
        int check = snprintf(hunts_path + strlen(hunts_path), 
        sizeof(hunts_path) - strlen(hunts_path), 
        "/Hunts"); // aici am alocat pentru hunt , tinand cont ca orice hunt se afla in directory-ul Hunts , creat de mine.

          if (check < 0 || check >= sizeof(hunts_path) - strlen(hunts_path)) {
            fprintf(stderr, "Hunts path too long\n");
            return 1;
        }

        list_hunts(hunts_path);
    } 
    else if (strcmp(argv[1], "--calculate_score") == 0) {
        calculate_score();
    }    
    else {
        printf("Unknown option: %s\n", argv[1]);
        printf("Available options: --add, --list, --view, --remove_treasure, --remove_hunt, --list_hunts\n");
        return 1;
    }

    return 0;
}

