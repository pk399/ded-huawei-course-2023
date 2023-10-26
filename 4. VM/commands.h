#define TWO_POPS    ARG_T a = ARG_ZERO; \
					ARG_T b = ARG_ZERO; \
										\
					RELAY( DO_POP(a) ); \
					RELAY( DO_POP(b) );

// DEF_CMD??
DC(-1, HLT,  0, {
	HALT;
})

//  cmd_code  cmd_name, arg_type,   code
// DC( 0,     PUSH,       1,        ... )

// DC( 0, PUSH, IMM, {} )
// DC( 0, PUSH, REG, {} )


DC( 0, PUSH, 1, {
	switch (PT) {
		case A_REG: {
			RELAY( DO_PUSH(REG(ARG)) );
			break;
		}
		case A_LIT: {
			RELAY( DO_PUSH(ARG) );
			break;
		}
		default:
			ERR("Push reached default case!");
			CANT;
	}
})

DC( 1, POP,  1, {
	switch (PT) {
		case A_REG: {
			ARG_T val = ARG_ZERO;
			RELAY( DO_POP(val) );
			REG(ARG) = val;
			break;
		}
		case A_LIT: {
			WARN("Pop received literal argument!");
			break;
		}
		default:
			ERR("Pop reached default case!");
			CANT;
	}
})

DC( 2, IN,   0, {
	long int val = 0;
	scanf("%ld", &val);
	RELAY( DO_PUSH(IARG(val)) );
})

DC( 3, OUT,  0, {
	ARG_T val = ARG_ZERO;
	RELAY( DO_POP(val) );
	printf("%ld\n", I(val));
})

DC( 4, ADD,  0, {
	TWO_POPS;
	
	RELAY( DO_PUSH(IARG( I(a) + I(b) )) );
})

DC( 5, SUB,  0, {
	TWO_POPS;
	
	RELAY( DO_PUSH(IARG( I(b) - I(a) )) );
})

DC( 6, MUL,  0, {
	TWO_POPS;
	
	RELAY( DO_PUSH(IARG( I(b) * I(a) )) );
})

DC( 7, DIV,  0, {
	TWO_POPS
	
	if (I(a) == 0) {
		ERR("Division by zero!");
		CANT;
	}
	
	RELAY( DO_PUSH(IARG( I(b) / I(a) )) );
})

DC( 8, SQRT, 0, {
	WARN("Sqrt not implemented!");
})

DC( 9, SIN,  0, {
	WARN("Sin not implemented!");
})

DC(10, COS,  0, {
	WARN("Cos not implemented!");
})

