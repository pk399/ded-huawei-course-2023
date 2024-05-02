.model tiny
.code
.286
org 100h

; DOS screen size: 80w * 25h * 2bytes 

VideoMem equ 0b800h

Start:
	mov ax, VideoMem
	mov es, ax

    
    mov ah, 10 ; x = 10
    mov al, 5  ; y = 5
    mov dh, 60 ; width = 60
    mov dl, 15 ; height = 15
    mov ch, 70h ; color - white on black
    mov cl, 70h ; shadow color ^
    mov bl, 2   ; style 2
    call DrawFrameShadow
    
	
	mov ah, 4ch		
	int 21h		; goodbye_world()

include RMKINC

end		Start
