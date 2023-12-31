//    opcode   arg type  stk pushs
//          name    stk pops    code
DEF_CMD( 0, HLT , NOP, 0, 0, { HALT; })
DEF_CMD(11, PUSH, RDD, 0, 1, { to_push[0] = ARG; })
DEF_CMD( 1, POP , WRT, 1, 0, { ARG = poped[0]; })
DEF_CMD( 1, POP , NOP, 1, 0, { })
DEF_CMD( 2, IN  , NOP, 0, 1, { double tmp = 0; printf("?> "); scanf("%lf", &tmp); to_push[0] = tmp; })
DEF_CMD( 3, OUT , NOP, 1, 0, { printf( MAGENTA("%lf\n"), poped[0]); })
DEF_CMD( 4, ADD , NOP, 2, 1, { to_push[0] = poped[1] + poped[0]; })
DEF_CMD( 5, SUB , NOP, 2, 1, { to_push[0] = poped[1] - poped[0]; })
DEF_CMD( 6, MUL , NOP, 2, 1, { to_push[0] = poped[1] * poped[0]; })
DEF_CMD( 7, DIV , NOP, 2, 1, { to_push[0] = poped[1] / poped[0]; })
DEF_CMD( 8, SQRT, NOP, 1, 1, { to_push[0] = sqrt(poped[0]); })
DEF_CMD( 9, SIN , NOP, 1, 1, { to_push[0] = sin(poped[0]); })
DEF_CMD(10, COS , NOP, 1, 1, { to_push[0] = cos(poped[0]); })

DEF_CMD(12, JMP , RDD, 0, 0, { IP = ARG; })
#define a poped[0]
#define b poped[1]
#define CND_JMP(NUM, NAME, COND) DEF_CMD(NUM, NAME, RDD, 2, 0, { if (COND) { IP = ARG; } })
#define DEQ(a, b) (fabs(a - b) < DBL_EPSILON)
CND_JMP(13, JB  , (a < b))
CND_JMP(14, JBE , (a < b) || DEQ(a, b))
CND_JMP(15, JA  , (a > b))
CND_JMP(16, JAE , (a < b) || DEQ(a, b))
CND_JMP(17, JE  , DEQ(a, b))
CND_JMP(18, JNE , !DEQ(a, b))
#undef a
#undef b
#undef CND_JMP
#undef DEQ

DEF_CMD(19, CALL, RDD, 0, 0, { CALLME(ARG); })
DEF_CMD(20, RET , NOP, 0, 0, { REN; })
