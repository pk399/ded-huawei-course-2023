#include <string.h>

#include "latex.h"

#include "tree.h"
#include "smiley.h"
#include "defines.h"

#define LL(op) _tree_latex(buf, sz, L(n), op_prio(op));
#define LR(op) _tree_latex(buf, sz, R(n), op_prio(op));
#define BRACE(op, br) if (prev_prio > op_prio(op) || (prev_prio == op_prio(op) && (op) == OP_SUB)) {SHPRINTF(buf, sz, (br))}
#define LBRACE(op) BRACE(op, "(")
#define RBRACE(op) BRACE(op, ")")


int _tree_latex(char** buf, unsigned* sz, const Node* n, unsigned prev_prio) {
    if (!n) {
        SHPRINTF(buf, sz, "Naf");
        return 1;
    }    
    
    if (tree_check(n)) return 1;
    
    switch (n->type) {
        case LIT:
            SHPRINTF(buf, sz, "%.2lf", n->lit);
            break;
        case VAR:
            SHPRINTF(buf, sz, "x");
            break;
        case OP:
            switch (n->op) {
                #include "latex.include"

                default:
                    SHPRINTF(buf, sz, SMILEY "(op)");
            }
            break;
        default:
            SHPRINTF(buf, sz, SMILEY);
    }
    
    return 0;
}


int tree_latex(char* buf, unsigned sz, const Node* n) {
    SHPRINTF(&buf, &sz, "\\documentclass[12pt, a4paper]{article}\n\\begin{document}\n$$ ");
    int res = _tree_latex(&buf, &sz, n, NULL_PRIO);
    SHPRINTF(&buf, &sz, " $$ \n\\end{document}");
    
    return res;
}


void latex2pdf(const char* filename, const char* buf) { // TODO: Move into separate file?
    char buf2[BUF_SZ] = {};
    
    snprintf(buf2, BUF_SZ, "%s.tex", filename);
    FILE* f = fopen(buf2, "wt");
    if (!f) return; // TODO: -1
    fwrite(buf, 1, strlen(buf), f);
    fclose(f);

    snprintf(buf2, BUF_SZ, "pdflatex %s.tex", filename);
    f = popen(buf2, "r");
    if (!f) return;
    // Read, so the process doesn't block
    fread(buf2, 1, BUF_SZ, f);
    if (pclose(f)) return;
    
    snprintf(buf2, BUF_SZ, "rm %s.tex %s.log %s.aux", filename, filename, filename);
    f = popen(buf2, "r");
    if (!f) return;
    if (pclose(f)) return;
}