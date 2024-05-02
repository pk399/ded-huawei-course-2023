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

section .rodata
ALPH: db'0123456789abcdef'
 

section .bss
buf: resb BUF_SIZE
bw_buf: resb BUF_SIZE

section .text

global MAprintf ; My Assembly printf
; Args:
; rdi - STR pointer
; rsi, rdx, rcx, r8, r9, ... - format arguments
MAprintf:
	push r12 ; save required regs
	push r13
	push r14

	; Return value - chars written, inc by WrtTmpBuf
	xor rax, rax

	; r10 = buf_fill
	xor r10, r10; buf_fill = 0
	; r11 = pointer
	xor r11, r11 ; pointer = 0
	; r12 = next_char
	; r13 = arg_pointer
	xor r13, r13
	; r14 = next_arg
	; r15 = temp_arg


.cycle:
	call NextChar ; next_char = NextChar()

	test r12b, r12b ; next_char == '\0' ?
	jz .done

	cmp r12b, '%' ; next_char == '%' ?
	jz .phandler

	call WrtTmpBuf

	jmp .cycle

.phandler:
	call NextChar

	; switch (next_char)
	cmp r12b, '%'
	jz .sw_percent
	cmp r12b, 'c'
	jz .sw_char
	cmp r12b, 's'
	jz .sw_str
	cmp r12b, 'x'
	jz .sw_hex
	cmp r12b, 'o'
	jz .sw_octal
	cmp r12b, 'd'
	jz .sw_decimal
	cmp r12b, 'b'
	jz .sw_binary
	; Default (quit)
	jmp .done

.sw_percent:
	call WrtTmpBuf ; write '%'
	jmp .sw_end

.sw_char:
	call NextArg
	mov r12b, r14b
	call WrtTmpBuf
	jmp .sw_end

.sw_str:
	call NextArg
	.s_loop: ; while (*next_arg != '\0') write *next_arg;
		mov r12b, [r14]
		
		test r12b, r12b
		jz .sw_end

		call WrtTmpBuf

		inc r14
		jmp .s_loop

.sw_hex:
	call NextArg
	mov r15, 16
	call BaseWriter
	jmp .sw_end

.sw_octal:
	call NextArg
	mov r15, 8
	call BaseWriter
	jmp .sw_end

.sw_decimal:
	call NextArg
	mov r15, 10 ; base = 10
	call BaseWriter
	jmp .sw_end

.sw_binary:
	call NextArg
	mov r15, 2
	call BaseWriter
	jmp .sw_end
	
.sw_end:
	jmp .cycle

.done:
	call FlushTmpBuf

	pop r14
	pop r13
	pop r12

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
	inc rax ; return++

	cmp r10, BUF_SIZE ; buf_fill < BUF_SIZE ?
	jnz .skip
	call FlushTmpBuf

	.skip:
	ret


; Get next_arg, increment arg_pointer
NextArg:
	; shifted, because of first arg being STR pointer
	cmp r13, 5 ; arg_pointer < 5 ?
	jg .sw_stack
	
	xor r15, r15
	mov r15, r13
	shl r15, 2
	add r15, r13
	add r15, .sw
	jmp r15 ; jump to: .sw + arg_pointer*5

.sw:
	mov r14, rsi
	jmp .sw_end
	mov r14, rdx
	jmp .sw_end
	mov r14, rcx
	jmp .sw_end
	mov r14, r8
	jmp .sw_end
	mov r14, r9
	jmp .sw_end

.sw_stack:
	mov r15, r13
	sub r15, 5 ; arg_pointer - 5
	mov r14, [rsp + 8*r15 + (5 * 8)] ; skip RETADDR R14 R13 R12 RETADD -> stack args


.sw_end:
	inc r13
	ret


; Write number r14 in base r15; destroy r14, r12
BaseWriter:
	push rdx
	push rax

	mov rax, r14
	xor r14, r14

	.cycle:
		xor rdx, rdx
		div r15 ; number/base

		mov r12b, [ALPH + rdx]
		mov [bw_buf + r14], r12b
		
		inc r14
		cmp r14, BUF_SIZE
		jz .end ; just quit

		test rax, rax
		jz .end

		jmp .cycle

	.end:

	pop rax

	.wrt_cycle:
		dec r14
		mov r12b, [bw_buf + r14]
		call WrtTmpBuf

		test r14, r14
		jnz .wrt_cycle

	pop rdx
	ret
