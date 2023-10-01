#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


const size_t LN_WIDTH = 3;


enum ORDER {
    OK = 0,
    SWAP = 1,
};


struct text5 {
    char** lines;
    size_t nLines;
    char* chars;
    size_t nChars;
};


int FileSize(size_t* sz, const char* path)
{
    struct stat stats_buf = {};

    if (stat(path, &stats_buf))
        return -1;

    *sz = stats_buf.st_size;
    //printf("Size: %zd\n", *sz);
    return 0;
}


void FindLines(text5* txt)
{
    txt->lines[0] = txt->chars;

    size_t nline = 1;
    for (size_t i = 0; i < txt->nChars; i++)
        if (txt->chars[i] == '\n')
        {
            txt->chars[i] = '\0';
            txt->lines[nline++] = txt->chars + i + 1;
        }

    txt->nLines = nline;
}


int ReadFromFile(text5* txt, const char* path)
{
    size_t sz = 0;
    if (FileSize(&sz, path)) return -1;

    sz++;
    txt->nChars = sz;
    txt->chars = (char*) calloc(sz, sizeof(char));
    if (!txt->chars) return -1;


    FILE* file = fopen(path, "rb");
    if (!file) return -1;
    size_t sz_read = fread(txt->chars, sizeof(char), sz, file);


    /*perror("errno");
    printf("Read: %zd, stat: %zd\n", sz_read, sz);
    fclose(file);

    printf("Last chars:");
    for (int i = 0 ; i< 10000; i++)
        printf("[%c %d]", txt->chars[sz-i], txt->chars[sz-i]);
    putchar('\n');*/


    #warning at least, don't hardcode
    const size_t MAX_LINES = 60015;
    txt->lines = (char**) calloc(MAX_LINES, sizeof(char*));
    if (!txt->lines) return -1;
    FindLines(txt);

    return 0;
}


bool HasAlpha(const char* str)
{
    for (size_t i = 0; str[i]; i++)
        if (isalpha(str[i]))
            return true;

    return false;
}


void PrintSampleAlpha(const text5* txt, size_t lines)
{
    size_t printed = 0;

    for (size_t i = 0; i < txt->nLines; i++)
        if (HasAlpha(txt->lines[i]))
        {
            printed++;

            printf("%*zd: %s\n",
                   LN_WIDTH, printed,
                   /*(i != txt->nLines - 1) ?
                   (int) (txt->lines[i+1] - txt->lines[i] - 1) :
                   strlen(txt->lines[i]),*/ txt->lines[i]);

            if  (printed == lines)
                break;
        }
}


void PrintSample(const text5* txt, size_t lines)
{
    for (size_t i = 0; i < lines && i < txt->nLines; i++)
        printf("%*zd: %s\n",
               LN_WIDTH, i + 1,
               /*(i != txt->nLines - 1) ?
               (int) (txt->lines[i+1] - txt->lines[i] - 1) :
               strlen(txt->lines[i]),*/ txt->lines[i]);
}


void TextKiller(text5* txt)
{
    free(txt->chars);
    txt->chars = 0;
    txt->nChars = 0;

    free(txt->lines);
    txt->lines = 0;
    txt->nLines = 0;
}

void Print(const text5* txt)
{
    PrintSample(txt, (size_t) -1);
}


inline void* VoidIndex(const void* arr, size_t el_size, size_t index)
{
    assert(arr);
    assert(el_size);

    return (void*) ((char*) arr + index*el_size);
}


void VoidSwap(void* arr, size_t el_size, size_t idx1, size_t idx2)
{
    assert(arr);
    assert(el_size);

    char temp = 0;

    char* el1 = (char*) VoidIndex(arr, el_size, idx1);
    char* el2 = (char*) VoidIndex(arr, el_size, idx2);

    for (size_t i = 0; i < el_size; i++)
    {
        temp = el1[i];
        el1[i] = el2[i];
        el2[i] = temp;
    }
}


void BubbleSort(void* arr, size_t el_size, size_t len,
                ORDER (*cmp)(const void*, const void*))
{
    assert(arr);
    assert(cmp);
    assert(el_size);

    if (len < 2) return;

    for (size_t i = 0; i < len - 1; i++)
    {
        bool wasSwapped = false;

        for (size_t j = 0; j < len - i - 1; j++)
            if (cmp(VoidIndex(arr, el_size, j), VoidIndex(arr, el_size, j+1))
                == SWAP)
            {
                wasSwapped = true;
                VoidSwap(arr, el_size, j, j+1);
            }

        if (!wasSwapped)
            break;
    }
}


size_t _QPartition(void* arr, size_t el_size,
                   ORDER (*cmp)(const void*, const void*),
                   size_t left, size_t right)
{
    assert(arr);
    assert(el_size);
    assert(cmp);
    assert(left < right);
    assert(right - left + 1 > 1);

    void* mid_el = calloc(1, el_size);
    assert(mid_el);

    //printf("L: %zd R: %zd Mid: %zd\n", left, right, (right + left)/2);

    memcpy(mid_el, VoidIndex(arr, el_size, (right + left)/2), el_size);


    while (left < right)
    {
        while (left < right
               && memcmp(VoidIndex(arr, el_size, left), mid_el, el_size)
               && cmp(VoidIndex(arr, el_size, left), mid_el) == OK) left++;
        while (left < right
               && memcmp(VoidIndex(arr, el_size, right), mid_el, el_size)
               && cmp(mid_el, VoidIndex(arr, el_size, right)) == OK) right--;

        // while (left < right && cmp(mid_el, VoidIndex(arr, el_size, left)) != OK) left++;
        //while (left < right && cmp(mid_el, VoidIndex(arr, el_size, right)) != SWAP) right--;
        //printf("%d %d\n", left, right);
        VoidSwap(arr, el_size, left, right);

    }


    free(mid_el);

    return left;
}


void _QSort(void* arr, size_t el_size,
            ORDER (*cmp)(const void*, const void*),
            size_t left, size_t right)
{
    size_t mid = _QPartition(arr, el_size, cmp, left, right);
    //printf("Partitioned L: %zd R: %zd - got mid %zd\n", left, right, mid);

    if (mid - left + 1 > 1)
        //(printf("%zd%zd: Calling 1\n", left, right),
        _QSort(arr, el_size, cmp, left, mid);//);

    if (right - (mid + 1) + 1 > 1)
        //(printf("%zd%zd: Calling 2\n", left, right),
        _QSort(arr, el_size, cmp, mid+1, right);//);
}


void QuickSort(void* arr, size_t el_size, size_t len,
                ORDER (*cmp)(const void*, const void*))
{
    _QSort(arr, el_size, cmp, 0, len-1);
}


bool Myisalpha(char a)
{
    //a:97 z:122 A:65 Z:90
    return ((a >= 65) && (a <= 90)) || ((a >= 97) && (a <= 122));
}


ORDER StrCompL2R(const void* a_void, const void* b_void)
{
    assert(a_void);
    assert(b_void);

    char* a = *((char**) a_void);
    char* b = *((char**) b_void);

    for (size_t i = 0, j = 0; a[i] && b[j]; i++, j++)
    {
        while (!isalpha(a[i]) && a[i]) i++;
        while (!isalpha(b[j]) && b[j]) j++;

        if (tolower(a[i]) > tolower(b[j]))
            return SWAP;
        else if (tolower(a[i]) < tolower(b[j]))
            return OK;
    }

    return OK;
}


void SortL2R(text5* txt)
{
    QuickSort(txt->lines, sizeof(char*), txt->nLines, &StrCompL2R);
}


ORDER StrCompR2L(const void* a_void, const void* b_void)
{
    assert(a_void);
    assert(b_void);

    char* a = *((char**) a_void);
    char* b = *((char**) b_void);

    for (int i = strlen(a) - 1, j = strlen(b) - 1; i > 0 && j > 0; i--, j--)
    {
        while (!isalpha(a[i]) && i > 0) i--;
        while (!isalpha(b[j]) && j > 0) j--;

        if (i < 0 || j < 0)
            return SWAP;


        if (tolower(a[i]) > tolower(b[j]))
            return SWAP;
        else if (tolower(a[i]) < tolower(b[j]))
            return OK;
    }

    return OK;
}


void SortR2L(text5* txt)
{
    QuickSort(txt->lines, sizeof(char*), txt->nLines, &StrCompR2L);
}


ORDER s1mple_cmp(const void* a, const void* b)
{
    assert(a);
    assert(b);

    return (*(long long unsigned*)a > *(long long unsigned*)b) ? SWAP : OK;
}


int main(void)
{
    long long unsigned l[5] = {133333333337, 420, 3, 1, 2};
    printf("%llu %llu %llu %llu %llu\n", l[0], l[1], l[2], l[3], l[4]);
    QuickSort(l, sizeof(l[0]), 5, &s1mple_cmp);
    printf("%llu %llu %llu %llu %llu\n", l[0], l[1], l[2], l[3], l[4]);


    const char* PUSHKIN = "evgenij_onegin_en.txt";

    text5 evgenij_onegin = {};
    int result = ReadFromFile(&evgenij_onegin, PUSHKIN);
    assert(!result);


    printf("\n======   NORMAL ORDER   ======\n");
    PrintSample(&evgenij_onegin, 30);

    printf("\n======   SORT L2R   ======\n");
    SortL2R(&evgenij_onegin);
    PrintSampleAlpha(&evgenij_onegin, 30);

    printf("\n======   SORT R2L   ======\n");
    SortR2L(&evgenij_onegin);
    PrintSampleAlpha(&evgenij_onegin, 300);

    TextKiller(&evgenij_onegin);
}
