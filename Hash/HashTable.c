#include "HashTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Node
{
    char * key;
    int value;
    struct Node * next;   
} Node;

typedef struct private
{
    Node ** nodes;
    int size;
} private;

static int insert(HashTable * ht, char const * key, int value)
{
    // TODO check pointers
    private * pr = (private *)ht->prvtPart_;
    // TODO make hashfunct
    Node ** curr = &pr->nodes[key[0] % pr->size];
    while(*curr)
    {
        if(strcmp((*curr)->key, key) == 0)
        {
            (*curr)->value = value;
            return E_recreate;
        }
        curr = &(*curr)->next;
    }
    (*curr) = (Node *) calloc (sizeof(Node), 1);
    asprintf(&(*curr)->key, key);
    // CHECK for -1;
    (*curr)->value = value;

    return E_success;
}

static int find(struct HashTable * ht, char const * key, int * value)
{
    // TODO check pointers
    private * pr = (private *)ht->prvtPart_; 
    Node * curr = pr->nodes[key[0] % pr->size];
    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            *value = curr->value;
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

HashTable * Ht_Create(int size)
{
    // TODO check size < 0;
    HashTable * ht = NULL;
    ht = (HashTable *) malloc (sizeof(HashTable));
    ht->prvtPart_ = NULL;
    ht->prvtPart_ = malloc(sizeof(private));
    ((private *)(ht->prvtPart_))->size = size;
    ((private *)(ht->prvtPart_))->nodes = (Node **) calloc (sizeof(Node *), size);

    ht->Insert_ = &insert;
    ht->Find_ = &find;
    return ht;
}

void Dump(HashTable * ht)
{
    private * pr = (private *)ht->prvtPart_; 
    Node * curr = NULL;

    int i = 0;
    for (i = 0; i < pr->size; ++i)
    {
        int flag = 0;
        curr = pr->nodes[i];
        while(curr)
        {
            printf("%s %d -- ", curr->key, curr->value);
            curr = curr->next;
            flag = 1;
        }
        if (flag)
            printf("\n");
    }
}



void Ht_Delete(HashTable * ht)
{   
    private * pr = (private *)ht->prvtPart_; 
    Node * curr;

    int i = 0;
    for (i = 0; i < pr->size; ++i)
    {
        curr = pr->nodes[i];
        while(curr)
        {
            free(curr->key);
            Node * tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(pr->nodes);
    free(pr);
    free(ht);
}