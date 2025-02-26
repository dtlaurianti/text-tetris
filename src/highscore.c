#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define TEXT_TETRIS_DB "text-tetris.db"

int create_scores_table() {
    sqlite3 *db;
    int rc = sqlite3_open(TEXT_TETRIS_DB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s cannot open database: %s\n", __func__, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    char *query = "CREATE TABLE IF NOT EXISTS Scores(Id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Score INT);";
    char *err_msg = NULL;
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s failed to fetch data: %s\n", __func__, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
}

int get_high_score(int *const high_score_ptr, char *const name, const size_t size) {
    sqlite3 *db;
    int rc = sqlite3_open(TEXT_TETRIS_DB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s cannot open database: %s\n", __func__, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    char *score_query = "SELECT MAX(Score) FROM Scores;";
    sqlite3_stmt *res = NULL;
    rc = sqlite3_prepare_v2(db, score_query, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s failed to fetch data: %s\n", __func__, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) {
        *high_score_ptr = sqlite3_column_int(res, 0);
    }
    sqlite3_finalize(res);
    char *name_query = "SELECT Name FROM Scores WHERE Score = (SELECT MAX(Score) FROM Scores);";
    rc = sqlite3_prepare_v2(db, name_query, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s failed to fetch data: %s\n", __func__, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) {
        const char *text = (char *)sqlite3_column_text(res, 0);
        if (strlen(text) >= size) {
            sqlite3_finalize(res);
            sqlite3_close(db);
            return 1;
        }
        strncpy(name, text, strlen(text));
        name[strlen(text)] = '\0';
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
    return 0;
}

int set_high_score(const char *const name, const int score) {
    sqlite3 *db;
    int rc = sqlite3_open(TEXT_TETRIS_DB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s cannot open database: %s\n", __func__, sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    char * query_format = "INSERT INTO Scores(Name, Score) VALUES('%s', %d);";
    size_t score_digits = floor(log10(abs(score)));
    char query[strlen(query_format) + strlen(name) + score_digits];
    snprintf(query, sizeof(query), query_format, name, score);
    char *err_msg = NULL;
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "%s failed to fetch data: %s\n", __func__, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
}
