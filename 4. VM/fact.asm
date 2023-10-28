jmp main

fact:
pop rax

push rax
push 1
je one

push rax
push rax
push 1
sub
call fact
push rax
mul
pop rax
ret

one:
push 1
pop rax
ret

main:
push 5
call fact
push rax
out

push 6
call fact
push rax
out

hlt
