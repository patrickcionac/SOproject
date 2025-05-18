#include "treasure_manager_header.h"
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
    snprintf(path , sizeof(path) , "%s%s/%s" , "Hunts/" , huntID , "logged_hunt");
 
    int file = open(path , O_WRONLY | O_CREAT | O_APPEND , 0777);

    time_t now = time(NULL);
    dprintf(file , "%s : %s\n" , ctime(&now) , action);
    close(file);
    //cu comanda " cat "file_name" " in terminal se pot vedea update-urile functiei
}

int get_treasure_file_path(char huntID[] , char path[]){
    return snprintf(path , 256 , "Hunts/%s/treasure.bin" , huntID);
}

void create(char* hunt) {
 
    treasure t;
    printf("Enter data:\n");
 
    printf("Treasure ID:");
    scanf("%s", t.id);
 
    printf("Username:");
    scanf("%s", t.user);
 
    printf("Latitude:");
    scanf("%lf", &t.lat);
 
    printf("Longitude:");
    scanf("%lf", &t.lon);
 
    printf("Clue:");
    getchar();
    fgets(t.clue , 20 , stdin); // am folosit fgets() deoarece clue-ul poate fi un string din mai multe cuvinte.
    t.clue[strlen(t.clue) - 1] = '\0';
 
    printf("Value:");
    scanf("%d", &t.value);
 
 
 
    static char path[128];
    get_treasure_file_path(hunt , path);

    int file = open(path , O_WRONLY | O_CREAT | O_APPEND , 0777);
    if (file < 0) {
        perror("Error opening treasure file");
        return;
    }

 
    if(write(file , &t , sizeof(treasure)) != sizeof(treasure)){
        printf("Error");
        close(file);
        return;
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
    get_treasure_file_path(huntID, path);

    int file = open(path , O_RDONLY , 0777);
    if(file < 0){
        perror("Error opening the file.\n");
        return;
    }

    treasure t;
 
    while(read(file , &t , sizeof(treasure)) == sizeof(treasure)){
      if(strcmp(t.id , treasureID) == 0){
        printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n" , 
            t.id , t.user , t.lat , t.lon , t.clue , t.value);
        printf("\n");
        close(file);
        log_updates(huntID , "Viewed a treasure.");
        return;
      }
    }

    printf("Treasure not found.\n");
    close(file);
}
 
void list(char huntID[]){
    static char path[256];
   

    get_treasure_file_path(huntID, path);

    int file = open(path , O_RDONLY);
    if(file < 0){
        perror("Error opening the file.\n");
        return;
    }

    struct stat st;
    if(fstat(file , &st) == -1){
        perror("Error getting file stats");
        close(file);
        return;
    }
   
    int size = st.st_size;
    time_t last = st.st_mtime;
    printf("Hunt: %s\nSize of file:%d\nLast mod:%s\n" , huntID , size , ctime(&last));
    //aici tocmai ce am oferit niste detalii despre hunt-ul care va fi listat
   

    treasure t;
    while(read(file , &t , sizeof(treasure)) == sizeof(treasure)){
        printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n" , 
            t.id , t.user , t.lat , t.lon , t.clue , t.value);
        printf("\n");
    }
    close(file);
    log_updates(huntID , "Listed treasures");
}
 
void remove_treasure(char huntID[] , char treasureID[]){
    static char path[128];
    get_treasure_file_path(huntID, path);
    
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
    static char dir_path[128];

    snprintf(path , sizeof(path) , "Hunts/%s/treasure.bin" , huntID);
    snprintf(log_path , sizeof(log_path) , "Hunts/%s/logged_hunt" , huntID);
    snprintf(dir_path, sizeof(dir_path), "Hunts/%s", huntID);

    unlink(path);
    unlink(log_path);

    if(rmdir(dir_path) == 0){
        printf("Hunt '%s' removed.\n" , huntID);
    }
    else{
        perror("Failed to remove");
    }

    char linkName[25];
    snprintf(linkName , sizeof(linkName) , "logged_hunt-%s" , huntID);
    unlink(linkName);
}
 
int get_treasure_nr(char *path){ //o functie auxiliara care numara cate treasure-uri sunt intr-un hunt
    int f = open(path , O_RDONLY);
    if(f < 0){
        return 0;
    }
    treasure t;
    int c =  0;
    while(read(f , &t , sizeof(treasure)) == sizeof(treasure)) c++;
    close(f);
    return c;
}

void list_hunts(char *hunts_path){
    DIR *d = opendir(hunts_path); 
    if(!d){
        return;
    }
    struct dirent *dir;

    while((dir = readdir(d)) != NULL){
        if(strcmp(dir->d_name , ".") == 0 || strcmp(dir->d_name , "..") == 0){
            continue;
        }
            char hunt_dir_path[256];
            int check = snprintf(hunt_dir_path , sizeof(hunt_dir_path) , "%s/%s" , hunts_path , dir->d_name);
            if (check < 0 || check >= sizeof(hunt_dir_path)) {
                fprintf(stderr, "Path too long, skipping: %s\n", dir->d_name);
                continue;
            }
            struct stat st;

            if (stat(hunt_dir_path, &st) == -1) {
                perror("stat failed");
                continue;
            }

            if(S_ISDIR(st.st_mode)){
                char treasure_path[256];
                int check = snprintf(treasure_path, sizeof(treasure_path), "%s/treasure.bin", hunt_dir_path); 
                if (check < 0 || check >= sizeof(treasure_path)) {
                    fprintf(stderr, "Path too long, skipping: %s\n", dir->d_name);
                    continue;
                }
                printf("%s: %d treasure uri\n", dir->d_name , get_treasure_nr(treasure_path));
      }
    }
    closedir(d);
}

void calculate_score() {  //am implementat functia ca sa poata fi folosita si aici , nu doar in monitor. ea va fi folosita prin pipe si la monitor.
    DIR *dir = opendir("Hunts"); 
    if (!dir) {
        perror("Failed to open Hunts directory");
        return;
    }

    struct dirent *entry;

    typedef struct {
        char user[32];
        int score;
    } UserScore;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }

        char *hunt_name = entry->d_name;

        char bin_path[275];
        snprintf(bin_path, sizeof(bin_path), "Hunts/%s/treasure.bin", hunt_name); //aici retinem hunt-ul specific

        int fd = open(bin_path, O_RDONLY);
        if (fd < 0){
            continue;
        }

        UserScore users[100];
        int user_count = 0;

        treasure t;
        while (read(fd, &t, sizeof(treasure)) == sizeof(treasure)) {
            if (t.value <= 0 || strlen(t.user) == 0){
                continue;
            }

            int found = 0;
            for (int i = 0; i < user_count; i++) {
                if (strcmp(users[i].user, t.user) == 0) {
                    users[i].score += t.value;
                    found = 1;
                    break;
                }
            }
            if (!found && user_count < 100) {
                strcpy(users[user_count].user, t.user);
                users[user_count].score = t.value;
                user_count++;
            }
        }

        close(fd);

        printf("User scores for %s:\n", hunt_name); // afisam pt fiecare hunt separat scorurile.
        if (user_count == 0) {
            printf("  No users found.\n");
        } else {
            for (int i = 0; i < user_count; i++) {
                printf("  %s: %d\n", users[i].user, users[i].score);
            }
        }
        printf("\n");
    }

    closedir(dir);

}


