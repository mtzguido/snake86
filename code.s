entry _main
_main:
	call	_mainloop
a:	jmp	a

export _printchar
export _printcharadv
export _cursoradv
export _printnum
export _newline
export _getcur
export _setcur
export _drawpix
export _setvideo
export _readkey
export _gettime

; Imprime un caracter en la posicion del cursor
; void printchar(char c, char attr)
_printchar:
	push	bp
	mov	bp, sp
	push	bx
	
	mov	al, 4[bp]
	mov	ah, #0x9
	mov	bl, 6[bp]
	mov	cx, #0x1
	mov	bh, #0x0
	int	#0x10

	pop	bx
	pop	bp
	ret

; Analoga a printchar, pero avanza el cursor
; void printcharadv(char c, char attr)
_printcharadv:
	push	bp
	mov	bp, sp

	push	6[bp]
	push	4[bp]
	call	_printchar
	add	sp, #4
	call	_cursoradv

	mov	sp, bp
	pop	bp
	ret

; Avanza el cursor, cambiando de linea
; y haciendo wrap-around si se llega al final
; void cursoradv()
_cursoradv:
	push	bp
	mov	bp, sp
	push	bx

	mov	ah, #0x03
	mov	bx, #0
	int	#0x10
	inc	dl
	cmp	dl, #80
	jne	_cursoradv_done
	
	inc	dh
	mov	dl, #0
	cmp	dh, #24
	jne	_cursoradv_done
	mov	dh, #0

_cursoradv_done:
	mov	bx, #0
	mov	ah, #0x02
	int	#0x10

	pop	bx
	mov	sp, bp
	pop	bp
	ret

; Devuelve la posicion del cursor
; int getcur()
; msb: row, lsb: col
_getcur:
	push	bx
	mov	ah, #0x03
	mov	bx, #0
	int	#0x10
	mov	ax, dx
	pop	bx
	ret

; Setea el cursor
; void setcur(int)
; msb: row, lsb: col
_setcur:
	push	bp
	mov	bp, sp
	push	bx

	mov	bx, #0
	mov	ah, #0x02
	mov	dx, 4[bp]
	int	#0x10

	pop	bx
	mov	sp, bp
	pop	bp
	ret

; Imprime un numero desde el cursor
; void printnum(int n, char attr, int minlen)
_printnum:
	push	bp
	mov	bp, sp
	push	bx

	mov	ax, 4[bp]
	mov	bx, 6[bp]
	mov	cx, #10

	push	#10 ; nunca puede ser un resultado valido
_printnum_div:
	mov	dx, #0
	div	cx
	push	dx
	dec	8[bp]
	cmp	ax, #0
	jne	_printnum_div

_printnum_print0:
	cmp	8[bp],#0
	jle	_printnum_show
	push	#0
	dec	8[bp]
	jmp	_printnum_print0

_printnum_show:
	pop	cx
	cmp	cx, #10
	je	_printnum_done
	add	cx, #0x30 ; 0x30 == '0' 
	push	bx
	push	cx
	call	_printcharadv ; no tiene por que mantener ax,cx,dx
	add	sp, #4
	jmp	_printnum_show

_printnum_done:
	pop	bx
	mov	sp, bp
	pop	bp
	ret

; Avanza de linea
; void newline()
_newline:
	push	bx
	mov	ah, #0x03
	mov	bh, #0
	int	#0x10
	inc	dh
	cmp	dh, #24
	jne	_newline_ok
	mov	dh, #0

_newline_ok:
	mov	dl, #0
	mov	bh, #0
	mov	ah, #0x02
	int	#0x10
	pop	bx
	ret
; Dibujar pixel
; void drawpix(int x, int y, char col)
_drawpix:
	push	bp
	mov	bp, sp
	push	bx
	
	mov	ah, #0x0c
	mov	bh, #0
	mov	al, 8[bp]
	mov	dx, 6[bp]
	mov	cx, 4[bp]
	int	#0x10

	pop	bx
	mov	sp, bp
	pop	bp
	ret

; Setea modo de video 320x240 (256 col)
; void setvideo()
_setvideo:
	mov	ah, #0x00
	mov	al, #0x13
	int	#0x10
	ret


; Similar a getchar
; inv readkey()
_readkey:
	mov	ah, #0x01
	int	#0x16
	jz	_readkey_nochar
	mov	ah, #0x00
	int	#0x16
	xor	ah, ah
	ret
_readkey_nochar:
	mov	ax, #65535
	ret

; Devuelve un int con una cant de clock ticks (approx 18.2 per sec)
; int gettime()
_gettime:
	mov	ah, #0x00
	int	#0x1a
	mov	ax, dx
	ret
