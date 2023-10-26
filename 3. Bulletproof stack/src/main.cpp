#include <stdio.h>
#include "stack_include.h"

int main() {
    Stack stk = {};
    printf("Prepating\n");
    STACKCTOR(&stk);
    
    printf("Constructed\n");
    STACKDUMP(&stk);

	for (int i = 1; i <= 11; i++) {
		StackPush(&stk, i*10);
		printf("Push\n");
	}

	//stk.data[10] = 1337;

	//*DataHashPtr(stk.data) = 12334;

	//((char*) stk.data)[-5] = 'E';
	//stk.data[64] = 1337;

	STACKDUMP(&stk);
	//stk.capacity = 3;
	for (int j = 0; j < 8; j++) {
		stack_t val = 0;
		StackPop(&stk, &val);
		printf(STACKTFORMAT "\n", val);
	}

	//stk.size = 5;
	
    //STACKDUMP(&stk);

    StackDtor(&stk);
}
