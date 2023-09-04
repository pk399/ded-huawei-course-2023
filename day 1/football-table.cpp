#include <stdio.h>
#include <assert.h>


size_t TableSize(size_t team_count)
{
    size_t n = team_count;
    return ((1+n)*n)/2;
}

void Size_tSwap(size_t *n1, size_t *n2)
{
    *n1 += *n2;
    *n2 = *n1 - *n2;
    *n1 -= *n2;
}

int TableGet(const int table[], size_t team1, size_t team2)
{
    assert(table != NULL);
    assert(team1 != team2);

    if (team1 < team2)
    {
        Size_tSwap(&team1, &team2);
    }

    return *(table + TableSize(team1 - 1) + team2);

}

void PrintTable(const int table[], size_t team_count)
{
    assert(team_count > 1);
    assert(table != NULL);

    printf("vs ");
    for (size_t i = 0; i < team_count - 1; i++)
    {
        printf("%3d ", i);
    }
    putchar('\n');

    for (size_t i = 1; i < team_count; i++)
    {
        printf("%2d ", i);

        for (size_t j = 0; j < team_count; j++)
        {
            if (j >= i)
                break;

            printf("%3d ", TableGet(table, i, j));
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

    PrintTable(table, 6);

    printf("3 vs 2: %d\n", TableGet(table, 2, 3));
}
