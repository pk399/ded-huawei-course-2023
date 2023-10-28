in
pop rax
in
pop rbx
in
pop rcx

push 0
push rbx
sub
pop xxx

push rax
push 0
je linear

push rbx
push rbx
mul
push 4
push rax
push rcx
mul
mul
sub
pop rdx

push 0
push rdx
je D=0

push 0
push rdx
jb D<0

push rdx
sqrt
pop rdx

push xxx
push rdx
add
push 2
push rax
mul
div
out

push xxx
push rdx
sub
push 2
push rax
mul
div
out

hlt

D=0:
push xxx
push 2
push rax
mul
div
out
hlt

D<0:
push -1337
out
hlt

linear:
push rbx
push 0
je none

push xxx
push rax
div
out
hlt


none:
push 0
push rcx
jne nosaul

push 1337
out
hlt

nosaul:
push -1337
out
hlt
