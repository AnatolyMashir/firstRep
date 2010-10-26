	org 7c00h
	xor ax,ax
	cli
    mov ds,ax
	mov es,ax
	sti
	mov ah,2
	mov al,1
	mov cx,2
	mov dh,0
	mov dl,0
	mov bx, mes
	int 13h
	mov cx, 512h
	mov ah ,0eh
	mov bh, 0
	mov bl, 0
	mov si, mes
_loop:
	lodsb
	cmp al,0
	je _end
	int 10h
	loop _loop
_end:
	cli
	hlt
times	510 - ($ - $$) db 0
db 55h, 0aah
mes:
db 'burya mglou nebo kroet, vihri serie krutya',0
times	1024 - ($ - $$) db 0
