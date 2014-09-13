entry _main
export _main
_main:
	; preparamos stack, arranca en 0x70000 y es de ~64k
	mov 	ax, #0x7000
	mov 	ss, ax
	mov 	ax, #0xfff0
	mov 	sp, ax

	mov	cl, #0x02
	;mov	dl, #0x00 ; LA BIOS CARGA EL DRIVE NUMBER EN DL AUTOMAGICAMENTE
	mov	dh, #0x00
	mov	ch, #0x00
	mov	bx, #0x7000
	mov	es, bx
	mov	bx, #0
	mov	ah, #0x02
	mov	al, #0x14 ; cant sectores a cargar (20)
	int	#0x13
	jc	_main_loaderr
	mov	bx, #0x7000
	mov	ds, bx

_main_loaddone:
	jmpi	#0, #0x7000

_main_loaderr:
	push	#0x0c
	add	ah, #0x41
	shr	ax, #8
	push	ax
	call	_printcharadv
	add	sp, #4

	push	#0x0f
	push	#0x45
	call	_printcharadv
	add	sp, #4
	push	#0x0f
	push	#0x52
	call	_printcharadv
	add	sp, #4
	push	#0x0f
	push	#0x52
	call	_printcharadv
	add	sp, #4
	call	_newline
_main_halt:
	jmp	_main_halt

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
