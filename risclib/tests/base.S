# Base.S - test printing a number in different bases

#include <asm/regdef.h>

			.text
			.globl		main
			.ent		main
main:
			.frame		sp,12,ra
			.mask		0x80010000,-8
#			.set		noreorder
#			.cpload		t9
#			.set		reorder
#			.cprestore	8

			sub			sp,12
			sw			ra,(sp)
			sw			s0,4(sp)

			la			a1,prompt
			jal			WR_str

			li			a0,10
			jal			RD_num

			li			s0,2
loop:
			la			a1,inbase
			jal			WR_str

			move		a0,s0
			li			a2,10
			jal			WR_num

			la			a1,its
			jal			WR_str

			move		a0,v0
			move		a2,s0
			jal			WR_num

			jal			WR_endl

			add			s0,1
			blt			s0,17,loop

			lw			ra,(sp)
			lw			s0,4(sp)
			add			sp,12

			jr			ra

			.end		main

			.data

prompt:		.asciiz		"input number in base 10: "
inbase:		.asciiz		"in base "
its:		.asciiz		" it's "
