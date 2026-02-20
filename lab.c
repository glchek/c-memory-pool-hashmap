#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ginput.h"
#include "record.h"

#define LINKEDLIST_TYPE Record*
#define LINKEDLIST_DATA_EQ(a, b) \
    (a && b && a->mark && b->mark && strcmp(a->mark, b->mark) == 0)
#include "LinkedList.h"

#define DARRAY_TYPE Node*
// Удалять не нужно, мемори пул всё сотрёт в конце
//  #define DARRAY_TYPE_FREE(a) LinkedList_free(a)
#define DARRAY_TYPE_INIT(a) a = NULL
#include "dArray.h"

typedef int hash_t;

#define HASH_BASE (5 * (1 << 25) + 1)

#define HASH_K 72

// polyhash
hash_t polyHash(char* key) {
    hash_t s = 0;
    hash_t mult = 1;
    for (int i = 0; key[i] != 0; i++) {
        s = (s + (((key[i] - 'A') * mult) % HASH_BASE)) %
            HASH_BASE;  // overflow handling
        mult = (mult * HASH_K) % HASH_BASE;
    }
    return s;
}

// rot13
hash_t rot13(char* key) {
    unsigned int hash = 0;
    unsigned int i = 0;
    size_t size = strlen(key);
    for (i = 0; i < size; key++, i++) {
        hash += (unsigned char)(*key);
        hash -= (hash << 13) | (hash >> 19);
    }
    return hash;
}

// djb
hash_t djb(char* key) {
    hash_t hash = 5381;
    int c = *key;
    while (c) {
        hash = ((hash << 5) + hash) + c;
        c = *(key++);
    }
    return hash;
}
hash_t (*hashFunctions[])(char*) = {polyHash, rot13, djb};

hash_t (*getHash)(char*);

// pointer structure
#define HASHTABLE_SIZE 1E7

typedef LinkedListType HashTableType;
typedef struct {
    dArray arr;
    size_t n;
} HashTable;

void HashTable_init(HashTable* ht) {
    dArray_init(&(ht->arr));
    dArray_increaseAllocatedSizeAE(&(ht->arr), HASHTABLE_SIZE);
    ht->n = 0;
}

void HashTable_free(HashTable* ht) {
    dArray_free(&(ht->arr));
    ht->n = 0;
}

HashTableType HashTable_find(HashTable* ht, char* key) {
    hash_t hash = getHash(key) % ht->arr.allocatedSize;
    FORILL(it, ht->arr.arr[hash]) {
        if (strcmp(it->data->mark, key) == 0) return it->data;
    }
    return NULL;
}

void HashTable_set(HashTable* ht, char* key, HashTableType data);

void HashTable_rehash_(HashTable* ht) {
    if (ht->arr.allocatedSize >= HASH_BASE && getHash == &polyHash)
        return;  // бесполезно в таком
                 // случае
    dArray_increaseAllocatedSizeAE(&(ht->arr), ht->arr.allocatedSize * 2);
    dArray temp = ht->arr;
    dArray_copy(&temp, &(ht->arr));
    dArray_clear(&(ht->arr));
    for (int i = 0; i < ht->arr.allocatedSize; i++) {
        FORILL(it, temp.arr[i]) {
            // printf("mark: %s", it->data->mark);
            HashTable_set(ht, it->data->mark, it->data);
        }
    }
    dArray_free(&temp);
}

void HashTable_set(HashTable* ht, char* key, HashTableType data) {
    hash_t hash = getHash(key) % ht->arr.allocatedSize;
    if (HashTable_find(ht, key) != NULL) {
        FORILL(it, ht->arr.arr[hash]) {  // Длина второй адресации — пыль
            if (strcmp(it->data->mark, key) == 0) it->data = data;
        }
        return;
    }
    ht->arr.arr[hash] = LinkedList_emplaceNode(ht->arr.arr[hash], data);
    ht->n++;
    if (ht->n * 2 >= ht->arr.allocatedSize) HashTable_rehash_(ht);
}

#define BIG 200

void readRecord(Record* r, FILE* file) {
    if (fread(r, sizeof(Record), 1, file) < 1) {
        printf("ReadRecord: couldnt Read");
        exit(1);
    }
}

void printHead(Record* records, int n) {
    printf("ord\t| id\t| yr\t| mrk\t| mdl\t| prc\t| state\t\t|\n");
    for (int i = 0; i < n && i < BIG; i++) printRecord(records + i);
}

void fsetPtr(FILE* file, long offset, int _Origin) {
    if (fseek(file, offset, _Origin)) {
        printf("erron on fseek");
        exit(1);
    }
}

// mark should be a pointer to a char array of RECORD_STRING_SIZE bytes
void inputMark(char* mark) {
    printf("Введите марку(ключ поиска):\n");
    for (;; printf("Некорректный ввод\n")) {
        fgets(mark, RECORD_STRING_SIZE, stdin);
        int flag = 0;
        for (int i = 0; i < RECORD_STRING_SIZE; i++) {
            if (mark[i] == '\n') {
                mark[i] = 0;
                flag = 1;
                break;
            }
            if (mark[i] == 0 && i != (RECORD_STRING_SIZE - 1)) {
                flag = 1;
                break;
            }
        }
        if (flag) break;
        printf("Марка должна содержать не более %d символов\n",
               RECORD_STRING_SIZE - 2);
        cleanBuffer();
    }
}

int main() {
    int n;
    clock_t start;
    FILE* file = fopen("data", "rb");
    if (file == NULL) {
        printf("couldnt open the data");
        exit(1);
    }
    int number;
    fsetPtr(file, 0, SEEK_SET);
    if (fread(&number, sizeof(int), 1, file) < 1) {
        printf("couldnt Read");
        exit(1);
    }
    printf("number: %d\n", number);
    Record* records = malloc(number * sizeof(Record));
    checkMalloc(records);
    for (int i = 0; i < number; i++) {
        readRecord(records + i, file);
    }
    fclose(file);

    printHead(records, number);
    printf("Функция:\n1 — polyHash\n2 — rot13\n3 — djb\n");
    int mode = 0;
    for (;; printf("неверный ввод\n")) {
        input("%d", &mode);
        if (1 <= mode && mode <= 3) break;
    }
    getHash = hashFunctions[mode - 1];
    HashTable ht;
    HashTable_init(&ht);
    start = clock();
    for (int i = 0; i < number; i++) {
        // printf("i: %d\n", i);
        HashTable_set(&ht, records[i].mark, records + i);
    }
    printf("constructed in %gms\n",
           (clock() - start) * 1000.0 / CLOCKS_PER_SEC);
    int count = 0;
    int m = 0;

    for (int i = 0; i < ht.arr.allocatedSize; i++) {
        int a = LinkedList_getLength(ht.arr.arr[i]);
        a = (a == 0 ? 0 : a - 1);
        count += a;
        m = (a > m ? a : m);
    }
    printf("Collisions: %d\nmax collision for item: %d\n", count, m);

    start = clock();
    for (int i = 0; i < number; i++) {
        if (HashTable_find(&ht, records[i].mark) == NULL) {
            printf("something wrong with %d", i);
            exit(1);
        }
    }
    printf("continuous search of of %d elements took: %gms\n", number,
           (clock() - start) * 1000.0 / CLOCKS_PER_SEC);
    char mark[RECORD_STRING_SIZE];
    int isContinuing = 0;
    for (;;) {
        printf("1 — продолжить\n0 — выход\n");
        for (;; printf("Неверный ввод")) {
            input("%d", &isContinuing);
            if (isContinuing == 1 || isContinuing == 0) break;
        }
        if (!isContinuing) break;
        inputMark(mark);

        Record* ans = NULL;
        clock_t start = clock();
        ans = HashTable_find(&ht, mark);
        if (ans == NULL)
            printf("Ничего не нашлось\n");
        else
            printRecord(ans);
        printf("took: %gms\n", (clock() - start) * 1000.0 / CLOCKS_PER_SEC);
    }
    NodePool_free(&np);
    HashTable_free(&ht);
    return 0;
}