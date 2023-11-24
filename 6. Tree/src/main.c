#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"

int main() {
    char buf[1024] = {};
    
    FILE* f = fopen("1.tree", "rt");
    assert(f);
    fread(buf, 1, 1024, f);
    fclose(f);
    
    printf("Read: [%s]\n", buf);
    
    Node* n = tree_parse(buf);
    
    sprintf(buf, "aboba was here");
    
    tree_export(buf, 1024, n);
    printf("Export: [%s]\n", buf);
    
    tree_delete(n);
}
