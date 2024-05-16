#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "text.h"

#include "tree.h"


Node* nctor(NODETYPE t) {
	Node* n = calloc(1, sizeof(Node));
	if (!n) return NULL;

	n->t = t;

	return n;
}


Node* nconcat(Node* l, Node* r) {
	//assert(l);
	//assert(r);

	Node* n = nctor(NT_OP_CONCAT);
	assert(n);
	
	n->l = l;
	n->r = r;

	return n;
}


Node* ndecl(Node* exp, const char* id, int calls_sz, Node** calls) {
	assert(id);
	assert(calls_sz >= 0 );

	Node* n = nctor(NT_OP_DECL);
	assert(n);

	n->l = exp;
	n->id = id;
	n->calls_sz = calls_sz;
	n->calls = calls;

	return n;
}


Node* nassign(Node* exp, const char* id, int calls_sz, Node** calls) {
	//assert(exp);
	assert(id);
	//assert(calls_sz >= 0);
	//assert(calls);

	Node* n = nctor(NT_OP_ASSIGN);
	assert(n);

	n->l = exp;
	n->id = id;
	n->calls_sz = calls_sz;
	n->calls = calls;

	return n;
}


Node* nloop(Node* l, Node* exp, const char* id) {
	assert(l);

	Node* n = nctor(NT_OP_LOOP);
	assert(n);

	n->l = l;
	n->r = exp;
	n->id = id;

	return n;
}


Node* nif(Node* l, Node* exp, IFCOND cond, bool is_seq) {
	//assert(l);
	//assert(exp);

	Node* n = nctor(NT_OP_IF);
	assert(n);

	n->l = l;
	n->r = exp;
	n->cond = cond;
	n->is_seq = is_seq;

	return n;
}


Node* nassert(Node* exp) {
	assert(exp);

	Node* n = nctor(NT_OP_ASSERT);
	assert(n);

	n->l = exp;

	return n;
}


Node* nbye(Node* exp, bool is_break) {
	Node* n = nctor(NT_OP_BYE);
	assert(n);

	n->l = exp;
	n->is_break = is_break;

	return n;
}


Node* nout(Node* exp) {
	assert(exp);

	Node* n = nctor(NT_OP_OUT);
	assert(n);

	n->l = exp;

	return n;
}


Node* nnum(long unsigned num) {
	Node* n = nctor(NT_EXP_NUM);
	assert(n);

	n->num = num;

	return n;
}


Node* ncall(const char* id, int calls_sz, Node** calls, bool is_display) {
	assert(id);
	assert(calls_sz >= 0);

	Node* n = nctor(NT_EXP_CALL);
	assert(n);

	n->id = id;
	n->calls_sz = calls_sz;
	n->calls = calls;
	n->is_display = is_display;

	return n;
}


Node* _nlr(NODETYPE t, Node* l, Node* r) {
	assert(l);
	//assert(r);
	
	Node* n = nctor(t);
	assert(n);

	n->l = l;
	n->r = r;

	return n;
}


Node* nadd(Node* l, Node* r) {
	return _nlr(NT_EXP_ADD, l, r);
}


Node* nsub(Node* l, Node* r) {
	return _nlr(NT_EXP_SUB, l, r);
}


Node* nmul(Node* l, Node* r) {
	return _nlr(NT_EXP_MUL, l, r);
}


Node* ndiv(Node* l, Node* r) {
	return _nlr(NT_EXP_DIV, l, r);
}


Node* npow(Node* l, Node* r) {
	return _nlr(NT_EXP_POW, l, r);
}


const char* _nt2str(NODETYPE t) {
	switch (t) {
		case NT_OP_CONCAT:
			return " ; ";

		case NT_OP_DECL:
			return "# =";
			
		case NT_OP_ASSIGN:
			return "  =";
			
		case NT_OP_LOOP:
			return "{-}";
			
		case NT_OP_IF:
			return " ? ";
			
		case NT_OP_ASSERT:
			return "ass";
			
		case NT_OP_BYE:
			return "bye";
			
		case NT_OP_OUT:
			return " $ ";
			
		case NT_EXP_NUM:
			return " 1 ";
			
		case NT_EXP_CALL:
			return "[ ]";

		case NT_EXP_ADD:
			return " + ";
			
		case NT_EXP_SUB:
			return " - ";
			
		case NT_EXP_MUL:
			return " * ";
			
		case NT_EXP_DIV:
			return " / ";
			
		case NT_EXP_POW:
			return " ^ ";
		
		default:
			return "E!!";
	}
}


int _ndump(Node* n, int level) {
	if (!n) return 0;

	for (int i = 0; i < level - 1; i++) {
		printf(" | ");
	}

	if (level > 0) {
		printf(" |-");
	}

	printf("%s ", _nt2str(n->t));
	if (n->t == NT_EXP_CALL) printf("%s ", n->id);
	if (n->calls_sz) printf("[%d] ", n->calls_sz);
	putchar('\n');

	_ndump(n->l, level + 1);
	_ndump(n->r, level + 1);

	return 0;
}


int ndump(Node* n) {
	_ndump(n, 0);

	return 0;
}


char _nt2char(NODETYPE t) {
	switch (t) {
		case NT_OP_CONCAT:
			return ';';

		case NT_OP_DECL:
			return '#';
			
		case NT_OP_ASSIGN:
			return '=';
			
		case NT_OP_LOOP:
			return '{';
			
		case NT_OP_IF:
			return '?';
			
		case NT_OP_ASSERT:
			return 'a';
			
		case NT_OP_BYE:
			return 'b';
			
		case NT_OP_OUT:
			return '$';
			
		case NT_EXP_NUM:
			return 'n';
			
		case NT_EXP_CALL:
			return '[';

		case NT_EXP_ADD:
			return '+';
			
		case NT_EXP_SUB:
			return '-';
			
		case NT_EXP_MUL:
			return '*';
			
		case NT_EXP_DIV:
			return '/';
			
		case NT_EXP_POW:
			return '^';
		
		default:
			return 'E';
	}
}


int _nexport(Text* t, Node* n) {
	if (!n) {
		tprintf(t, "nil ");
		return 0;
	}

	tprintf(t, "%c ", _nt2char(n->t));

	tprintf(t, "%1d ", n->is_display);

	if (n->t == NT_OP_BYE) {
		tprintf(t, "%1d ", n->is_break);
	}

	if (n->t == NT_EXP_NUM) {
		tprintf(t, "%lu ", n->num);
	}

	if (n->t == NT_OP_IF) {
		tprintf(t, "%1d %1d ", n->is_seq, n->cond);
	}

	if (n->t == NT_EXP_CALL || n->t == NT_OP_ASSIGN || n->t == NT_OP_DECL) {
		tprintf(t, "%d ", n->calls_sz);

		for (int i = 0; i < n->calls_sz; i++) {
			_nexport(t, n->calls[i]);
		}
	}

	if (n->t == NT_EXP_CALL || n->t == NT_OP_LOOP
		|| n->t == NT_OP_ASSIGN || n->t == NT_OP_DECL) {
		tprintf(t, "%s ", n->id);
	}

	_nexport(t, n->l);
	_nexport(t, n->r);
}


int nexport(Text* t, Node* n) {
	_nexport(t, n);
}


int nisconstant(Node* n) {
	if (!n) {
		return 1;
	}

	switch (n->t) {
		case NT_EXP_NUM:
			return 0;

		case NT_EXP_ADD:
		case NT_EXP_SUB:
		case NT_EXP_MUL:
		case NT_EXP_DIV:
			if (nisconstant(n->l)) return 1;
			if (nisconstant(n->r)) return 1;
			return 0;

		default:
			return 1;
	}
}


int ndeval(Node* n, double* result) {
	if (!n) {
		*result = NAN;
		return 1;
	}

	switch (n->t) {
		case NT_EXP_NUM:
			*result = n->num;
			return 0;

		double d1, d2;
		case NT_EXP_ADD:
			if (ndeval(n->l, &d1)) return 1;
			if (ndeval(n->r, &d2)) return 1;
			*result = d1 + d2;
			return 0;
		case NT_EXP_SUB:
			if (ndeval(n->l, &d1)) return 1;
			if (ndeval(n->r, &d2)) return 1;
			*result = d1 - d2;
			return 0;
		case NT_EXP_MUL:
			if (ndeval(n->l, &d1)) return 1;
			if (ndeval(n->r, &d2)) return 1;
			*result = d1 * d2;
			return 0;
		case NT_EXP_DIV:
			if (ndeval(n->l, &d1)) return 1;
			if (ndeval(n->r, &d2)) return 1;
			*result = d1 / d2;
			return 0;
		default:
			return 1;
	}
}


int ndtor(Node* n) {
	if (!n) return 0;

	ndtor(n->l);
	ndtor(n->r);

	if (n->id) free((void*) n->id);
	for (int i = 0; i < n->calls_sz - 1; i++) {
		ndtor(n->calls[i]);
	}
	if (n->calls) free(n->calls);
	free(n);

	return 0;
}
