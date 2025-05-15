#ifndef HEADER_H
#define HEADER_H

typedef struct{
    char id[10];
    char user[25];
    double lat , lon;
    char clue[20];
    int value;
}treasure;

int get_treasure_file_path(char huntID[] , char path[]);
void create(char *hunt);
void log_updates(char huntID[] , char action[]);
void print_treasure(char *tr_path);
void view(char huntID[] , char treasureID[]);
void list(char huntID[]);
void remove_treasure(char huntID[] , char treasureID[]);
void remove_hunt(char huntID[]);
int get_treasure_nr(char *path);
void list_hunts(char *hunts_path);

#endif


