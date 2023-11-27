#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "latex.h"
#include "derivative.h"
#include "eval.h"

int main() {
    char buf[1024] = {};
    
    FILE* f = fopen("3.tree", "rt");
    assert(f);
    fread(buf, 1, 1024, f);
    fclose(f);
    
    printf("Read: [%s]\n", buf);
    
    Node* n = tree_parse(buf);
    
    tree_dump(n);
    
    n = derivative(n);
    
    tree_dump(n);
        
    printf("Eval: %lf\n", eval(n, 2));
    
    sprintf(buf, "aboba was here");
    
    tree_latex(buf, 1024, n);
    printf("Export: [%s]\n", buf);
    
    latex2pdf("output", buf);
    
    tree_delete(n);
}
