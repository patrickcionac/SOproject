#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

typedef struct {
    char id[10];
    char user[25];
    double lat, lon;
    char clue[20];
    int value;
} treasure;
 

void log_updates(char huntID[] , char action[]){
    static char path[128];
    snprintf(path , sizeof(path) , "%s/%s" , huntID , "logged_hunt");
 
    int file = open(path , O_WRONLY | O_CREAT | O_APPEND , 0777);

    time_t now = time(NULL);
    dprintf(file , "%s : %s\n" , ctime(&now) , action);
    close(file);
    //with " cat "file_name" " command in the terminal you can see the updates of the function
}

void create(char* hunt) {
 
    treasure t;
    printf("Enter data:\n");
 
    printf("Treasure ID:");
    scanf("%s", t.id);
 
    //getchar();
    printf("Username:");
    scanf("%s", t.user);
 
    printf("Latitude:");
    scanf("%lf", &t.lat);
 
    printf("Longitude:");
    scanf("%lf", &t.lon);
 
    printf("Clue:");
    getchar();
    fgets(t.clue , 20 , stdin);
    t.clue[strlen(t.clue) - 1] = '\0';
 
    printf("Value:");
    scanf("%d", &t.value);
 
 
 
    static char path[128];
    snprintf(path , sizeof(path) , "%s/%s" , hunt , "treasure.bin");
 
    int file = open(path , O_WRONLY | O_CREAT | O_APPEND , 0777);
   
    //int write(int fileDescriptor, void *buffer, size_t bytesToWrite)
 
    if(write(file , &t , sizeof(treasure)) != sizeof(treasure)){
        printf("Error");
        //exit(-1);
    }
    close(file);

    log_updates(hunt , "Treasure added.");
    char linkName[25];
    char log_path[128];
    snprintf(log_path , sizeof(log_path) , "%s/%s" , hunt , "logged_hunt");
    snprintf(linkName , sizeof(linkName) , "logged_hunt-%s" , hunt);
    symlink(log_path , linkName);
}
 
void view(char huntID[] , char treasureID[]){
 
    static char path[128];
    snprintf(path , sizeof(path) , "%s/%s" , huntID , "treasure.bin");
 
    int file = open(path , O_RDONLY , 0777);
    if(file < 0){
        perror("Error opening the file.\n");
        return;
    }

    //int write(int fileDescriptor, void *buffer, size_t bytesToWrite)
    treasure t;
 
    while(read(file , &t , sizeof(treasure)) == sizeof(treasure)){
      if(strcmp(t.id , treasureID) == 0){
        printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n" , 
            t.id , t.user , t.lat , t.lon , t.clue , t.value);
        close(file);
        log_updates(huntID , "Viewed a treasure.");
        return;
      }
    }

    printf("Treasure not found.\n");
    close(file);
}
 
void list(char huntID[]){
    static char path[128];
    snprintf(path , sizeof(path) , "%s/%s" , huntID , "treasure.bin");

    struct stat st;
    stat(huntID , &st);
    int size = st.st_size;
    time_t last = st.st_mtime;
    printf("Hunt: %s\nSize of file:%d\nLast mod:%s\n" , huntID , size , ctime(&last));
    
    int file = open(path , O_RDONLY);
    if(file < 0){
        perror("Error opening the file.\n");
        return;
    }

    treasure t;
    while(read(file , &t , sizeof(treasure)) == sizeof(treasure)){
        printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n" , 
            t.id , t.user , t.lat , t.lon , t.clue , t.value);
        printf("\n");
    }
    close(file);
    log_updates(huntID , "Listed treasures:");
}
 
void remove_treasure(char huntID[] , char treasureID[]){
    static char path[128];
    snprintf(path , sizeof(path) , "%s/%s" , huntID , "treasure.bin");
    
    int file = open(path , O_RDONLY);
    if(file < 0){
        perror("Error opening the file.\n");
        return;
    }
    
    treasure t;
    treasure *treasures = malloc(100 * sizeof(treasure));
    int index = 0;
    while(read(file , &t , sizeof(treasure)) == sizeof(treasure)){
        if(strcmp(t.id , treasureID) != 0){
            treasures[index++] = t;
        }
    }
    close(file);

    file = open(path , O_WRONLY | O_TRUNC);
    write(file , treasures , index * sizeof(treasure));

    close(file);
    free(treasures);
    log_updates(huntID , "Removed a treasure.\n");
} 

void remove_hunt(char huntID[]){
    static char path[128];
    static char log_path[128];
    snprintf(path , sizeof(path) , "%s/treasure.bin" , huntID);
    snprintf(log_path , sizeof(log_path) , "%s/logged_hunt" , huntID);

    unlink(path);
    unlink(log_path);

    if(rmdir(huntID) == 0){
        printf("Hunt '%s' removed.\n" , huntID);
    }
    else{
        perror("Failed to remove");
    }

    char linkName[25];
    snprintf(linkName , sizeof(linkName) , "logged_hunt-%s" , huntID);
    unlink(linkName);
}
 

 
int main(int argc, char** argv) {
 
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
    else {
        printf("Usage: treasure_manager --opt");
    }

    return 0;
}