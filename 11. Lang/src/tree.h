#ifndef _TREE_H_
#define _TREE_H_

#include <stdbool.h>

#include "text.h"

// TODO
// 1. ~~u8 n strings~~
// 2. better naming Ctx -> Context, c -> ctx
// 3. ~~Loop: seperate ids by space~~
// 4. Execute in buffer with nmap
// 5. Docs

typedef enum {
	NT_OP_CONCAT,
		// l - OP
		// r - next OP
	NT_OP_DECL,
		// id
		// calls
		// l - EXP (if assigning)
	NT_OP_ASSIGN,
		// id
		// calls
		// l - EXP (must be)
	NT_OP_LOOP,
		// l - EXP - loop count, or NULL - infinite
		// r - OP
		// id - var name, NULL - nothing
	NT_OP_IF,
		// cond
		// is_seq
		// l - EXP
		// r - OP
	NT_OP_ASSERT,
		// l - EXP
	NT_OP_BYE,
		// is_break
		// l - EXP (if any)
	NT_OP_OUT,
		// l - EXP

	// all exp can have is_display
	NT_EXP_NUM,
		// num
	NT_EXP_CALL,
		// id
		// calls
	NT_EXP_ADD,
		// l + r - EXP
	NT_EXP_SUB,
		// l - r - EXP
	NT_EXP_MUL,
		// l * r - EXP
	NT_EXP_DIV,
		// l / r - EXP
	NT_EXP_POW
		// l ^ r - EXP
} NODETYPE;

typedef enum {
	IF_ELSE,
	IF_ONE,
	IF_ZERO,
	IF_NZERO,
	IF_GZERO,
	IF_LZERO
} IFCOND;

typedef struct _node {
	struct _node* l;
	struct _node* r;

	NODETYPE t;

	bool is_display;

	union {
		long unsigned num;

		struct {
			const char* id;

			int calls_sz; // 0 - nothing, >1 - something
			struct _node** calls;
		};

		// for IF
		struct {
			IFCOND cond;
			bool is_seq;
		};

		// for BYE
		bool is_break;
	};
} Node;

typedef struct {
	Node* root;
} Tree;

// Ctors
Node* nctor(NODETYPE t);
Node* nconcat(Node* l, Node* r);
Node* ndecl(Node* exp, const char* id, int calls_sz, Node** calls);
Node* nassign(Node* exp, const char* id, int calls_sz, Node** calls);
Node* nloop(Node* l, Node* exp, const char* id);
Node* nif(Node* l, Node* exp, IFCOND cond, bool is_seq);
Node* nassert(Node* exp);
Node* nbye(Node* exp, bool is_break);
Node* nout(Node* exp);
Node* nnum(long unsigned num);
Node* ncall(const char* id, int calls_sz, Node** calls, bool is_display);
Node* nadd(Node* l, Node* r);
Node* nsub(Node* l, Node* r);
Node* nmul(Node* l, Node* r);
Node* ndiv(Node* l, Node* r);
Node* npow(Node* l, Node* r);

int nexport(Text* t, Node* n);

int nisconstant(Node* n);
int ndeval(Node* n, double* result);

int ndump(Node* n);
int ndtor(Node* n);

#endif /* _TREE_H_ */
