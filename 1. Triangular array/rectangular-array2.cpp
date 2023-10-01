#include <stdio.h>

const int ROWS = 7;
const int COLS = 7;


void PrintData(const char* data, size_t rows, size_t cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%c ", *(data + i*cols + j)); // data[i*cols + j]
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

    PrintData((char*) data, ROWS, COLS);
}
