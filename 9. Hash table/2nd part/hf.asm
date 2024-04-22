; Hash function on SSE4.2 CRC
; nasm

section .text

global hf

hf: ; input: rsi - 256bit string ptr; output: rax - hash
	mov rax, 1337
	crc32 rax, qword [rdi]
	crc32 rax, qword [rdi + 1*8]
	crc32 rax, qword [rdi + 2*8]
	crc32 rax, qword [rdi + 3*8]
	ret
