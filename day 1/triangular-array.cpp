#include <stdio.h>
#include <assert.h>


size_t TrArrSize(size_t indexes)
{
    size_t n = indexes;
    return ((1+n) * n) / 2;
}

void SwapSize_t(size_t *n1, size_t *n2)
{
    *n1 += *n2;
    *n2 = *n1 - *n2;
    *n1 -= *n2;
}

int TrArrGet(const int tr_arr[], size_t idx1, size_t idx2)
{
    assert(tr_arr != NULL);
    assert(idx1 != idx2);

    if (idx1 < idx2)
    {
        Size_tSwap(&idx1, &idx2);
    }

    return *(tr_arr + TrArrSize(idx1 - 1) + idx2);

}

void PrintTrArr(const int tr_arr[], size_t indexes)
{
    assert(indexes > 1);
    assert(tr_arr != NULL);

    printf("   ");
    for (size_t i = 0; i < indexes - 1; i++)
    {
        printf("%3d ", i);
    }
    putchar('\n');

    for (size_t i = 1; i < indexes; i++)
    {
        printf("%2d ", i);

        for (size_t j = 0; j < indexes; j++)
        {
            if (j >= i)
                break;

            printf("%3d ", TrArrGet(tr_arr, i, j));
        }

        putchar('\n');
    }
}


int main(void)
{
    int table[15] = { 0,
                      2, 3,
                      -2, -1, 5,
                      5, 2, -1, 0,
                      5, 1, -3, 2, 0 };

    PrintTrArr(table, 6);

    printf("3 vs 2: %d\n", TrArrGet(table, 2, 3));
}
