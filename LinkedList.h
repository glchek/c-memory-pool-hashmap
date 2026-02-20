#ifndef LINKEDLIST_
#define LINKEDLIST_

#include <stdio.h>
#include <stdlib.h>

#include "glib.h"
#ifndef LINKEDLIST_TYPE
#define LINKEDLIST_TYPE
#endif

typedef LINKEDLIST_TYPE LinkedListType;
// pointer structure, so LINKEDLIST_TYPE should be pointer type

typedef struct Node {
    LinkedListType data;
    struct Node* next;
} Node;

#define NODEPOOL_SIZE (1 << 16)

typedef struct NodePool {
    Node* pool;
    size_t allocatedSize;
    size_t curAvailable;
    struct NodePool* next;
} NodePool;

// clock_t totalUsedByMalloc = 0;

NodePool np = {NULL, 0};

void NodePool_init(NodePool* np, int size) {
    np->pool = (Node*)malloc(size * sizeof(Node));
    checkMalloc(np->pool);
    np->curAvailable = 0;
    np->allocatedSize = NODEPOOL_SIZE;
    np->next = NULL;
}

void NodePool_free(NodePool* np) {
    if (np->next != NULL) {
        NodePool_free(np->next);
    }
    free(np->pool);
    np->allocatedSize = 0;
    np->curAvailable = 0;
}

void NodePool_continue(NodePool* np) {
    if (np->next != NULL) {
        printf("next should be null");
        exit(1);
    }
    np->next = (NodePool*)malloc(sizeof(NodePool));
    checkMalloc(np->next);
    NodePool_init(np->next, np->allocatedSize * 2);
}

Node* NodePool_getNewNode(NodePool* np) {
    for (NodePool* it = np; it != NULL; it = it->next) {
        if (it->pool == NULL) NodePool_init(it, NODEPOOL_SIZE);
        if (it->curAvailable == it->allocatedSize && it->next == NULL)
            NodePool_continue(it);
        if (it->curAvailable < it->allocatedSize) {
            return &(it->pool[it->curAvailable++]);
        }
    }
    exit(1);
    return NULL;
}

Node* createNode_(LinkedListType data) {
    // clock_t start = clock();
    // Node* a = (Node*)malloc(sizeof(Node));
    Node* a = NodePool_getNewNode(&np);
    // totalUsedByMalloc += clock() - start;
    checkMalloc(a);
    a->data = data;
    a->next = NULL;
    return a;
}

// insert el after it
void nodeInsert_(Node* it, Node* el) {
    if (it == NULL) {
        printf("inserting after NULL\n");
        exit(1);
    }
    el->next = it->next;
    it->next = el;
}

// emplaces after (it) and returns (it) back
// creates List if (it) is NULL and returns its head
Node* LinkedList_emplaceNode(Node* it, LinkedListType data) {
    if (it == NULL) {
        it = createNode_(data);
        return it;
    }
    Node* newNode = createNode_(data);
    nodeInsert_(it, newNode);
    return it;
}
// returns new head
Node* LinkedList_emplaceBeforeHead(Node* head, LinkedListType data) {
    Node* newHead = LinkedList_emplaceNode(NULL, data);
    newHead->next = head;
    return newHead;
}

#ifdef LINKEDLIST_DATA_EQ
int LinkedList_nodesEq(Node* a, Node* b) {
    return LINKEDLIST_DATA_EQ(a->data, b->data);
}

int LinkedList_listCount(Node* beg, Node* el) {
    Node* cur = beg;
    int count = 0;
    for (; cur != NULL; cur = cur->next) count += LinkedList_nodesEq(cur, el);
    return count;
}
#endif

int LinkedList_getLength(Node* beg) {
    Node* cur = beg;
    int size = 0;
    for (size = 0; cur != NULL; size++) cur = cur->next;
    return size;
}

#define FORILL(it, beg) for (Node* it = beg; it != NULL; it = it->next)

// Node* listFindPow(Node* beg, int pow) {
//     Node* cur = beg;
//     for (; cur != NULL; cur = cur->next)
//         if (cur->pow == pow) return cur;
//     return NULL;
// }

// неудобно, но прозрачно
void LinkedList_incNode(Node** p) { *p = (*p)->next; }

// deletesNode after it
void LinkedList_deleteNodeAfter(Node* it) {
    Node* tmp = it->next;
    it->next = it->next->next;
    free(tmp);
}

void LinkedList_free(Node* beg) {
    // nodepool содержит все узлы, если вызывается то это утечка памяти
    // Не отмечаю, что нода свободна, потому что аллокатору легче выделить ещё
    // один кусок, чем устранять дефрагментацию
    exit(1);
    Node* cur = beg;
    for (; cur != NULL;) {
        Node* tmp = cur->next;

        free(cur);
        cur = tmp;
    }
}

#endif