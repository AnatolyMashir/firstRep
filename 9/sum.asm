global sum
sum:
	push ebp
	mov ebp, esp
	mov ebx, [ebp+4+4]
	mov eax,[ebx]
	mov ebx, [ebp+4+4+4]
	add eax, [ebx]
	pop ebp
	ret
