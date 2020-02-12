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
    ht->Insert_(NULL, "ASD", 6);
    Dump(ht);
    ht->Delete_(ht, "DAGJSK");
    ht->Insert_(ht, "A", 6);
    ht->Insert_(ht, "", 6);
    ht->Delete_(ht, "ASD");
    ht->Delete_(NULL, "ASD");
    ht->Delete_(ht, "GHFJKD");
    printf("%d\n", ht->Find_(ht, "", NULL));
    Dump(ht);
    ht->Find_(NULL, "", NULL);
    ht->Insert_(ht, "D", 67548);
    Dump(ht);
    Dump(NULL);
    printf("%d\n", ht->Find_(ht, "AD", NULL));
    printf("%d\n", ht->Find_(ht, "A", &val));

    HashTable * another = Ht_Create(0);
   

    Ht_Delete(ht);
    return 0;
}