#ifndef DARRAY_

#define DARRAY_

#include <stdio.h>
#include <stdlib.h>

#include "glib.h"

#ifndef DARRAY_TYPE
#define DARRAY_TYPE
#endif

#ifndef ARRAY_SIZE
// darray default allocation size
#define ARRAY_SIZE 64
#endif
typedef DARRAY_TYPE dArrayType;

// pointer structure, so LINKEDLIST_TYPE should be pointer type
typedef struct {
    dArrayType* arr;
    size_t size;
    size_t allocatedSize;
} dArray;

void dArray_init(dArray* arr) {
    arr->arr = (dArrayType*)calloc(ARRAY_SIZE, sizeof(dArrayType));
    checkMalloc(arr->arr);
    arr->allocatedSize = ARRAY_SIZE;
#ifdef DARRAY_TYPE_INIT
    for (int i = 0; i < arr->allocatedSize; i++) DARRAY_TYPE_INIT(arr->arr[i]);
#endif
    arr->size = 0;
}

void increaseAllocatedSize_(dArray* arr) {
    arr->arr = (dArrayType*)realloc(
        arr->arr, arr->allocatedSize * 2 * sizeof(dArrayType));
    memset(arr->arr + arr->allocatedSize, 0,
           arr->allocatedSize * sizeof(dArrayType));
    // for(int i = arr->allocatedSize; i < arr->allocatedSize * 2; i++)
    // arr->arr[i] = NULL;
    if (arr->arr == NULL) {
        printf("realloc failed");
        exit(1);
    }
    arr->allocatedSize *= 2;
}

// guranteed that new allocated size will be above or equal than newSize
void dArray_increaseAllocatedSizeAE(dArray* arr, size_t newSize) {
    while (arr->allocatedSize < newSize) increaseAllocatedSize_(arr);
}

void dArray_append(dArray* arr, dArrayType rec) {
    if (arr->size == arr->allocatedSize) increaseAllocatedSize_(arr);
    arr->arr[arr->size] = rec;
    arr->size++;
}

void dArray_copy(dArray* dst, dArray* src) {
    if (dst == NULL || src == NULL) return;
    dst->size = src->size;
    dst->allocatedSize = src->allocatedSize;
    dst->arr = (dArrayType*)calloc(src->allocatedSize, sizeof(dArrayType));
    memcpy(dst->arr, src->arr, src->allocatedSize);
}

// will not releasy anything, just sets zero \ pure evil
void dArray_clear(dArray* arr) {
    if (arr == NULL) return;
    for (int i = 0; i < arr->allocatedSize; i++) arr->arr[i] = (dArrayType)NULL;
}

void dArray_free(dArray* arr) {
#ifdef DARRAY_TYPE_FREE
    for (int i = 0; i < arr->size; i++) DARRAY_TYPE_FREE(arr->arr[i]);
#endif
    free(arr->arr);
    arr->allocatedSize = 0;
    arr->size = 0;
    arr->arr = NULL;
}

#endif