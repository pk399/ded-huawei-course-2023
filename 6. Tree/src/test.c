#include <stdio.h>

int main() {
    char buf[15];
    
    int res = snprintf(buf, 15, "%d", 123333333);
    
    printf("Res = %d\n Buf: [%s]\n", res, buf);
    
}
