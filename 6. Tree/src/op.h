//     n       prio      eval
//         name    argc
OP_DEF(0 , ADD , 3, 2, EL + ER)
OP_DEF(1 , SUB , 2, 2, EL - ER)
OP_DEF(2 , MUL , 4, 2, EL * ER)
OP_DEF(3 , DIV , 1, 2, EL / ER)
OP_DEF(4 , SQRT, 1, 1, sqrt(EL))
OP_DEF(5 , SIN , 1, 1, sin(EL))
OP_DEF(6 , COS , 1, 1, cos(EL))
OP_DEF(7 , PI  , 0, 0, M_PI)
OP_DEF(8 , POW , 1, 2, pow(EL, ER))
OP_DEF(9 , LOG , 1, 2, log(ER) / log(EL))
OP_DEF(10, LN  , 1, 1, log(EL))
OP_DEF(11, E   , 0, 0, 2.7182818284590452)