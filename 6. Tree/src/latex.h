#ifndef _LATEX_H_
#define _LATEX_H_

#include "tree.h"

int tree_latex(char* buf, unsigned sz, const Node*);
void latex2pdf(const char* filename, const char* buf);

#endif /* _LATEX_H_ */