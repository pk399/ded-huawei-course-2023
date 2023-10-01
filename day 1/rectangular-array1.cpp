#include <stdio.h>

const int ROWS = 7;
const int COLS = 7;


void PrintData(const char data[ROWS][COLS])
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("%c ", data[i][j]);
        }

        putchar('\n');
    }
}

int main(void)
{
    char data[ROWS][COLS] = { {'A', 'B', 'C', 'D'},
                              "EFGH",
                              {'a', 'b', 'c', 'd', 'e', 'f', 'g'},
                              "xyz123",
                              "KLMN",
                              "EOF AC",
                              "MEOW" };

    PrintData(data);
}
