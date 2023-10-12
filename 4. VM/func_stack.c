#include <stdlib.h>
#include <stdio.h>

#include "func_stack.h"
#include "colors.h"


typedef struct _node {
	stack_t value;
	struct _node* next;
} Node;

struct _stack {
	Node* head;
};


Stack* StackCtor() {
	Stack* stack = (Stack*) calloc(1, sizeof(Stack));
	stack->head = NULL;
	
	return stack;
}


int StackPush(Stack* stk, stack_t value) {
	Node* new_node = (Node*) malloc(sizeof(Node));
	if (!new_node) return -1;
	*new_node = (Node) {value, stk->head};
	
	stk->head = new_node;
	
	return 0;
}


int StackPop(Stack* stk, stack_t* value) {
	if (!stk->head) return -1;
	*value = stk->head->value;
	
	Node* temp = stk->head;
	stk->head = temp->next;
	free(temp);
	
	return 0;
}


int _stack_dtor(Node* node) {
	if (!node) return 0;
	
	int res1 = _stack_dtor(node->next);
	if (res1) return res1;
	
	free(node);
	
	return 0;
}


int StackDtor(Stack* stk) {
	int res = _stack_dtor(stk->head);
	if (res) return res;
	
	free(stk);
	
	return 0;
}


void _stack_dump(Node* node) {
	if (!node) return;
	
	printf("%hu ", node->value);
	
	_stack_dump(node->next);
}


int StackDump(Stack* stk) {
	printf(GREEN("Stack") "[" BLUE("%p") "] { ", stk);
	_stack_dump(stk->head);
	printf("}\n");
	
	return 0;
}
