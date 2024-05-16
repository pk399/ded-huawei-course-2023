#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "text.h"
#include "tree.h"
#include "prog.h"

#include "frontend.h"

const int DEFAULT_VAR_SZ = 64;
const int DEFAULT_FUNC_SZ = 16;
const int DEFAULT_CALLS_SZ = 4;

typedef struct {
	Func* f; // current func
	int vars_cap;

	int l; // level
	
	int r; // last return value
 	const char* e; // error, NULL for no err

	// Latest parse results
	char* id; // id (must free)
	Node* n; // Node, maybe with children (must free)
	int calls_sz;
	Node** calls; // #asdefwf[0][1][2 + 2 * 3]
} Ctx;

Func* parse_func(Ctx* c, Text* t);
int p_op(Ctx* c, Text* t);
int p_bye(Ctx* c, Text* t);
int p_vardecl(Ctx* c, Text* t);
int p_calls(Ctx* c, Text* t);
int p_if(Ctx* c, Text* t);
int p_loop(Ctx* c, Text* t);
int p_hashtag(Ctx* c, Text* t);
int p_matan(Ctx* c, Text* t);
int p_exp(Ctx* c, Text* t);
int p_sexp(Ctx* c, Text* t);
int p_mexp(Ctx* c, Text* t);
int p_eexp(Ctx* c, Text* t);
int p_pexp(Ctx* c, Text* t);
int p_id(Ctx* c, Text* t);
int p_iid(Ctx* c, Text* t);
int p_num(Ctx* c, Text* t);


Program* parse_file(const char* s) {
	Program* p = calloc(1, sizeof(Program));
	assert(p);
	p->funcs = calloc(DEFAULT_FUNC_SZ, sizeof(Func));
	assert(p->funcs);
	int psize = DEFAULT_FUNC_SZ;

	Ctx c = {};
	Text* t = tfromstr(s);

	int i = 0;
	Func* f = NULL;
	while (f = parse_func(&c, t)) {
		p->funcs = f;

		i++;
		if (i == p->size) {
			psize *= 2;
			p->funcs = realloc(p->funcs, psize * sizeof(Func));
			assert(p->funcs);
		}
	}

	p->size = i;

	Text* texp = pexport(p);
	tdump(texp);
	tdtor(texp);
	
	if (c.e) {
		tdump(t);
		printf("Result: %d, |%s|\n", c.r, c.e);
	}

	tdtor(t);

	return p;
}


// functions return int:
// 0 - didn't parse / err
// 1 - parsed

#define RETURN(c, v) (c)->r = (v); return (v)                         // RETURN: return this value
#define ERROR(c, s) (c)->e = s; RETURN((c), 0)                        // ERROR:  CERTAINLY parsing has failed
#define ASSERT(c) if (!(c)->r || (c)->e != NULL) { RETURN((c), 0); }  // ASSERT: this part MUST be parsed
#define CHECK(c) if ((c)->e != NULL) { RETURN((c), 0); }              // CHECK:  this part CAN be parsed

int _addvar(Ctx* c) {
	Var* v = c->f->vars + c->f->vars_size;

	v->name = strdup(c->id);
	v->ndims = c->calls_sz;
	if (c->calls_sz) {
		v->dims = calloc(c->calls_sz, sizeof(int));
		assert(v->dims);
	}
	for (int i = 0; i < c->calls_sz; i++) {
		assert(nisconstant(c->calls[i]));
		double res = NAN;
		ndeval(c->calls[i], &res);
		v->dims[i] = res;
	}

	c->f->vars_size++;
	if (c->f->vars_size == c->vars_cap) {
		c->vars_cap *= 2;
		c->f->vars = realloc(c->f->vars, c->vars_cap * sizeof(Var));
		assert(c->f->vars);
	}
}

Func* parse_func(Ctx* c, Text* t) {
	if (texpect(t, u8"нач")) {
		return NULL;
	}

	Func* f = calloc(1, sizeof(Func));
	assert(f);
	c->f = f;
	c->vars_cap = DEFAULT_VAR_SZ;
	f->vars = calloc(c->vars_cap, sizeof(Var));
	assert(f->vars);

	if (!p_id(c, t)) {
		//printf("Parsed id: |%s|\n", c->id);
		ERROR(c, "Expected ID as func name");
	}
	
	f->name = c->id;
	
	int i = 0;
	while (!tnewlined(t) && p_hashtag(c, t)) {
		if (c->calls_sz > 0) {
			ERROR(c, "Function args cannot have dimensions");
		}

		_addvar(c);
		i++;
	}
	CHECK(c);
	
	f->inputs = i;

	Node* ast = NULL;
	Node** n = &ast; // pointer to the bottom of ast
	while (1) {
		if (p_bye(c, t)) {
			*n = c->n;

			break;
		}
		CHECK(c);

		p_op(c, t);
		ASSERT(c); // Expected bye at the end of func
		*n = nconcat(c->n, NULL);
		n = &((*n)->r);
	}

	f->n = ast;

	return f;
}


int p_id(Ctx* c, Text* t) {
	int res = tparseid(t, &(c->id));
	RETURN(c, res);
}


int p_iid(Ctx* c, Text* t) {
	int res = tparseid(t, &(c->id));
	RETURN(c, res);
}


int p_hashtag(Ctx* c, Text* t) {
	if (texpect(t, u8"#")) {
		RETURN(c, 0);
	}

	p_vardecl(c, t);
	ASSERT(c);
	// ...
	//E(c, "Expected VarDecl in hashtag");

	RETURN(c, 1);
}


int p_vardecl(Ctx* c, Text* t) {
	if (!p_id(c, t)) {
		ERROR(c, "Expected variable name");
	}

	char* saved_id = c->id;

	p_calls(c, t);
	CHECK(c);

	c->id = saved_id;

	RETURN(c, 1);
}


int p_calls(Ctx* c, Text* t) {
	int buf_sz = 0;
	int calls_sz = 0;
	Node** calls = NULL;

	if (texpect(t, u8"[")) {
		// Shouldn't be necessary
		// but it does break
		c->calls_sz = 0;
		c->calls = NULL;
		RETURN(c, 0);
	}

	calls = calloc(DEFAULT_CALLS_SZ, sizeof(Node*));
	assert(calls);
	buf_sz = DEFAULT_CALLS_SZ;

	do {
		calls_sz++;

		if (calls_sz > buf_sz) {
			buf_sz *= 2;
			calls = realloc(calls, sizeof(Node*) * buf_sz);
			assert(calls);
		}

		p_exp(c, t);
		ASSERT(c);
		//E(c, "Expected Exp");
		calls[calls_sz - 1] = c->n;

		if (texpect(t, u8"]")) {
			ERROR(c, "Expected closing brace '['");
		}
	} while (!texpect(t, u8"["));

	assert(calls_sz > 0);
	assert(calls != NULL);
	c->calls_sz = calls_sz;
	c->calls = calls;
	RETURN(c, 1);
}


int p_exp(Ctx* c, Text* t) {
	bool is_display = 0;

	if (!texpect(t, u8"$")) {
		is_display = 1;
	}
	CHECK(c);

	if (p_matan(c, t)) {
		// matan takes care of it
		c->n->is_display = is_display;
		RETURN(c, 1);
	}
	CHECK(c);

	if (p_num(c, t)) {
		// num takes care of it
		c->n->is_display = is_display;
		RETURN(c, 1);
	}
	
	if (!p_iid(c, t)) {
		ERROR(c, "Expected something in expression");
	}

	const char* saved_id = c->id;

	p_calls(c, t);
	CHECK(c);

	c->n = ncall(saved_id, c->calls_sz, c->calls, is_display);

	RETURN(c, 1);
}


int p_matan(Ctx* c, Text* t) {
	if (texpect(t, u8"м(")) {
		RETURN(c, 0);
	}

	p_sexp(c, t);
	ASSERT(c);
	//E(c, "Expected expression in matan");

	// sexp takes care of it
	
	if (texpect(t, u8")")) {
		ERROR(c, "Expected closing brace ')' in matan");
	}

	RETURN(c, 1);
}


int p_sexp(Ctx* c, Text* t) {
	p_mexp(c, t);
	ASSERT(c);

	int run = 1;
	while (run) {
		Node* n = c->n; // previous node
		switch (ttrychar(t, "+-")) {
			case '+':
				p_mexp(c, t);
				ASSERT(c);
				//E(c, "Expected expression after plus '+'");

				c->n = nadd(n, c->n);
				break;
			case '-':
				p_mexp(c, t);
				ASSERT(c);
				//E(c, "Expected expression after minus '-'");

				c->n = nsub(n, c->n);
				break;
			default:
				run = 0;
		}
	}

	RETURN(c, 1);
}


int p_mexp(Ctx* c, Text* t) {
	p_eexp(c, t);
	ASSERT(c);

	int run = 1;
	while (run) {
		Node* n = c->n; // previous node
		switch (ttrychar(t, "*/")) {
			case '*':
				p_eexp(c, t);
				ASSERT(c);
				//E(c, "Expected expression after multiplication '*'");
				
				c->n = nmul(n, c->n);
				break;
			case '/':
				!p_eexp(c, t);
				ASSERT(c);
				//E(c, "Expected expression after division '/'");
				
				c->n = ndiv(n, c->n);
				break;
			default:
				run = 0;
		}
	}

	RETURN(c, 1);
}


int p_eexp(Ctx* c, Text* t) {
	Node* n = NULL;
	Node** attach_point = &n;

	if (!texpect(t, "-")) {
		n = nsub(nnum(0), NULL);
		attach_point = &(n->r);
	}

	p_pexp(c, t);
	ASSERT(c);
	//E(c, "Expected primary expression");
	*attach_point = c->n;

	if (!texpect(t, "^")) {
		p_pexp(c, t);
		ASSERT(c);
		//E(c, "Expected primary expression after power");
		c->n = npow(n, c->n);
	}

	// ... lol

	RETURN(c, 1);
}


int p_pexp(Ctx* c, Text* t) {
	if (!texpect(t, "(")) {
		p_exp(c, t);
		ASSERT(c);
		//E(c, "Nothing after bracket is open");

		// ...

		if (texpect(t, ")")) {
			ERROR(c, "Expected closing brace ')'");
		}

		RETURN(c, 1);
	}

	if (p_num(c, t)) {
		// ...
		
		RETURN(c, 1);
	}

	if (!p_iid(c, t)) {
		ERROR(c, "Need an expression");
	}

	const char* saved_id = c->id;

	p_calls(c, t);
	CHECK(c);

	c->n = ncall(saved_id, c->calls_sz, c->calls, false); // Don't display by default

	RETURN(c, 1);
}


int p_num(Ctx* c, Text* t) {
	long unsigned n = 0;

	int res = tparsenum(t, &n);
	if (!res) { RETURN(c, 0); }

	c->n = nnum(n);

	RETURN(c, 1);
}


int p_bye(Ctx* c, Text* t) {
	if (!texpect(t, u8"оке") || !texpect(t, u8"байбай")) {
		c->n = nbye(NULL, false);

		RETURN(c, 1);
	}

	if (!texpect(t, u8"всё")) {
		c->n = nbye(NULL, true);

		RETURN(c, 1);
	}

	if (!texpect(t, u8"это")) {
		p_exp(c, t);
		ASSERT(c);
		//NO(c, "Expected expression in return");

		c->n = nbye(c->n, false);
		
		RETURN(c, 1);
	}

	RETURN(c, 0);
}


int p_op(Ctx* c, Text* t) {
	if (p_bye(c, t)) {
		// ...
		
		RETURN(c, 1);
	}
	CHECK(c);

	if (!texpect(t, "assert(")) {
		p_sexp(c, t);
		ASSERT(c);
		//NO(c, "Assert needs expression");
		
		if (texpect(t, ")")) {
			ERROR(c, "Expected closing brace ')' in assert");
		}

		c->n = nassert(c->n);
		
		RETURN(c, 1);
	}

	if (!texpect(t, "$")) {
		p_exp(c, t);
		ASSERT(c);
		//NO(c, "Expected expression to display");
		
		c->n->is_display = true;
		
		RETURN(c, 1);
	}

	if (p_hashtag(c, t)) {
		c->n = ndecl(NULL, c->id, c->calls_sz, c->calls);
		_addvar(c);
		
		if (!texpect(t, "=")) {
			Node* prev_n = c->n;

			p_exp(c, t);
			ASSERT(c);
			//NO(c, "Need EXP");
			
			prev_n->l = c->n;
			c->n = prev_n;
		}

		RETURN(c, 1);
	}

	if (p_if(c, t)) {
		// ...
		
		RETURN(c, 1);
	}
	CHECK(c);

	if (p_loop(c, t)) {
		// ...
		
		RETURN(c, 1);
	}
	CHECK(c);


	if (!p_iid(c, t)) {
		ERROR(c, "Expected a statement (OP)");
	}

	p_calls(c, t);
	CHECK(c);

	if (texpect(t, "=")) {
		ERROR(c, "Expected a '='");
	}

	Node* new_n = nassign(NULL, c->id, c->calls_sz, c->calls);

	p_exp(c, t);
	ASSERT(c); 

	new_n->l = c->n;
	c->n = new_n;
	
	RETURN(c, 1);
}


int p_if(Ctx* c, Text* t) {
	bool is_seq = false; // if is located after another if

	if (!texpect(t, u8"ачо")) {
		is_seq = 1;
	} else if (!texpect(t, u8"чо")) {
		is_seq = 0;
	} else {
		RETURN(c, 0);
	}

	Node* if_n = NULL;

	if (!texpect(t, "?")) { // It's an else
		if_n = nif(NULL, NULL, IF_ELSE, is_seq);
	} else if (!texpect(t, u8"ели")) {
		p_exp(c, t);
		ASSERT(c);

		IFCOND cnd;
		if (!texpect(t, u8"один")) {
			cnd = IF_ONE;
		} else if (!texpect(t, u8"ноль")) {
			cnd = IF_ZERO;
		} else if (!texpect(t, u8"сущ")) {
			cnd = IF_NZERO;
		} else if (!texpect(t, ">0")) {
			cnd = IF_GZERO;
		} else if (!texpect(t, "<0")) {
			cnd = IF_LZERO;
		} else {
			ERROR(c, "Expected a condition");
		}
		
		if (texpect(t, "?")) {
			ERROR(c, "Expected a '?'");
		}

		if_n = nif(NULL, c->n, cnd, is_seq);
	} else {
		ERROR(c, "Ambiguous if statement");
	}

	Node* op_n = NULL;
	Node** attach_point = &op_n;

	while (1) {
		if (!texpect(t, "!")) {
			break;
		}

		p_op(c, t);
		ASSERT(c); // Expected '!' at the end of if

		if (*attach_point != NULL) {
			*attach_point = nconcat(*attach_point, NULL);
			attach_point = &((*attach_point)->r);
		}
		*attach_point = c->n;
	}

	if_n->l = op_n;
	c->n = if_n;

	RETURN(c, 1);
}


int p_loop(Ctx* c, Text* t) {
	if (texpect(t, u8"топ")) {
		RETURN(c, 0);
	}

	Node* loop_exp = NULL;

	if (texpect(t, u8"хз")) {
		p_exp(c, t);
		ASSERT(c); // Expected expression in loop
		
		loop_exp = c->n;
	}

	const char* loop_id = NULL;

	if (texpect(t, "_")) {
		if (!p_id(c, t)) {
			ERROR(c, "Expected variable in loop");
		}

		loop_id = c->id;
	}

	if (texpect(t, ":")) {
		ERROR(c, "Expected a ':'");
	}

	c->l++;
	
	Node* loop_op = NULL;
	Node** attach_point = &loop_op;

	int in_loop = 1;
	while (in_loop) {
		tsave(t);

		for (int i = 0; i < c->l; i++) {
			if (!p_num(c, t)) {
				trollback(t);
				in_loop = 0;
				break;
			}

			if (texpect(t, ".")) {
				ERROR(c, "Expected a '.'");
			}
		}

		if (in_loop) {
			p_op(c, t);
			ASSERT(c); // NEED EXP

			if (*attach_point != NULL) {
				*attach_point = nconcat(*attach_point, NULL);
				attach_point = &((*attach_point)->r);
			}
			*attach_point = c->n;
		}
	}

	c->l--;

	c->n = nloop(loop_op, loop_exp, loop_id);

	RETURN(c, 1);
}
