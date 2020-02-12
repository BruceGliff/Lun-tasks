#include <stdio.h>
#include "HashTable.h"

int main() 
{
    HashTable * ht = Ht_Create(50);
    ht->Insert_(ht, "AD", 5);
    ht->Insert_(ht, "ASD", 578);
    ht->Insert_(ht, "HGFJD", 574839);
    int val = 0;
    ht->Find_(ht, "HGFJD", &val);
    Dump(ht);
    printf("%d\n", val);
    ht->Insert_(NULL, "DAGJSK", 62);

    Ht_Delete(ht);
    return 0;
}