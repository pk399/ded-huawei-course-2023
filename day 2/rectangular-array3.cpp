#include <stdio.h>
#include <assert.h>

const int ROWS = 7;
const int COLS = 7;

struct RectArr {
    char* arr;
    size_t rows;
    size_t cols;
};

void PrintData(const RectArr* data)
{
    assert(data->arr != NULL);

    for (size_t i = 0; i < data->rows; i++)
    {
        for (size_t j = 0; j < data->cols; j++)
        {
            printf("%c ", data->arr[i*data->rows + j]);
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

    RectArr arr = {(char*) data, ROWS, COLS};

    PrintData(&arr);
}
