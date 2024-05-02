.286
.model tiny
.code
org 100h

VideoMem equ 0b800h

; TODO: normal int + toggle

Start:
		push 0
		pop es
		mov bx, 4 * 08h
		cli
		mov es:[bx], offset Timer08Int
		push cs
		pop ax
		mov es:[bx+2], ax
		sti

		mov ax, 3100h
		mov dx, offset EOP
		add dx, 100h
		shr dx, 4
		inc dx
		int 21h

Timer08Int		proc
		push ax

		
		;in al, 60h
		;cmp al, 1bh
		;jne NoMatch

        pop ax		

        push ax bx cx dx si di bp sp ds ss es 
        mov bp, sp
		
        mov ax, cs ; right 
        mov ds, ax

		mov ax, VideoMem
	mov es, ax
    
    mov ah, 65 ; x = 65
    mov al, 2  ; y = 2
    mov dh, 9 ; width = 9
    mov dl, 15 ; height = 15
    mov ch, 70h ; color - white on black
    mov cl, 70h ; shadow color ^
    mov bl, 2   ; style 2
    call DrawFrameShadowSave

    ;jmp Skip

    xor di, di
    mov di, 22d

RegLoop:
    sub di, 2
    
    mov dx, [bp+di]                         ; reg value

    mov bl, 66                              ; x = 66 (todo: unhardcode)

    mov ax, 20d
    sub ax, di
    shr ax, 1                               
    add ax, 3                               ; y = this

    mov cx, word ptr [di + offset RegNames] ; name
    call WriteReg
    
    cmp di, 0d
    je Skip

    jmp RegLoop


Skip:       pop es ss ds sp bp di si dx cx bx
NoMatch:	
        mov al, 20h
		out 20h, al
        pop ax


        iret
		endp

include wrtreg

include rmkinc

RegNames:   db 'ESSSDSSPBPDISIDXCXBXAX'

EOP:
end		Start
