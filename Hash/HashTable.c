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


static Node ** HASH(private * pr, char const * key)
{
    return &pr->nodes[key[0] % pr->size];
}
static int insert(HashTable * ht, char const * key, int value)
{
    if (!ht || !key)
        return E_wrongdata;

    private * pr = (private *)ht->prvtPart_;
    Node ** curr = HASH(pr, key);
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
    if (!*curr)
        return E_alloc;
    if (asprintf(&(*curr)->key, key) == -1)
        return E_alloc;

    // CHECK for -1;
    (*curr)->value = value;

    return E_success;
}

static int find(HashTable const * ht, char const * key, int * value)
{
    if (!ht || !key)
        return E_wrongdata;

    private * pr = (private *)ht->prvtPart_; 
    Node * curr = *HASH(pr, key);
    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            if(value)
                *value = curr->value;
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}
static int delete(HashTable * ht, char const * key)
{
     if (!ht || !key)
        return E_wrongdata;

    private * pr = (private *)ht->prvtPart_; 
    Node ** curr = HASH(pr, key);
    
    while (*curr)
    {
        Node * prev = *curr;
        if (strcmp((*curr)->key, key) == 0)
        {
            *curr = (*curr)->next;
            free(prev->key);
            free(prev);
            return E_success;
        }
        curr = &(*curr)->next;
    }

    return E_success;
}

HashTable * Ht_Create(int size)
{
    if (size <= 0)
        return NULL;

    HashTable * ht = NULL;
    ht = (HashTable *) malloc (sizeof(HashTable));
    if (!ht)
        return NULL;

    ht->prvtPart_ = NULL;
    ht->prvtPart_ = malloc(sizeof(private));
    if (!ht->prvtPart_)
        return NULL;

    ((private *)(ht->prvtPart_))->size = size;
    ((private *)(ht->prvtPart_))->nodes = (Node **) calloc (sizeof(Node *), size);
    if (!((private *)(ht->prvtPart_))->nodes)
        return NULL;

    ht->Insert_ = &insert;
    ht->Find_ = &find;
    ht->Delete_ = &delete;

    return ht;
}

int Dump(HashTable const * ht)
{
    if (!ht)
        return E_wrongdata;

    private * pr = (private *)ht->prvtPart_; 
    Node * curr = NULL;

    int i = 0;
    printf("-----------------------\n");
    for (i = 0; i < pr->size; ++i)
    {
        int flag = 0;
        curr = pr->nodes[i];
        while(curr)
        {
            printf("%s--%d ", curr->key, curr->value);
            curr = curr->next;
            flag = 1;
        }
        if (flag)
            printf("\n");
    }
    printf("-----------------------\n\n");
}

void Ht_Delete(HashTable * ht)
{   
    private * pr = (private *)ht->prvtPart_; 
    Node * curr  = NULL;

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