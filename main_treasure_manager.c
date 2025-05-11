#include "treasure_manager.h"
#include "treasure_manager.c"

int main(int argc, char** argv) {
    
    char cwd[1024];
    getcwd(cwd , sizeof(cwd));
    char cwd_nou[1024];
    int written = snprintf(cwd_nou , sizeof(cwd_nou) , "%s%s" , cwd , "/Hunts/");
    if (written < 0 || written >= sizeof(cwd_nou)) {
        perror("Path too long\n");
        exit(-1);
    }  

    if (argc > 1) {
        if (strcmp(argv[1], "--add") == 0) {
            struct stat path_stat;  
            if (stat(argv[2], &path_stat) == 0) {
                if (S_ISDIR(path_stat.st_mode)) {
                    create(argv[2]);
                }
            }
            else {
 
                mkdir(argv[2], 0777);
                create(argv[2]);
            }
        }
        else if (strcmp(argv[1], "--list") == 0) {
           if(argc > 2){
                struct stat path_stat;
            if (stat(argv[2], &path_stat) == 0) {
                if (S_ISDIR(path_stat.st_mode)) {
                    list(argv[2]);
                }
            }
            else {
 
                mkdir(argv[2], 0777);
                list(argv[2]);
            }
            } 
        }
        else if (strcmp(argv[1], "--view") == 0) {
            if(argc > 3){
                view(argv[2] , argv[3]);
            }
            else{
                printf("Usage: --view <huntID> <treasureID>");
            }
        }
        else if (strcmp(argv[1], "--remove_treasure") == 0) {
            if(argc > 3){
                remove_treasure(argv[2] , argv[3]);
            }
            else{
                printf("Usage: --remove_treasure <huntID> <treasureID>");
            }
        }
        else if (strcmp(argv[1], "--remove_hunt") == 0) {
            if(argc > 2){
                struct stat path_stat;
            if (stat(argv[2], &path_stat) == 0) {
                if (S_ISDIR(path_stat.st_mode)) {
                    remove_hunt(argv[2]);
                }
            }
            else {
 
                printf("Hunt '%s' does not exist.\n" , argv[2]);
                }
            }
            else{
                printf("Usage: --remove_treasure <huntID> <treasureID>");
            }

        }
 
    }
    else if(strcmp(argv[1] , "--list_hunts") == 0){
        list_hunts(cwd_nou);
    }
    else {
        printf("Usage: treasure_manager --opt");
    }

    return 0;
}