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
    jc _printErr
    cmp ah,0
    jne _printErr
    cmp al,0
    je _printErr
    jmp _seg
_printErr:
    mov si,errMes
    mov cx, 512h
    mov ah ,0eh
    mov bh, 0
    mov bl, 0
_loopPrint:
    lodsb
    cmp al,0
    je _endPrint
    int 10h
    loop _loopPrint
_endPrint:
    cli
    hlt
errMes: db 'i can not read sector',0
times	510 - ($ - $$) db 0
db	0x55, 0xAA
_seg:
    ; запрет всех прерываний
    cli
    in   al, 70h
    or   al, 80h
    out  70h, al  ; запрет NMI
	
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
    mov al, 00010011b ; icw1, 1 кнтроллер icw3 не будет
    out 20h, al
    mov al, 00001000b ; icw2 стандартный int level 0, теперь таймер на 8м прерывании и все от него вверх
    out 21h, al
    mov al, 00000011b ; icw4 8088/8086, aeoi - чтоб не парится со сбросом isr
    out 21h, al
    mov al, 11111101b ; disable mask
    out 21h, al
   ; разрешаем аппаратные прерывания и NMI
    in   al, 70h
    and  al, 7Fh
    out  70h, al
    sti
	
    mov si, mesInt9
    mov cx, lenInt9rMes
    call print
    
_main_loop:
    hlt
    jmp _main_loop

_intLoopBack:
    mov si, mes2
    mov cx,	lenAnotherMes
    call print
    jmp _end
_0dh:
    pop  eax ; код ошибки надо вытолкнуть из стека
    mov si, mes
    mov cx, len0dhMes
    call print
_end:
    cli
    hlt

_int9:
    pusha
    push edi
    xor  ax, ax

    ; запрашиваем позиционный код клавиши
    in   al, 060h
    dec al
_continue_handling:
    ; отжатия не обрабатываем, только нажатия
    mov  ah, al
    and  ah, 80h
    jnz clear_request

    ; преобразуем позиционный код в ASCII по таблице
    and  al, 7Fh
    push edi
    mov  edi, ascii
    add  di, ax
    mov  al, [edi]
    pop  edi	
    mov [char], al
    mov si, char
    mov cx, 1
    call print

    ; посылка подтверждения обрабоки в порт клавиатуры
    ; (установка и сброс 7 бита порта 061h)
    in   al, 061h
    or   al, 80
    out  061h, al
    xor  al, 80
    out  061h, al

clear_request:
    pop  edi
    popa
    iret
	
print:
    pusha
    xor ax,ax
    mov ds,ax
    mov ax, 8*2
    mov es, ax
    mov di, [pos]
    mov	al, 02h
_loop:
    movsb
    stosb
    loop	_loop
    mov [pos], di
    popa
    ret
pos dw 0
char db 0
mes db 'it is a int 0dh',0
len0dhMes equ	$ - mes
mes2 db 'it is a another int',0
lenAnotherMes equ	$ - mes2
mesInt9 db 'Please write something: '
lenInt9rMes equ	$ - mesInt9
ascii    db 0,'1234567890-+',0,0,'QWERTYUIOP[]',0,0,'ASDFGHJKL;',"'`",0,0,'ZXCVBNM,./',\
            0,'*',0,' ',0, 0,0,0,0,0,0,0,0,0,0, 0,0, '789-456+1230.', 0,0
times	1024 - ($ - $$) db 0
GDT:
    times	8 db 0
     ;Дескриптор сегмента кода, размер сегмента 64 Кб, базовый адрес 0h, Execute/Read:
     ; прим base addr написан снизу вверх, например baseAddr=12345678h, то сверху вних байты 78h,56h,34h,12h
    db	0FFh        ; Segment Limit
    db	0FFh 
    db	00h         ; base address
    db	00h
    db	00h
    db	10011010b   ; 1001, C/D – 1, 0, R/W – 1, 0
    db	00000000b   ; G - 0, 000, Limit - 0000
    db	00h         ; base address
    ;Дескриптор сегмента данных для экрана
    db  00h
    db	10h
    db	00h
    db	80h
    db	0Bh
    db	10010010b
    db	00000000b
    db	00h
gdtLen equ	$ - GDT
IDT:
    %rep 9
	dw _intLoopBack
        dw 8
        db 0, 10000110b, 0, 0
    %endrep

    dw _int9
    dw 8
    db 0, 10000110b, 0, 0
	
    %rep 13 - (9+1)
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
