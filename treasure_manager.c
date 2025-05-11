#include "treasure_manager.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

void log_updates(char huntID[] , char action[]){
    static char path[128];
    snprintf(path , sizeof(path) , "%s%s/%s" , "hunts/" , huntID , "logged_hunt");
 
    int file = open(path , O_WRONLY | O_CREAT | O_APPEND , 0777);

    time_t now = time(NULL);
    dprintf(file , "%s : %s\n" , ctime(&now) , action);
    close(file);
    //with " cat "file_name" " command in the terminal you can see the updates of the function
}

int get_treasure_file_path(char huntID[] , char path[]){
    return snprintf(path , 128 , "%s%s/treasure.bin" , "hunts/" , huntID);
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

void print_treasure(char *tr_path){
    int f = open(tr_path , O_RDONLY);

    if(!f){
        exit(-1);
    }

    treasure t;
    int nr = 1;
    while(read(f , &t , sizeof(treasure))){
        printf("Treasure no.%d\n" , nr++);
        printf("ID: %s\n", t.id);
        printf("Username: %s\n", t.user);
        printf("GPS Longitude: %lf\n", t.lon);
        printf("GPS Latitude: %lf\n", t.lat);
        printf("Clue: %s\n", t.clue);
        printf("Value: %d\n\n\n",t.value);
    }
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
 
int get_treasure_nr(char *path){
    int f = open(path , O_RDONLY);
    if(!f){
        exit(-1);
    }
    treasure t;
    int c =  0;
    while(read(f , &t , sizeof(treasure))) c++;
    close(f);
    return c;
}

void list_hunts(char *hunts_path){
    DIR *d;
    if((d = opendir(hunts_path)) == NULL){
        exit(-1);
    }
    struct dirent *dir;

    while((dir = readdir(d)) != NULL){
        if(strcmp(dir->d_name , ".") == 0 || strcmp(dir->d_name , "..") == 0){
            continue;
        }
            struct stat st;
            char path[1024];
            int written = snprintf(path, sizeof(path), "%s%s",hunts_path , dir->d_name); 
            if (written < 0 || written >= sizeof(path)) {
                perror("Path too long\n");
                exit(-1);
            }
            if(stat(path,&st) == -1){
                perror("stat failed\n");
                exit(-1);
            }
            if(S_ISDIR(st.st_mode)){
                printf("%s: ", dir->d_name);
                char treasure_path[1024];
                written = snprintf(treasure_path, sizeof(treasure_path), "%s/treasure_%s.bin", path ,dir->d_name); 
                if (written < 0 || written >= sizeof(path)) {
                    perror("Path too long\n");
                    exit(-1);
                }
                printf("%d treasure uri\n", get_treasure_nr(treasure_path));
      }
    }
    closedir(d);
}

