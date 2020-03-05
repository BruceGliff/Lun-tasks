enum EError
{
    E_success,
    E_alloc,
    E_recreate,
    E_wrongdata,
    E_refree,
    E_WFunctExit,
    E_RFunctExit
} ERRNO;

//extern int BrokenFlag;

typedef struct UNode
{
    char const * key;
    int value;
} UNode;

struct HashTable;
typedef struct HashTable HashTable;

HashTable * Ht_Create(int size);
int Ht_Find(HashTable const * ht, char const * key, int * value);
int Ht_Delete(HashTable * ht, char const * key);
int Ht_Insert(HashTable * ht, char const * key, int const value);
void Ht_Free(HashTable * ht);
int Ht_for_each(HashTable * ht, int (*yf)(HashTable * ht, UNode * node_el, void * value), void * value);
