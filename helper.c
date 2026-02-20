#include <stdio.h>   //printf
#include <stdlib.h>  //rand
#include <string.h>  //strcpy
#include <time.h>    //seed for srand — time

#include "../lab6/record.h"
// #define BY_HAND

// Менять вот это
int number = 3e5; 

void writeRecord(Record* r, FILE* file) {
    fwrite(r, sizeof(Record), 1, file);
    fflush(file);
    return;
}
#ifndef BY_HAND
int randint(int a, int b) {
    if (a >= b) {
        printf("incorrect interval");
        exit(1);
    }
    if (b - a > RAND_MAX) {
        printf("too big interval for rand");
        exit(1);
    }
    return a + rand() % (b - a + 1);
}

void generateRecord(Record* r, int order) {
    r->order = order;
    r->id = randint(0, 2000);
    r->year = randint(1977, 2030);
    int length = randint(3, 10);
    for (int i = 0; i < length; i++) r->model[i] = randint('a', 'z');
    length = randint(4, 15);
    for (int i = 0; i < length; i++) r->mark[i] = randint('a', 'z');
    r->price = randint(100, 5000);
    switch (randint(1, 3)) {
        case 1:
            strcpy(r->status, "available");
            break;
        case 2:
            strcpy(r->status, "rented");
            break;
        case 3:
            strcpy(r->status, "repairing");
            break;
    }
}
#endif

int main() {
    FILE* file = fopen("data", "w+b");
    if (file == NULL) {
        printf("couldnt create the data");
        exit(1);
    }
    FILE* names = fopen("names.txt", "w+");
    if(names == NULL){
        printf("couldnt create the names.txt");
        exit(1);
    }
#ifdef BY_HAND
    number = 6;
    fwrite(&number, sizeof(int), 1, file);
    Record arr[6] = {{0, 126, 1994, "abc", "def", 4655, "available"},
                     {1, 126, 1995, "abc", "def", 4655, "available"},
                     {2, 126, 1995, "abd", "def", 4655, "available"},
                     {3, 126, 1995, "abd", "dee", 4655, "available"},
                     {4, 126, 1995, "abd", "dee", 46, "available"},
                     {5, 126, 1995, "abd", "dee", 46, "repairing"}};
    for (int i = 0; i < number; i++) writeRecord(arr + i, file);
    return 0;
#else
    srand(time(NULL));
    fwrite(&number, sizeof(int), 1, file);
    Record a;
    for (int order = 0; order < number; order++) {
        if (order % 100000 == 0) printf("order: %d\n", order);
        memset(a.model, 0, RECORD_STRING_SIZE * sizeof(char));
        memset(a.mark, 0, RECORD_STRING_SIZE * sizeof(char));
        generateRecord(&a, order);
        writeRecord(&a, file);
        fprintf(names, "%d: %s\n", a.order, a.mark);
    }
#endif
    fflush(file);
    fflush(names);
    fclose(file);
    fclose(names);
    return 0;
}