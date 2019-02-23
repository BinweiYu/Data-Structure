#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WORD_LEN    32
#define DEF_LEN     256
#define COMMAND_LEN 32
#define OPT_LOOP_TIMES  200

#define false   0
#define true    1

typedef struct list_node_t {
    char word[WORD_LEN];
    char definition[DEF_LEN];
    struct list_node_t* next;
} list_node;

typedef struct list_t {
    int size;
    list_node* head;
    list_node* tail;
} list;

typedef struct vector_t {
    int size;
    list* data;         // data = list[]
} vector;

vector* vector_create(int size) {
    if(size > 1) {
        vector* v = (vector* )malloc(sizeof(vector));
        v->size = size;
        v->data = (list* )malloc(sizeof(list) * size);
        memset(v->data, 0x00, sizeof(list) * size);
        return v;
    }
    else {
        printf("error: initial size must be bigger than 1\n");
        return NULL;
    }
}

list* vector_get(vector* v, int idx) {
    if(0 <= idx && idx < v->size) {
        return &v->data[idx];
    }
    else {
        printf("error: invalid idx\n");
        exit(-1);
    }
}

void vector_resize(vector* v, int size) {
    if(size > 1) {
        if(size == v->size)
            return;
        
        list* new_data = (list* )malloc(sizeof(list) * size);
        if(size < v->size) {
            printf("warning: Downsize might cause data lost");
            memcpy(new_data, v->data, sizeof(list) * size);
        }
        else
            memcpy(new_data, v->data, sizeof(list) * v->size);
        free(v->data);
        v->data = new_data;
        v->size = size;
    }
    else {
        printf("error: initial size must be bigger than 1\n");
        exit(-1);
    }
}

void list_append(list* L, char* word, char* definition) {
    list_node* node = (list_node* )malloc(sizeof(list_node));
    /* Deep copy */
    int word_len = strlen(word);
    memset(node->word, 0x00, sizeof(char) * WORD_LEN);
    memcpy(node->word, word, sizeof(char) * word_len);
    int def_len = strlen(definition);
    memset(node->definition, 0x00, sizeof(char) * DEF_LEN);
    memcpy(node->definition, definition, sizeof(char) * def_len);
    node->next = NULL;
    
    /* When list is empty */
    if(NULL == L->tail)
        L->head = L->tail = node;
    else {
        L->tail->next = node;
        L->tail = node;
    }
    ++L->size;
}

/*
void list_remove_idx(list* L, int idx) {
    list_node dump_node;
    dump_node.next = L->head;
    if(0 <= idx && idx < L->size) {
        list_node* node = &dump_node;
        while(idx--)
            node = node->next;
 
        list_node* tmp = node->next;
        node->next = tmp->next;
        free(tmp);
        tmp = NULL;
        
        --L->size;
        L->head = dump_node.next;
        node = L->head;
        while(node && node->next)
            node = node->next;
        L->tail = node;
    }
    else {
        printf("error: invalid idx\n");
        exit(-1);
    }
}
*/

int list_remove_word(list* L, char* word) {
    list_node dump_node;
    dump_node.next = L->head;
    list_node* node = &dump_node;
    while(node->next && strcmp(node->next->word, word) != 0)
        node = node->next;
    if(NULL == node->next) {
        return false;
    }
    else {
        /* Remove */
        list_node* tmp = node->next;
        node->next = tmp->next;
        free(tmp);
        tmp = NULL;
        
        --L->size;
        L->head = dump_node.next;
        node = L->head;
        while(node && node->next)
            node = node->next;
        L->tail = node;
        return true;
    }
}

/* Return NULL when word doesn't exist, otherwise return its position */
list_node* list_find(list* L, char* word) {
    list_node* node = L->head;
    while(node != NULL) {
        if(strcmp(node->word, word) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

void list_print(list* L) {
    list_node* node = L->head;
    while(node) {
        printf("%s -> ", node->word);
        node = node->next;
    }
    printf("\n");
}

void list_destroy(list* L) {
    list_node* node = L->head;
    while(node) {
        list_node* tmp = node->next;
        free(node);
        node = tmp;
    }
    L->size = 0;
    L->head = L->tail = NULL;
}
