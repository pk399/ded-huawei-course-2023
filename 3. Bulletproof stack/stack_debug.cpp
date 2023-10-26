#include <assert.h>

#include "stack_internal.h"


#ifdef DEBUG
inline const char* InsideStackMarker(const Stack* stk, size_t i) {
	return (i < stk->size) ? "*" : " ";
}


void StackDump(const Stack* stk, FILE* stream, const char* caller_file, size_t caller_line, const char* caller_func) {
    assert(stk);

    error_t err = StackCheck(stk);

    fprintf(stream, "Stack [%p] ", stk);

    if (err & NO_DEBUG_DATA) {
    	#define RED_COLOR "\033[1;31m"
		#define NORMAL_COLOR "\033[1;0m"
    
        fprintf(stream, RED_COLOR "no debug data inside Stack" NORMAL_COLOR "\n");
        
        #undef RED_COLOR
        #undef NORMAL_COLOR
	} else
        fprintf(stream, "\"%s\" from %s(%zd) %s()\n",
               stk->debug.var, stk->debug.file, stk->debug.line, stk->debug.func);
    

    #define PAD "                       "
    fprintf(stream, PAD "called from %s(%zd) %s()\n\n", caller_file, caller_line, caller_func);    
    #undef PAD
    
    if (IgnoreError(err, NO_DEBUG_DATA)) {
        fprintf(stream, "Errors:\n");
		
		#define PRIFERR(err_val, mess) if (err & err_val) fprintf(stream, mess)
		
        PRIFERR(NULL_DATA,           "- Data pointer is NULL\n"); 
        PRIFERR(ZERO_CAP,            "- Capacity is zero\n");
        PRIFERR(SIZE_GT_CAP,         "- Size is greater than capacity\n");
        
        #ifdef CANARY
			PRIFERR(LEFT_STRUCT_CANARY,  "- Left struct canary is corrupted\n");
		    PRIFERR(RIGHT_STRUCT_CANARY, "- Right struct canary is corrupted\n");
		    PRIFERR(LEFT_DATA_CANARY,    "- Left data canary is corrupted\n");
		    PRIFERR(RIGHT_DATA_CANARY,   "- Right data canary is corrupted\n");
		#endif
		
		#ifdef HASH
			PRIFERR(STRUCT_HASH,         "- Struct hash mismatch\n");
			PRIFERR(DATA_HASH,           "- Data hash mistmatch\n");
		#endif
		
		#undef PRIFERR

        fputc('\n', stream);
    }

    fprintf(stream, "{\n");

    #define TAB "   " // Three spaces

    fprintf(stream, TAB "size=%zd\n", stk->size);
    fprintf(stream, TAB "capacity=%zd\n", stk->capacity);
    fprintf(stream, TAB "data[%p]\n", stk->data);

    if (stk->data) {
        fprintf(stream, TAB "{\n");

        for (size_t i = 0; i < stk->capacity; i++) {
            fprintf(stream, TAB TAB "%s[%zd]=" STACKTFORMAT,
                   InsideStackMarker(stk, i), i, stk->data[i]);

            if (stk->data[i] == STACK_POISON_VALUE)
                fprintf(stream, "(POISON)\n");
            else
                fputc('\n', stream);
        }

        fprintf(stream, TAB "}\n");
    }

    fprintf(stream, "}\n");
    
    #undef TAB
}
#endif
