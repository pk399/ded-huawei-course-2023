//Считывает текст из файла с помощью динамического *массива* строк

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int ReadLines(FILE* file, char* buf, int max_len);


int ReadLines(FILE* file, char* buf, int max_len)
{
    assert(file != NULL);
    assert(buf != NULL);
    assert(max_len > 0);

    int read_lines = 0;
    int read_symbols = 0;

    while (read_symbols < max_len && !feof(file))
    {
        char* result = fgets((buf + read_symbols), max_len - read_symbols, file);
        assert(result != NULL);

        read_symbols += strlen(buf + read_symbols) + 1;
        read_lines++;
    }

    return read_lines;
}

int main(void)
{
    const int MAX_LEN = 1000;

    char* text2 = (char*) calloc(MAX_LEN, sizeof(char));

    FILE* file = fopen("meow.txt", "rt");
    assert(file != NULL);

    int line_count = ReadLines(file, (char*) text2, MAX_LEN);

    printf("Read lines:\n");

    char* nextptr = text2;

    for (int i = 0; i < line_count; i++)
    {
        printf("%d: %s", i + 1, nextptr);

        nextptr += strlen(nextptr) + 1;
    }

    fclose(file);
    free(text2);
}
