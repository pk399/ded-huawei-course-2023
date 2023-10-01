//Считывает текст из файла с помощью прямоугольного массива строк

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int ReadLines(FILE* file, char* buf, int max_lines, int max_line_len);


int ReadLines(FILE* file, char* buf, int max_lines, int max_line_len)
{
    assert(file != NULL);
    assert(buf != NULL);
    assert(max_lines > 0);
    assert(max_line_len > 0);


    for (int i = 0; i < max_lines; i++)
    {
        char* result = fgets((buf + i*max_line_len), max_line_len, file);
        assert(result != NULL);

        if (feof(file))
        {
            return i+1;
        }
    }

    return max_lines;
}

int main(void)
{
    const int MAX_LINES = 10;
    const int MAX_LINE_LEN = 100;

    char text1[MAX_LINES][MAX_LINE_LEN] = {};

    FILE* file = fopen("meow.txt", "rt");
    assert(file != NULL);

    int line_count = ReadLines(file, (char*) text1, MAX_LINES, MAX_LINE_LEN);

    printf("Read lines:\n");
    for (int i = 0; i < line_count; i++)
    {
        printf("%d: %s", i + 1, text1[i]);
    }

    fclose(file);
}
