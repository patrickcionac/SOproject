#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    char* id;
    char* user;
    double lat, lon;
    char* clue;
    int value;
} treasure;


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
    scanf("%s", t.clue);

    printf("Value:");
    scanf("%d", &t.value);
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

        }
        else if (strcmp(argv[1], "--view") == 0) {

        }
        else if (strcmp(argv[1], "--remove_treasure") == 0) {

        }
        else if (strcmp(argv[1], "--remove_hunt") == 0) {

        }

    }
    else {
        printf("Usage: treasure_manager --opt");
    }
    return 0;
}