	.model tiny
	.code
	org 100h
_1:
	mov ah ,0eh
	mov bh, 0
	mov bl, 0
	mov al, 'H'
	int 10h
	mov al, 'e'
	int 10h
	mov al, 'l'
	int 10h
	mov al, 'l'
	int 10h
	mov al, 'o'
	int 10h
	xor ax,ax
	cli
	push ax
	push ax
	push ax
	pop ds
	pop sp
	pop ss
	sti
	cli
	hlt
a db 472 dup(0)
b db 55h, 0aah
end _1
