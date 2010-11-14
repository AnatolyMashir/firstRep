	org 7c00h
	xor ax,ax
	cli
	mov ds,ax
	mov es,ax
	sti

	mov ah,2
	mov al,6
	mov cx,2
	mov dh,0
	mov dl,0
	mov bx, _seg
	int 13h
	jmp _seg
times	510 - ($ - $$) db 0
db	0x55, 0xAA
_seg:
	cli
	LIDT [IDT_Load]
	LGDT [GDT_Load]
	mov bx,	1
	LMSW bx

	mov	ax, 8*2
	mov	ss, ax
	jmp	8:_code
GDT_Load:
	dw	gdtLen
	dw	GDT
	dw	0
IDT_Load:
	dw	256*8
	dw	IDT
	dw	0
_code:
	sti
	int	0dh

_intLoopBack:
	mov si, mes2
	mov cx,	lenAnotherMes
	jmp _print
_0dh:
	mov si, mes
	mov cx, len0dhMes
_print:
	mov ax, 8*2
	mov es, ax
	xor di, di
	mov ds, di
	mov	al, 02h
_loop:
	movsb
	stosb
	loop	_loop
_end:
	cli
	hlt
mes:
db 'it is a int 0dh',0
len0dhMes equ	$ - mes
mes2:
db 'it is a another int',0
lenAnotherMes equ	$ - mes2
times	1024 - ($ - $$) db 0
GDT:
	times	8 db 0
    ;Дескриптор сегмента кода, размер сегмента 64 Кб, базовый адрес 0h, Execute/Read:
	; прим base addr написан снизу вверх, например baseAddr=12345678h, то сверху вних байты 78h,56h,34h,12h

	db      0FFh        ; Segment Limit
	db      0FFh 
	db      00h         ; base address
	db      00h
	db      00h
	db      10011010b   ; 1001, C/D – 1, 0, R/W – 1, 0
	db      00000000b   ; G - 0, 000, Limit - 0000
	db      00h         ; base address
	;Дескриптор сегмента данных для экрана
	db      00h
	db		10h
	db		00h
	db		80h
	db		0Bh
	db		10010010b
	db		00000000b
	db		00h
gdtLen equ	$ - GDT
IDT:
	%rep 13
		dw _intLoopBack
        	dw 8
	        db 0, 10000110b, 0, 0
	%endrep

	dw _0dh
	dw 8
	db 0, 10000110b, 0, 0

	%rep 256 - 14
        	dw _intLoopBack
	        dw 8
        	db 0, 10000110b, 0, 0
	%endrep
