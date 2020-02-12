enum EError
{
    E_success,
    E_alloc,
    E_recreate,
    E_wrongdata,
    E_refree
};

typedef struct HashTable
{
    void * prvtPart_;
    int (*Find_) (struct HashTable * ht, char const * key, int * value);
    //int (*Delete_) (char const * key);
    int (*Insert_) (struct HashTable * ht, char const * key, int const value);
} HashTable;

HashTable * Ht_Create(int size);
// int Insert(struct HashTable * ht, char const * key, int const value);
// int Delete(struct HashTable * ht, char const * key);
// int Find(struct HashTable * ht, char const * key, int * value);
void Ht_Delete(HashTable * ht);
void Dump(HashTable * ht);

#define E       printf("ASad\n");\
                fflush(NULL); 
