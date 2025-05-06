#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>

typedef struct {
    char id[10];
    char user[25];
    double lat, lon;
    char clue[20];
    int value;
} treasure;

int get_treasure_file_path(char huntID[] , char path[]);
