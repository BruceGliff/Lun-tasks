enum EError
{
    E_success,
    E_alloc,
    E_recreate,
    E_wrongdata,
    E_refree
} ERRNO;

extern int BrokenFlag;

typedef struct HashTable
{
    void * prvtPart_;
} HashTable;

HashTable * Ht_Create(int size);
int Ht_Find(HashTable const * ht, char const * key, int * value);
int Ht_Delete(HashTable * ht, char const * key);
int Ht_Insert(HashTable * ht, char const * key, int const value);
void Ht_Free(HashTable * ht);
int Ht_Dump(HashTable const * ht);
int Ht_for_each(HashTable * ht, int (*yf)(HashTable * ht, int * node_el, void * value), void * value);

#define E       printf("ASad\n");\
                fflush(NULL); 
