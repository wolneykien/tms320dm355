
# ============================================================================
#  bandwidth 0.15a, a benchmark to estimate memory transfer bandwidth.
#  ARM assembly module.
#  Copyright (C) 2010 by Zack T Smith.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#  The author may be reached at fbui@comcast.net.
# =============================================================================

#--------------
# Version 0.1
#--------------

.section code 

.global L1_writer
.global L1_reader
.global L2_writer
.global L2_reader
.global Main_writer
.global Main_reader

L1_writer:
# save r4 and return address.
	stmfd   sp!, {r4, r14}	
	mov	r4, #127

myloop:
	str	r4, [r0]
	str	r4, [r0, #4]
	str	r4, [r0, #8]
	str	r4, [r0, #12]
	str	r4, [r0, #16]
	str	r4, [r0, #20]
	str	r4, [r0, #24]
	str	r4, [r0, #28]
	str	r4, [r0, #32]
	str	r4, [r0, #36]
	str	r4, [r0, #40]
	str	r4, [r0, #44]
	str	r4, [r0, #48]
	str	r4, [r0, #52]
	str	r4, [r0, #56]
	str	r4, [r0, #60]
	str	r4, [r0, #64]
	str	r4, [r0, #68]
	str	r4, [r0, #72]
	str	r4, [r0, #76]
	str	r4, [r0, #80]
	str	r4, [r0, #84]
	str	r4, [r0, #88]
	str	r4, [r0, #92]
	str	r4, [r0, #96]
	str	r4, [r0, #100]
	str	r4, [r0, #104]
	str	r4, [r0, #108]
	str	r4, [r0, #112]
	str	r4, [r0, #116]
	str	r4, [r0, #120]
	str	r4, [r0, #124]
	str	r4, [r0, #128]
	str	r4, [r0, #132]
	str	r4, [r0, #136]
	str	r4, [r0, #140]
	str	r4, [r0, #144]

	sub	r1, r1, #1
	bgt	myloop

# restore & return.
	ldmfd   sp!, {r4, r15}	

L1_reader:
# save r4 and return address.
	stmfd   sp!, {r4, r14}	

myloop2:
	ldr	r4, [r0]
	ldr	r4, [r0, #4]
	ldr	r4, [r0, #8]
	ldr	r4, [r0, #12]
	ldr	r4, [r0, #16]
	ldr	r4, [r0, #20]
	ldr	r4, [r0, #24]
	ldr	r4, [r0, #28]
	ldr	r4, [r0, #32]
	ldr	r4, [r0, #36]
	ldr	r4, [r0, #40]
	ldr	r4, [r0, #44]
	ldr	r4, [r0, #48]
	ldr	r4, [r0, #52]
	ldr	r4, [r0, #56]
	ldr	r4, [r0, #60]
	ldr	r4, [r0, #64]
	ldr	r4, [r0, #68]
	ldr	r4, [r0, #72]
	ldr	r4, [r0, #76]
	ldr	r4, [r0, #80]
	ldr	r4, [r0, #84]
	ldr	r4, [r0, #88]
	ldr	r4, [r0, #92]
	ldr	r4, [r0, #96]
	ldr	r4, [r0, #100]
	ldr	r4, [r0, #104]
	ldr	r4, [r0, #108]
	ldr	r4, [r0, #112]
	ldr	r4, [r0, #116]
	ldr	r4, [r0, #120]
	ldr	r4, [r0, #124]
	ldr	r4, [r0, #128]
	ldr	r4, [r0, #132]
	ldr	r4, [r0, #136]
	ldr	r4, [r0, #140]
	ldr	r4, [r0, #144]

	sub	r1, r1, #1
 	bgt	myloop2

# restore & return.
	ldmfd   sp!, {r4, r15}	

L2_reader:
# r0 = address
# r1 = count

# save r4 and return address.
	stmfd   sp!, {r4, r5, r6, r14}	

	add	r0, r0, #131072
	mov	r5, r1
	mov	r6, r0

myloop3b:
	sub	r0, r0, #128
	mov	r1, r5

myloop3a:
	ldr	r4, [r0]
	ldr	r4, [r0]
	ldr	r4, [r0, #4]
	ldr	r4, [r0, #8]
	ldr	r4, [r0, #12]
	ldr	r4, [r0, #16]
	ldr	r4, [r0, #20]
	ldr	r4, [r0, #24]
	ldr	r4, [r0, #28]
	ldr	r4, [r0, #32]
	ldr	r4, [r0, #36]
	ldr	r4, [r0, #40]
	ldr	r4, [r0, #44]
	ldr	r4, [r0, #48]
	ldr	r4, [r0, #52]
	ldr	r4, [r0, #56]
	ldr	r4, [r0, #60]
	ldr	r4, [r0, #64]
	ldr	r4, [r0, #68]
	ldr	r4, [r0, #72]
	ldr	r4, [r0, #76]
	ldr	r4, [r0, #80]
	ldr	r4, [r0, #84]
	ldr	r4, [r0, #88]
	ldr	r4, [r0, #92]
	ldr	r4, [r0, #96]
	ldr	r4, [r0, #100]
	ldr	r4, [r0, #104]
	ldr	r4, [r0, #108]
	ldr	r4, [r0, #112]
	ldr	r4, [r0, #116]
	ldr	r4, [r0, #120]
	ldr	r4, [r0, #124]

	sub	r1, r1, #1
 	bgt	myloop3a
	
	cmp	r0, r6
	bgt	myloop3b

# restore & return.
	ldmfd   sp!, {r4, r5, r6, r15}	

L2_writer:
# r0 = address
# r1 = count

# save r4 and return address.
	stmfd   sp!, {r4, r5, r6, r14}	

	add	r0, r0, #131072
	mov	r5, r1
	mov	r6, r0

myloop4b:
	sub	r0, r0, #128
	mov	r1, r5

myloop4a:
	str	r4, [r0]
	str	r4, [r0]
	str	r4, [r0, #4]
	str	r4, [r0, #8]
	str	r4, [r0, #12]
	str	r4, [r0, #16]
	str	r4, [r0, #20]
	str	r4, [r0, #24]
	str	r4, [r0, #28]
	str	r4, [r0, #32]
	str	r4, [r0, #36]
	str	r4, [r0, #40]
	str	r4, [r0, #44]
	str	r4, [r0, #48]
	str	r4, [r0, #52]
	str	r4, [r0, #56]
	str	r4, [r0, #60]
	str	r4, [r0, #64]
	str	r4, [r0, #68]
	str	r4, [r0, #72]
	str	r4, [r0, #76]
	str	r4, [r0, #80]
	str	r4, [r0, #84]
	str	r4, [r0, #88]
	str	r4, [r0, #92]
	str	r4, [r0, #96]
	str	r4, [r0, #100]
	str	r4, [r0, #104]
	str	r4, [r0, #108]
	str	r4, [r0, #112]
	str	r4, [r0, #116]
	str	r4, [r0, #120]
	str	r4, [r0, #124]

	sub	r1, r0, #1
 	bgt	myloop4a
	
	cmp	r0, r6
	bgt	myloop4b

# restore & return.
	ldmfd   sp!, {r4, r5, r6, r15}	

Main_reader:
# r0 = address
# r1 = count

# save r4 and return address.
	stmfd   sp!, {r4, r5, r6, r14}	

	add	r0, r0, #16777216
	mov	r5, r1
	mov	r6, r0

myloop5b:
	sub	r0, r0, #128
	mov	r1, r5

myloop5a:
	ldr	r4, [r0]
	ldr	r4, [r0]
	ldr	r4, [r0, #4]
	ldr	r4, [r0, #8]
	ldr	r4, [r0, #12]
	ldr	r4, [r0, #16]
	ldr	r4, [r0, #20]
	ldr	r4, [r0, #24]
	ldr	r4, [r0, #28]
	ldr	r4, [r0, #32]
	ldr	r4, [r0, #36]
	ldr	r4, [r0, #40]
	ldr	r4, [r0, #44]
	ldr	r4, [r0, #48]
	ldr	r4, [r0, #52]
	ldr	r4, [r0, #56]
	ldr	r4, [r0, #60]
	ldr	r4, [r0, #64]
	ldr	r4, [r0, #68]
	ldr	r4, [r0, #72]
	ldr	r4, [r0, #76]
	ldr	r4, [r0, #80]
	ldr	r4, [r0, #84]
	ldr	r4, [r0, #88]
	ldr	r4, [r0, #92]
	ldr	r4, [r0, #96]
	ldr	r4, [r0, #100]
	ldr	r4, [r0, #104]
	ldr	r4, [r0, #108]
	ldr	r4, [r0, #112]
	ldr	r4, [r0, #116]
	ldr	r4, [r0, #120]
	ldr	r4, [r0, #124]

	sub	r1, r1, #1
 	bgt	myloop5a
	
	cmp	r0, r6
	bgt	myloop5b

# restore & return.
	ldmfd   sp!, {r4, r5, r6, r15}	

Main_writer:
# r0 = address
# r1 = count

# save r4 and return address.
	stmfd   sp!, {r4, r5, r6, r14}	

	add	r0, r0, #16777216
	mov	r5, r1
	mov	r6, r0

myloop6b:
	sub	r0, r0, #128
	mov	r1, r5

myloop6a:
	str	r4, [r0]
	str	r4, [r0]
	str	r4, [r0, #4]
	str	r4, [r0, #8]
	str	r4, [r0, #12]
	str	r4, [r0, #16]
	str	r4, [r0, #20]
	str	r4, [r0, #24]
	str	r4, [r0, #28]
	str	r4, [r0, #32]
	str	r4, [r0, #36]
	str	r4, [r0, #40]
	str	r4, [r0, #44]
	str	r4, [r0, #48]
	str	r4, [r0, #52]
	str	r4, [r0, #56]
	str	r4, [r0, #60]
	str	r4, [r0, #64]
	str	r4, [r0, #68]
	str	r4, [r0, #72]
	str	r4, [r0, #76]
	str	r4, [r0, #80]
	str	r4, [r0, #84]
	str	r4, [r0, #88]
	str	r4, [r0, #92]
	str	r4, [r0, #96]
	str	r4, [r0, #100]
	str	r4, [r0, #104]
	str	r4, [r0, #108]
	str	r4, [r0, #112]
	str	r4, [r0, #116]
	str	r4, [r0, #120]
	str	r4, [r0, #124]

	sub	r1, r1, #1
 	bgt	myloop6a
	
	cmp	r0, r6
	bgt	myloop6b

# restore & return.
	ldmfd   sp!, {r4, r5, r6, r15}	

