1. Frontend
    1. Correctly model parser logic
    2. Make Text
    3. Check if working
    4. ... A lot of work
        1. Tree nodes
            1. Exp: +, -, *, /, ^, Num, Call, IId
            2. OP: Id, Decl, Assign, Loop, If
        2. Function table (array of trees)
        3. ID (variables) table for functions
    5. Make Tree, import / export
2. Middleend
    1. Dummy for now
    2. At the end return and port some simple opts (nisconstant, neval) - easy
3. Backend
    1. Make simple IR :)
    2. Export to SPU asm (should be easy at this point)
    3. make nasm file, little bit harder
    4. Make 
    5. export to elf64