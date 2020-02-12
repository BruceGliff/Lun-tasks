enum EError
{
    E_success = -1,
    E_alloc = -2,
    E_recreate = -3,
    E_wrongdata = -4,
    E_refree = -5
};

typedef struct HashTable
{
    void * prvtPart_;
    int (*Find_) (struct HashTable const * ht, char const * key, int * value);
    int (*Delete_) (struct HashTable * ht, char const * key);
    int (*Insert_) (struct HashTable * ht, char const * key, int const value);
} HashTable;

HashTable * Ht_Create(int size);
// int Insert(struct HashTable * ht, char const * key, int const value);
// int Delete(struct HashTable * ht, char const * key);
// int Find(struct HashTable * ht, char const * key, int * value);
void Ht_Delete(HashTable * ht);
int Dump(HashTable const * ht);

#define E       printf("ASad\n");\
                fflush(NULL); 
