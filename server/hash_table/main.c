#include<stdio.h>
#include "hashtable.h"

int main()
{
    hash_table_t *table = hash_table_new(MODE_VALUEREF);
    int i = 1;
    int val = 100;
    int val2 = 200;
    int j = 2;
    int x =0;
    for (x=0;x<300;x++)
    {
        // use the macro
        HT_ADD(table, &j, &val);
        // or use the function
        //hash_table_add(table, &j, i, (void *) &val, sizeof(int));
        val++;
        j++;
    }
    hash_table_add(table, &j, i, (void *) &val2, 1);
    j--; j--;
    hash_table_remove(table, &j, i);
    HT_REMOVE(table, &j);
    if (hash_table_has_key(table, &j, i))
    {
        printf("Key found %d\n", j);
    }
    else
    {
        printf("Key NOT found %d\n", j);
    }
    val = -100;
    val2 = -200;
    int *value = NULL;
    value = (int* ) HT_LOOKUP(table, &j);
    void** keys = NULL;
    size_t num = hash_table_get_keys(table, keys);
    printf("found %d keys\n", (int)num);
    printf("j -> %d \n", j);
    if (value)
        printf("value is %d\n", *value);
    else
        printf("*value is %p\n", value);

    return 0;
}
