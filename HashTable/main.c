#include <stdio.h>
#include "HashTable.h"

#define Ht_Dump Ht_for_each(ht, &Dump, NULL); printf("\n");

int BrokenFlag = 0;

int Sum(HashTable * ht, UNode * node_el, void * value)
{
    *(int*)value += node_el->value;
    printf("KEYS!! %s \n", node_el->key);
    return 0;
}
int SetNULL(HashTable * ht, UNode * node_el, void * value)
{
    node_el->value = 0;
    return 0;
}

int WrongEx(HashTable * ht, UNode * node_el, void * value)
{
    return -1;
}
int Ex(HashTable * ht, UNode * node_el, void * value)
{
    return 1;
}

int Dump(HashTable * ht, UNode * node_el, void * value)
{
    printf("%s--%d ", node_el->key, node_el->value);
    return 0;
}

int main() 
{
    //Create part
    HashTable * ht = Ht_Create(50);
    HashTable * ErrorSize = Ht_Create(6473892);
    
    //Insert test
    Ht_Insert(ht, "A", 50);
    Ht_Insert(ht, "ASD", 578);
    Ht_Insert(ht, "HGFJD", 574839);
    Ht_Insert(ht, "DAGJSK", 62);
    Ht_Insert(ht, "AD", 5);
    Ht_Insert(ht, "fdgvbsdkfjgdsvbskdjfhsgvjdfhyvdjhmgdfkjvbdfugbvdkjfhgvdjkfvgjf", 68741265);
    Ht_Dump;

    //Find test
    int val = 42;
    Ht_Find(ht, "A", &val);
    printf("%d\n", val); //50
    Ht_Delete(ht, "ASD");
    val = 42;
    Ht_Find(ht, "ASD", &val);
    printf("%d\n", val); //42
    Ht_Insert(ht, "ASD", 578);
    Ht_Dump;

    //Update
    Ht_Insert(ht, "ASD", 78);
    Ht_Dump;

    // Delete test
    Ht_Delete(ht, "ASD");
    Ht_Delete(ht, "AD");
    Ht_Delete(ht, "HGFJD");
    Ht_Delete(ht, "GFHJDKSL"); // delete wmth not in ht
    Ht_Dump;


    //Broken tests
    Ht_Insert(ht, NULL, 654);
    printf("%d\n", ERRNO); //3
    BrokenFlag = 1;
    Ht_Insert(ht, "GHFJDK", 789);
    printf("%d\n", ERRNO); //1

    Ht_Find(NULL, "asd", NULL);
    Ht_Find(ht, NULL, NULL);
    printf("%d\n", ERRNO); //3
    Ht_Delete(NULL, NULL);
    Ht_for_each(NULL, &Sum, NULL);
    printf("%d\n", ERRNO); //3

    HashTable * err = Ht_Create(0);
    printf("%d %p\n", ERRNO, err); //3 0
    BrokenFlag = 1;
    err = Ht_Create(4);
    printf("%d %p\n", ERRNO, err); //1 0
    BrokenFlag = 2;
    err = Ht_Create(4);
    printf("%d %p\n", ERRNO, err); //1 0
    BrokenFlag = 3;
    err = Ht_Create(4);
    printf("%d %p\n", ERRNO, err); //1 0

    Ht_Free(NULL);

    // FUNCT test
    int sum = 0;
    Ht_Insert(ht, "ASDFGHJK", 1);
    Ht_Dump;
    Ht_for_each(ht, &Sum, &sum);
    Ht_for_each(ht, &WrongEx, NULL);
    Ht_for_each(ht, &Ex, NULL);
    printf("sum = %d\n", sum); // 68741378

    Ht_for_each(ht, &SetNULL, NULL);
    Ht_Dump;
    sum = 0;
    Ht_for_each(ht, &Sum, &sum);
    printf("sum = %d\n", sum); // 0

    //INSERT broke
    BrokenFlag = 2;
    Ht_Insert(ht, "RTYUI", 65748);
    Ht_Dump;
   
    Ht_Free(ErrorSize);
    Ht_Free(ht);
    return 0;
}
