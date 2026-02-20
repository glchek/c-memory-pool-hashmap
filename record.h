#ifndef RECORD_H
#define RECORD_H
#define RECORD_STRING_SIZE 40  // SIZE FOR STRINGS
#include <stdio.h>

typedef struct {
    int order;
    int id;
    int year;
    char mark[RECORD_STRING_SIZE];
    char model[RECORD_STRING_SIZE];
    int price;
    char status[RECORD_STRING_SIZE];

} Record;

int lowerKey(Record* a, Record* b) {
    if (strcmp(a->mark, b->mark) < 0) return 1;
    return 0;
}

int lower(Record* a, Record* b) {
    if (a->id < b->id) return 1;
    if (a->id != b->id) return 0;
    if (a->year < b->year) return 1;
    if (a->year != b->year) return 0;
    if (strcmp(a->mark, b->mark) < 0) return 1;
    if (strcmp(a->mark, b->mark) != 0) return 0;
    if (strcmp(a->model, b->model) < 0) return 1;
    if (strcmp(a->model, b->model) != 0) return 0;
    if (a->price < b->price) return 1;
    if (a->price != b->price) return 0;
    if (strcmp(a->status, b->status) < 0) return 1;
    if (strcmp(a->status, b->status) != 0) return 0;
    return 0;
}

int ge(Record* a, Record* b) { return !lower(a, b); }

void printRecord(Record* r) {
    printf("%d\t| %d\t| %d\t| %s\t| %s\t| %d\t| %s\t\t|\n", r->order, r->id,
           r->year, r->mark, r->model, r->price, r->status);
}

#endif  // RECORD_H