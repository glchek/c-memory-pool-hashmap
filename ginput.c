#include "ginput.h"  //подключаю заголовочный файл для объявления сигнатур функций

#include <stdio.h>

void input(char* buffer, void* a) {
    for (;;) {
        // по "определению" input должен "заставлять" пользователя вводить
        // только одно значение в одной строке, притом в ней не должно быть
        // мусора
        // printf("last buffer& a: %s", buffer);
        if (scanf(buffer, a) & cleanBuffer()) break;
        printf("Неверный ввод\n");
    }
}



int cleanBuffer() {
    char temp;
    int flag = 1;
    do {
        temp = getchar();
        if (temp != '\n' && temp != EOF && temp != ' ') flag = 0;
    } while (temp != '\n' && temp != EOF);
    return flag;
}

void inputArray(char* form, int sizeOfType, int n, void* a) {
    int isEmpty;
    do {
        for (int i = 0; i < n; i++) {
            if (!scanf(form, a + i * sizeOfType)) {
                printf("Неверный ввод\n");
                cleanBuffer();
                i = -1;
            }
        }
        isEmpty = cleanBuffer();
        if (!isEmpty) printf("Неверный ввод\n");
    } while (!isEmpty);
}

