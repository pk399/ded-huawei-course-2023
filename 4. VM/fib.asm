jmp main



fib:
pop rbx

push 2
push rbx
jbe one

push rbx
push rbx
push 1
sub
call fib
pop rcx
pop rbx

push rcx
push rbx
push 2
sub
call fib

add
ret

one:
push 1
ret



main:
push 27
pop rax

loop:

;push rax
;out

push rax
call fib
out


push rax
push 1
sub
pop rax
push rax
push 1
jbe loop

hlt
