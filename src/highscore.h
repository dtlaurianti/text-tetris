#pragma once

#include <stddef.h>

#define NUM_HIGH_SCORES_DISPLAY 5

int create_scores_table();
int get_high_score(int *const high_score_ptr, char *const name, const size_t size);
int get_high_scores(int *const high_scores, 
        char *const *names,
        const size_t count, const size_t name_length);
int get_high_scorers(int *const high_scores, 
        char *const *names,
        const size_t count, const size_t name_length);
int add_score(const char *const name, const int score);
