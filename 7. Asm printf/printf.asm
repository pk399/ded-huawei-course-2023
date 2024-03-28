; int MAprintf(char* fmtstr, ...)   My Asm printf
; mods: %% %d %o %x !!!%b %s %c!!!
; input: null-terminated format string with mods (modifiers)
; output: characters written
;
; Algorithm:
;          main:
;            |
;     setup temp buff
;|---------->|
;|    read next char
;|           |
;|       is it "\0"? -y--> done:
;|           |n
;|       is it "%"?
;|     |---n-|-y----|
;| wrt tmp buf:  %handler:
;|     |-----|------|
;|           |
;|--<<-cycle-|
;
;
;        %handler:
;            |         !!! TODO: getarg: N, gets the argument, from regs or stack
;         SWITCH --- | % -> wrt temp buf: "%" - ret
;                    |
;                    | d -> base writer: 10 - ret
;                    |
;                    | o -> base writer: 8 - ret
;                    |
;                    | x -> base writer: 16 - ret
;                    |
;                    | (default) -> done: !!! Think aboud cleaning up the stack
;
;
;       wrt tmp buf: TODO: please flush, it will be two pointer one - chars written, other - tmpbuf fill
;            |
;        pos >= LEN?
;      |---n-|-y----|
;     wrt         done:
;      |            | !!! Think about cleaning up the stack (or don't ._. if ret is somehow jmp)
;    pos++
;      |
;     ret
;
;
;       base writer: db ALPH '0123456789abcdef'
;|---------->|
;|     v = in % base
;|           |
;|  wrt temp buf: ALPH[v]
;|           |
;|      in = in / base
;|           |
;|        in > 0? -n--> ret
;|           |y
;|--<<-cycle-|
;
;
;         done:
;           |
;  TODO: please write
;           |
;    clean up stuff
;           |
;       ret to c

%define BUF_SIZE 1024

%define SYS_write 0x1
section .bss
buf: resb BUF_SIZE

section .text

global MAprintf ; My Assembly printf
; Args:
; rdi - STR pointer
; rsi, rdx, rcx, r8, r9, ... - format arguments
MAprintf:
	; r10 = buf_fill
	xor r10, r10; buf_fill = 0
	; r11 = pointer
	xor r11, r11 ; pointer = 0
	; r12 = next_char

.cycle:
	call NextChar ; next_char = NextChar()

	test r12, r12 ; next_char == '\0' ?
	jz .done

	call WrtTmpBuf

	jmp .cycle

.done:
	call FlushTmpBuf
	ret


; Writes next_char, increments pointer
NextChar:
	mov r12b, [rdi + r11] ; next_char = fmt[pointer]
	inc r11 ; pointer++
	ret


; Write buf to stdout, reset buf_fill
FlushTmpBuf:
	push rax ; Save regs
	push rdi
	push rsi
	push rdx

	mov eax, SYS_write
	mov rdi, 0x1 ; stdout
	mov rsi, buf
	mov rdx, r10 ; buf_fill
	syscall

	xor r10, r10 ; buf_fill = 0
	
	pop rdx
	pop rsi
	pop rdi
	pop rax
	ret


; Write next_char to buf, flush if reached the end
WrtTmpBuf:
	mov [buf + r10], r12b ; buf[buf_fill] = next_char
	inc r10 ; buf_fill

	cmp r10, BUF_SIZE ; buf_fill < BUF_SIZE ?
	jnz .skip
	call FlushTmpBuf

	.skip:
	ret
