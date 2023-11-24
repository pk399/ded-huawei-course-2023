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
    
    printf("Read: [%s]", buf);
    
    Node* n = tree_parse(buf);
    
    tree_dump(n);
    
    tree_delete(n);
}
