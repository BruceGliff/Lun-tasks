#include "HashTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ret {ERRNO = E_success; return 0;}

typedef struct Node
{
    char * key;
    int value;
    struct Node * next;  
} Node;
struct HashTable
{
    Node ** nodes;
    int size;
};

static void * new(size_t size_of, int count);
static Node ** find(HashTable const * ht, char const * key);
static int free_(HashTable * ht, UNode * node, void * value);
static Node ** HASH(HashTable const * pr, char const * key)
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
    Node ** curr = find(ht, key);

    if (*curr)
    {
        (*curr)->value = value;
        ERRNO = E_recreate;
        return 0;
    }
    (*curr) = (Node *) new(sizeof(Node), 1);
    if (!*curr)
    {
        ERRNO = E_alloc;
        return -1;
    }

    int const len = strlen(key);
    (*curr)->key = (char *) new(sizeof(char), len + 1);
    if (!(*curr)->key)
    {
        free(*curr);
        *curr = NULL;
        ERRNO = E_alloc;
        return -1;
    }
    strcpy((*curr)->key, key);
    (*curr)->value = value;
    
    ret;
}

int Ht_Find(HashTable const * ht, char const * key, int * value)
{
    if (!ht || !key)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    Node * curr = *find(ht, key);
    if (curr)
        *value = curr->value;

    ret;
}
int Ht_Delete(HashTable * ht, char const * key)
{
    if (!ht || !key)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    Node ** curr = find(ht, key);

    if(!*curr)
        ret;

    Node * tmp = *curr;
    *curr = (*curr)->next;
    free(tmp->key);
    free(tmp);

    ret;
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

    ht->size = size;
    ht->nodes = (Node **) new (sizeof(Node *), size);
    if (!ht->nodes)
    {   
        free(ht);
        ERRNO = E_alloc;
        return NULL;
    }
    ERRNO = E_success;
    return ht;
}

void Ht_Free(HashTable * ht)
{   
    if (!ht)
    {
        ERRNO = E_wrongdata;
        return;
    }

    Ht_for_each(ht, &free_, NULL);

    free(ht->nodes);
    free(ht);


    ERRNO = E_success;
}

int Ht_for_each(HashTable * ht, int (*yf)(HashTable * ht, UNode * node_el, void * value), void * value)
{
    if (!ht || !yf)
    {
        ERRNO = E_wrongdata;
        return -1;
    }

    Node * curr = NULL;

    int i = 0;

    for (i = 0; i < ht->size; ++i)
    {
        int flag = 0;
        curr = ht->nodes[i];
        while(curr)
        {
            Node * tmp = curr->next;
            int const status = yf(ht, (UNode *) curr, value);
            if (status < 0)
            {
                ERRNO = E_WFunctExit;
                return -1;
            }
            if (status > 0)
            {
                ERRNO = E_RFunctExit;
                return 1;
            }
            
            curr = tmp;
        }
    }
    ret;
}

static Node ** find(HashTable const * ht, char const * key)
{
    Node ** curr = HASH(ht, key);
    while (*curr)
    {
        if (strcmp((*curr)->key, key) == 0)
        {
            return curr;
        }
        curr = &(*curr)->next;
    }

    return curr;
}

static int free_(HashTable * ht, UNode * node, void * value)
{
    Node * curr = (Node *) node;
    free(curr->key);
    Node * tmp = curr->next;
    free(curr);

    return 0;
}

static void * new(size_t size_of, int count)
{
    static int c = -1;
    ++c;

    if (c == 18 || c == 19 || c == 21 || c == 25)
        return NULL;

    return calloc(size_of, count);
}
