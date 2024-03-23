#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_high_score() {
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

int set_high_score(char *name, int score) {
    FILE *hs = fopen("hs.csv", "w");
    if (hs != NULL) {
        fprintf(hs, "%s, %d", name, score);
        fclose(hs);
        return 0;
    }
    return 1;
}
