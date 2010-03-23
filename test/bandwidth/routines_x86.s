
; ============================================================================
;  bandwidth 0.15, a benchmark to estimate memory transfer bandwidth.
;  Copyright (C) 2005,2007,2008 by Zack T Smith.
;
;  This program is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this program; if not, write to the Free Software
;  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;
;  The author may be reached at fbui@comcast.net.
; =============================================================================

global	Get_CPUID

global	L1_reader
global	L1_writer

global	L2_reader
global	L2_writer

global	Main_reader
global	Main_writer

global	Framebuffer_reader
global	Framebuffer_writer

global	my_bzero
global	my_memcpy

global 	Has_MMX

	section .data

tmp:	dw	1,2,3,4

	section .text

Get_CPUID:
	mov	eax, [esp+8]	; 2nd param
	cpuid
	push	esi
	mov	esi, [esp+8]	; 8 is due to push
	mov	[esi], ebx
	mov	[esi+4], edx
	mov	[esi+8], ecx
	pop	esi
	ret

Has_MMX:
	push	ebx
	push	ecx
	push	edx
	mov	eax, 1
	cpuid	
	and	edx, 800000h
	mov	eax, edx
	pop	edx
	pop	ecx
	pop	ebx
	ret

L1_writer:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do
	mov	eax, 12345678

myloop:
	mov	[edx], eax
	mov	[4+edx], eax
	mov	[8+edx], eax
	mov	[12+edx], eax
	mov	[16+edx], eax
	mov	[20+edx], eax
	mov	[24+edx], eax
	mov	[28+edx], eax
	mov	[32+edx], eax
	mov	[36+edx], eax
	mov	[40+edx], eax
	mov	[44+edx], eax
	mov	[48+edx], eax
	mov	[52+edx], eax
	mov	[56+edx], eax
	mov	[60+edx], eax
	mov	[64+edx], eax
	mov	[68+edx], eax
	mov	[72+edx], eax
	mov	[76+edx], eax
	mov	[80+edx], eax
	mov	[84+edx], eax
	mov	[88+edx], eax
	mov	[92+edx], eax
	mov	[96+edx], eax
	mov	[100+edx], eax
	mov	[104+edx], eax
	mov	[108+edx], eax
	mov	[112+edx], eax
	mov	[116+edx], eax
	mov	[120+edx], eax
	mov	[124+edx], eax
	mov	[128+edx], eax
	mov	[132+edx], eax
	mov	[136+edx], eax
	mov	[140+edx], eax
	mov	[144+edx], eax
 	loop 	myloop

	ret

L1_reader:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do

myloop2:
	mov	eax, [edx]
	mov	eax, [4+edx]
	mov	eax, [8+edx]
	mov	eax, [12+edx]
	mov	eax, [16+edx]
	mov	eax, [20+edx]
	mov	eax, [24+edx]
	mov	eax, [28+edx]
	mov	eax, [32+edx]
	mov	eax, [36+edx]
	mov	eax, [40+edx]
	mov	eax, [44+edx]
	mov	eax, [48+edx]
	mov	eax, [52+edx]
	mov	eax, [56+edx]
	mov	eax, [60+edx]
	mov	eax, [64+edx]
	mov	eax, [68+edx]
	mov	eax, [72+edx]
	mov	eax, [76+edx]
	mov	eax, [80+edx]
	mov	eax, [84+edx]
	mov	eax, [88+edx]
	mov	eax, [92+edx]
	mov	eax, [96+edx]
	mov	eax, [100+edx]
	mov	eax, [104+edx]
	mov	eax, [108+edx]
	mov	eax, [112+edx]
	mov	eax, [116+edx]
	mov	eax, [120+edx]
	mov	eax, [124+edx]
	mov	eax, [128+edx]
	mov	eax, [132+edx]
	mov	eax, [136+edx]
	mov	eax, [140+edx]
	mov	eax, [144+edx]
 	loop 	myloop2
	ret

L2_reader:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do

myloop1b:
	mov	ebx, edx
	add	ebx, 131072	; 128k 
myloop1a:
	sub	ebx, 128

	mov	eax, [ebx]
	mov	eax, [4+ebx]
	mov	eax, [8+ebx]
	mov	eax, [12+ebx]
	mov	eax, [16+ebx]
	mov	eax, [20+ebx]
	mov	eax, [24+ebx]
	mov	eax, [28+ebx]
	mov	eax, [32+ebx]
	mov	eax, [36+ebx]
	mov	eax, [40+ebx]
	mov	eax, [44+ebx]
	mov	eax, [48+ebx]
	mov	eax, [52+ebx]
	mov	eax, [56+ebx]
	mov	eax, [60+ebx]
	mov	eax, [64+ebx]
	mov	eax, [68+ebx]
	mov	eax, [72+ebx]
	mov	eax, [76+ebx]
	mov	eax, [80+ebx]
	mov	eax, [84+ebx]
	mov	eax, [88+ebx]
	mov	eax, [92+ebx]
	mov	eax, [96+ebx]
	mov	eax, [100+ebx]
	mov	eax, [104+ebx]
	mov	eax, [108+ebx]
	mov	eax, [112+ebx]
	mov	eax, [116+ebx]
	mov	eax, [120+ebx]
	mov	eax, [124+ebx]

	cmp	ebx,edx
	jne	myloop1a

	loop	myloop1b

	ret

L2_writer:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do
	mov	eax, 0x55555555

myloop4b:
	lea	ebx, [edx+131072]

myloop4a:
	sub	ebx, 128

	mov	[ebx], eax
	mov	[4+ebx], eax
	mov	[8+ebx], eax
	mov	[12+ebx], eax
	mov	[16+ebx], eax
	mov	[20+ebx], eax
	mov	[24+ebx], eax
	mov	[28+ebx], eax
	mov	[32+ebx], eax
	mov	[36+ebx], eax
	mov	[40+ebx], eax
	mov	[44+ebx], eax
	mov	[48+ebx], eax
	mov	[52+ebx], eax
	mov	[56+ebx], eax
	mov	[60+ebx], eax
	mov	[64+ebx], eax
	mov	[68+ebx], eax
	mov	[72+ebx], eax
	mov	[76+ebx], eax
	mov	[80+ebx], eax
	mov	[84+ebx], eax
	mov	[88+ebx], eax
	mov	[92+ebx], eax
	mov	[96+ebx], eax
	mov	[100+ebx], eax
	mov	[104+ebx], eax
	mov	[108+ebx], eax
	mov	[112+ebx], eax
	mov	[116+ebx], eax
	mov	[120+ebx], eax
	mov	[124+ebx], eax

	cmp	ebx,edx
	jnz	myloop4a

	loop	myloop4b

	ret

Main_reader:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do

myloop5b:
	lea	ebx, [edx+16777216]

myloop5a:
	sub	ebx, 128

	mov	eax, [ebx]
	mov	eax, [4+ebx]
	mov	eax, [8+ebx]
	mov	eax, [12+ebx]
	mov	eax, [16+ebx]
	mov	eax, [20+ebx]
	mov	eax, [24+ebx]
	mov	eax, [28+ebx]
	mov	eax, [32+ebx]
	mov	eax, [36+ebx]
	mov	eax, [40+ebx]
	mov	eax, [44+ebx]
	mov	eax, [48+ebx]
	mov	eax, [52+ebx]
	mov	eax, [56+ebx]
	mov	eax, [60+ebx]
	mov	eax, [64+ebx]
	mov	eax, [68+ebx]
	mov	eax, [72+ebx]
	mov	eax, [76+ebx]
	mov	eax, [80+ebx]
	mov	eax, [84+ebx]
	mov	eax, [88+ebx]
	mov	eax, [92+ebx]
	mov	eax, [96+ebx]
	mov	eax, [100+ebx]
	mov	eax, [104+ebx]
	mov	eax, [108+ebx]
	mov	eax, [112+ebx]
	mov	eax, [116+ebx]
	mov	eax, [120+ebx]
	mov	eax, [124+ebx]

	cmp	ebx,edx
	jne	myloop5a

	dec	ecx
	jnz	myloop5b

	ret

Main_writer:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do
	mov	eax, 0xAAAAAAAA

myloop6b:
	lea	ebx, [edx+16777216]

myloop6a:
	sub	ebx, 128

	mov	[ebx], eax
	mov	[4+ebx], eax
	mov	[8+ebx], eax
	mov	[12+ebx], eax
	mov	[16+ebx], eax
	mov	[20+ebx], eax
	mov	[24+ebx], eax
	mov	[28+ebx], eax
	mov	[32+ebx], eax
	mov	[36+ebx], eax
	mov	[40+ebx], eax
	mov	[44+ebx], eax
	mov	[48+ebx], eax
	mov	[52+ebx], eax
	mov	[56+ebx], eax
	mov	[60+ebx], eax
	mov	[64+ebx], eax
	mov	[68+ebx], eax
	mov	[72+ebx], eax
	mov	[76+ebx], eax
	mov	[80+ebx], eax
	mov	[84+ebx], eax
	mov	[88+ebx], eax
	mov	[92+ebx], eax
	mov	[96+ebx], eax
	mov	[100+ebx], eax
	mov	[104+ebx], eax
	mov	[108+ebx], eax
	mov	[112+ebx], eax
	mov	[116+ebx], eax
	mov	[120+ebx], eax
	mov	[124+ebx], eax

	cmp	ebx,edx
	jne	myloop6a

	dec	ecx
	jnz	myloop6b

	ret


Framebuffer_reader:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do

myloop7b:
	mov	ebx, [esp+12]	; length
	add	ebx, edx	; end of area to read.

myloop7a:
	sub	ebx, 128

	mov	eax, [ebx]
	mov	eax, [4+ebx]
	mov	eax, [8+ebx]
	mov	eax, [12+ebx]
	mov	eax, [16+ebx]
	mov	eax, [20+ebx]
	mov	eax, [24+ebx]
	mov	eax, [28+ebx]
	mov	eax, [32+ebx]
	mov	eax, [36+ebx]
	mov	eax, [40+ebx]
	mov	eax, [44+ebx]
	mov	eax, [48+ebx]
	mov	eax, [52+ebx]
	mov	eax, [56+ebx]
	mov	eax, [60+ebx]
	mov	eax, [64+ebx]
	mov	eax, [68+ebx]
	mov	eax, [72+ebx]
	mov	eax, [76+ebx]
	mov	eax, [80+ebx]
	mov	eax, [84+ebx]
	mov	eax, [88+ebx]
	mov	eax, [92+ebx]
	mov	eax, [96+ebx]
	mov	eax, [100+ebx]
	mov	eax, [104+ebx]
	mov	eax, [108+ebx]
	mov	eax, [112+ebx]
	mov	eax, [116+ebx]
	mov	eax, [120+ebx]
	mov	eax, [124+ebx]

	cmp	ebx,edx
	jne	myloop7a

	dec	ecx
	jnz	myloop7b

	ret

Framebuffer_writer:

	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; loops to do
	xor	eax, eax

myloop8b:
	mov	ebx, [esp+12]	; length
	add	ebx, edx	; end of area to fill.

myloop8a:
	sub	ebx, 128

	mov	[ebx], eax
	mov	[4+ebx], eax
	mov	[8+ebx], eax
	mov	[12+ebx], eax
	mov	[16+ebx], eax
	mov	[20+ebx], eax
	mov	[24+ebx], eax
	mov	[28+ebx], eax
	mov	[32+ebx], eax
	mov	[36+ebx], eax
	mov	[40+ebx], eax
	mov	[44+ebx], eax
	mov	[48+ebx], eax
	mov	[52+ebx], eax
	mov	[56+ebx], eax
	mov	[60+ebx], eax
	mov	[64+ebx], eax
	mov	[68+ebx], eax
	mov	[72+ebx], eax
	mov	[76+ebx], eax
	mov	[80+ebx], eax
	mov	[84+ebx], eax
	mov	[88+ebx], eax
	mov	[92+ebx], eax
	mov	[96+ebx], eax
	mov	[100+ebx], eax
	mov	[104+ebx], eax
	mov	[108+ebx], eax
	mov	[112+ebx], eax
	mov	[116+ebx], eax
	mov	[120+ebx], eax
	mov	[124+ebx], eax

	add	eax, 0x01020301

	cmp	ebx,edx
	jne	myloop8a

	dec	ecx
	jnz	myloop8b

	ret

;=========================================================
;
my_bzero:
	mov	edx, [esp+4]	; ptr to memory chunk
	mov	ecx, [esp+8]	; length

myloop9a:
	cmp	ecx, 64
	jb	myloop9c

	mov	[edx], eax
	mov	[edx+4], eax
	mov	[edx+8], eax
	mov	[edx+12], eax
	mov	[edx+16], eax
	mov	[edx+20], eax
	mov	[edx+24], eax
	mov	[edx+28], eax
	mov	[edx+32], eax
	mov	[edx+36], eax
	mov	[edx+40], eax
	mov	[edx+44], eax
	mov	[edx+48], eax
	mov	[edx+52], eax
	mov	[edx+56], eax
	mov	[edx+60], eax
	add	edx, 64
	sub	ecx, 64
	jz	myloop9d
	jnz	myloop9a

myloop9c:
	cmp	ecx, 16
	jb	myloop9e

	mov	[edx], eax
	mov	[edx+4], eax
	mov	[edx+8], eax
	mov	[edx+12], eax
	add	edx, 16
	sub	ecx, 16
	jz	myloop9d
	jnz	myloop9c

myloop9e:
	cmp	ecx, 0
	je	myloop9d

myloop9b:
	mov	[edx], al
	add	edx, 1
	loop	myloop9b

myloop9d:
	ret

;=========================================================
;
my_memcpy:
	mov	edx, [esp+4]	; ptr to dest memory chunk
	mov	ebx, [esp+8]	; ptr to src memory chunk
	mov	ecx, [esp+12]	; length

myloop0c:
	cmp	ecx, 16
	jb	myloop0e

	mov	eax, [ebx]
	mov	[edx], eax
	mov	eax, [ebx+4]
	mov	[edx+4], eax
	mov	eax, [ebx+8]
	mov	[edx+8], eax
	mov	eax, [ebx+12]
	mov	[edx+12], eax
	add	edx, 16
	add	ebx, 16
	sub	ecx, 16
	jz	myloop0d
	jnz	myloop0c

myloop0e:
	cmp	ecx, 0
	je	myloop0d

myloop0b:
	mov	al, [ebx]
	mov	[edx], al
	add	edx, 1
	add	ebx, 1
	loop	myloop0b

myloop0d:
	ret

