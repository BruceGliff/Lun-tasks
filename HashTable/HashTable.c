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

static void * new(size_t size_of, int count);
static Node ** HASH(private const * pr, char const * key)
{
    return &pr->nodes[key[0] % pr->size];
}
int Ht_Insert(HashTable * ht, char const * key, int value)
{
    if (!ht || !key)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    private const * pr = (private *)ht->prvtPart_;
    Node ** curr = HASH(pr, key);
    while(*curr)
    {
        if(strcmp((*curr)->key, key) == 0)
        {
            (*curr)->value = value;
            ERRNO = E_recreate;
            return 0;
        }
        curr = &(*curr)->next;
    }
    (*curr) = (Node *) new(sizeof(Node), 1);
    if (!*curr)
    {
        ERRNO = E_alloc;
        return -1;
    }
    if (BrokenFlag || asprintf(&(*curr)->key, key) == -1)
    {
        BrokenFlag = 0;
        free(*curr);
        *curr = NULL;
        ERRNO = E_alloc;
        return -1;
    }
    (*curr)->value = value;

    return 0;
}

int Ht_Find(HashTable const * ht, char const * key, int * value)
{
    if (!ht || !key)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    private const * pr = (private *)ht->prvtPart_; 
    Node const * curr = *HASH(pr, key);
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
int Ht_Delete(HashTable * ht, char const * key)
{
    if (!ht || !key)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    private const * pr = (private *)ht->prvtPart_; 
    Node ** curr = HASH(pr, key);
    
    while (*curr)
    {
        Node * prev = *curr;
        if (strcmp((*curr)->key, key) == 0)
        {
            *curr = (*curr)->next;
            free(prev->key);
            free(prev);
            return 0;
        }
        curr = &(*curr)->next;
    }

    return 0;
}

HashTable * Ht_Create(int size)
{
    if (size <= 0)
    {
        ERRNO = E_wrongdata;
        return NULL;
    }

    HashTable * ht = NULL;
    ht = (HashTable *) new(sizeof(HashTable), 1);
    if (!ht)
    {
        ERRNO = E_alloc;
        return NULL;
    }

    ht->prvtPart_ = NULL;
    ht->prvtPart_ = new(sizeof(private), 1);
    if (!ht->prvtPart_)
    {
        free(ht);
        ERRNO = E_alloc;
        return NULL;
    }
    ((private *)(ht->prvtPart_))->size = size;
    ((private *)(ht->prvtPart_))->nodes = (Node **) new (sizeof(Node *), size);
    if (!((private *)(ht->prvtPart_))->nodes)
    {   
        free(ht->prvtPart_);
        free(ht);
        ERRNO = E_alloc;
        return NULL;
    }
    return ht;
}

int Ht_Dump(HashTable const * ht)
{
    if (!ht)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

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

    return 0;
}

void Ht_Free(HashTable * ht)
{   
    if (!ht)
    {
        ERRNO = E_wrongdata;
        return;
    }

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

void * new(size_t size_of, int count)
{
    if (BrokenFlag == 1)
    {
        --BrokenFlag;
        return NULL;
    }
    if (BrokenFlag > 0)
        --BrokenFlag;
    return calloc(size_of, count);
}

int Ht_for_each(HashTable * ht, int (*yf)(HashTable * ht, int * node_el, void * value), void * value)
{
    if (!ht || !yf)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    private * pr = (private *)ht->prvtPart_; 
    Node * curr = NULL;

    int i = 0;

    for (i = 0; i < pr->size; ++i)
    {
        int flag = 0;
        curr = pr->nodes[i];
        while(curr)
        {
            yf(ht, &curr->value, value);
            curr = curr->next;
        }
    }
    return 0;
}
