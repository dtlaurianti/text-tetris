#pragma once

#include <stddef.h>

int create_scores_table();
int get_high_score(int *const high_score_ptr);
int get_high_score_name(char *const name, const size_t size);
int set_high_score(const char *const name, const int score);
