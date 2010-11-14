	org 7c00h
	xor ax,ax
	cli
    mov ds,ax
	mov es,ax
	sti
	mov ah,2
	mov al,2
	mov cx,2
	mov dh,0
	mov dl,0
	mov bx, _seg
	int 13h
	jmp _seg
times	510 - ($ - $$) db 0
db 55h, 0aah
_seg:
LIDT [IDT_load]
LGDT [GDT_load]
mov bx, 1
SMSW  bx
jmp 0000:_code
IDT_load:
dw 104
db 0
dw IDT
db 0
GDT_load:
dw 16
db 0
dw GDT
db 0
GDT:
resb 8
dw 1024
db 0,
dw _seg
db 10001011b
dw 0
_code:
	int 0dh
	jmp _0dh
_intLoopBack:
	mov si, mes
	jmp _print
_0dh:
	mov si, mes
_print:
	xor di,di
	mov ds,di
	mov ax, 0b800h
	mov es,ax
	mov cx, 12
_loop:
	movsb
	inc di
	loop _loop
_end:
	cli
	hlt
mes:
db 'burya mglou nebo kroet, vihri serie krutya',0
mes2:
db 'lalala',0
times	1024 - ($ - $$) db 0
IDT:
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _intLoopBack
db 0, 00001000
db 0
db 10000110b
dw 0
dw _0dh
db 0, 00001000
db 0
db 10000110b
dw 0
times	1536 - ($ - $$) db 0
