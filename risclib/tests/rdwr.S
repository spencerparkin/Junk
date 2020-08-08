# rdwr.S - test read/write functions

#include <asm/regdef.h>

			.text
			.globl		main
			.ent		main
main:
			.frame		sp,8,ra
			.mask		0x800100000,-4
			.set		noreorder
			.cpload		t9
			.set		reorder

			sub			sp,8
			sw			ra,(sp)
			sw			s0,4(sp)

			li			a0,10		# read numbers in decimal format
			li			a2,10		# write numbers in decimal format

			la			a1,lmult	# load address of string to print
			jal			WR_str		# print left operand prompt to stdout

			jal			RD_num		# read decimal number from stdin
			move		s0,v0		# save result in s0

			la			a1,rmult	# load address of string to print
			jal			WR_str		# print right operand prompt to stdout

			jal			RD_num		# read decimal number from stdin
			mul			a0,s0,v0	# compute product of each number read

			la			a1,prod		# load address of string to print
			jal			WR_str		# print product string to stdout

			jal			WR_num		# print product (a0) to stdout
			jal			WR_endl		# print carrage return

			lw			ra,(sp)
			lw			s0,4(sp)
			add			sp,8

			jr			ra

			.end		main

			.data

lmult:		.asciiz		"input left multiplicand: "
rmult:		.asciiz		"input right multiplicand: "
prod:		.asciiz		"output product = "
