#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_highscore() {
    FILE *hs = fopen("hs.csv", "r");
    if (hs != NULL) {
        char buffer[256];
        fgets(buffer, sizeof(buffer), hs);
        char *name = strtok(buffer, ",");
        char *score = strtok(NULL, "\n");
        fclose(hs);
        return atoi(score);
    }
    return 0;
}
