bits 16

; Start image after one row, to avoid overwriting our code!
mov dl, 0
y_loop_start:
	
	mov cl, 0
	x_loop_start:
		; Fill pixel
		mov byte [bp + 0], dl ; Red
        mov byte [bp + 1], dl ; Green
		mov byte [bp + 2], cl ; Blue
		mov byte [bp + 3], 255 ; Alpha
			
		; Advance pixel location
		add bp, 4
			
		; Advance X coordinate and loop
		add cl, 1
		cmp cx, 127
		jnz x_loop_start
	
	; Advance Y coordinate and loop
	add dl, 1
	cmp dl, 127
	jnz y_loop_start