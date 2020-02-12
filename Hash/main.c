#include <stdio.h>
#include "HashTable.h"

int main() 
{
    int val = 0;
    HashTable * ht = Ht_Create(50);
    ht->Insert_(ht, "A", 50);
    ht->Insert_(ht, "ASD", 578);
    ht->Insert_(ht, "HGFJD", 574839);
    ht->Insert_(ht, "DAGJSK", 62);
    ht->Insert_(ht, "AD", 5);
    Dump(ht);
    ht->Delete_(ht, "DAGJSK");
    ht->Delete_(ht, "ASD");
    Dump(ht);
    ht->Insert_(ht, "D", 67548);
    Dump(ht);
    printf("%d\n", ht->Find_(ht, "AD", NULL));

    HashTable * another = Ht_Create(0);
   

    Ht_Delete(ht);
    return 0;
}