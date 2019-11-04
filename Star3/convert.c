#include <stdio.h>
#include <stdlib.h>

int bit;
char a;


void get1()
{  
    a |= 1<<bit;

    if (bit == 7)
    {
        printf("%c", a);
        a = 0;
    }

    bit = (bit + 1) % 8;
}
void get0()
{
    a |= 0<<bit;

    if (bit == 7)
    {
        printf("%c", a);
        a = 0;
    }
    bit = (bit + 1) % 8;
}


int main()
{
    FILE * f = fopen("Text.txt", "r");

    while(1)
    {
        int b = 0;
        char c = fgetc(f);
        if (feof(f)) break;
        while(b < 8)
        {
            if (c & (1<<b))
                get1();
            else 
                get0();
            b++;
        }
    }

}