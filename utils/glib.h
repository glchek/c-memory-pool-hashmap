#ifndef GLIB
#define GLIB
#include <stdlib.h>
#include <stdio.h>

#include "ginput.h"

void checkMalloc(void* ptr) {
    if (ptr == NULL) {
        printf("malloc failed");
        exit(1);
    }
}

#endif