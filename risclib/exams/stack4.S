#
# stack4.S
#
# Question:
# the program must find the smallest number in
# a sssequence of ten numbers stored on the stack.
#
# Do not rely on the existence of the "test"
# variable, or the code above the
# dashed line.
#
# Output format must be:
# "min is = -8"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			t0,test		# This code sets up the stack
			li			t1,10
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			sub			t1,1
			bnez		t1,loop

									# Stack set up now....

#--------------- start cut -----------------------

			# Spencer`s solution:

			li			a0,0x7FFFFFFF	# initial min equals max signed value
			li			t1,10			# load pushed count
pop_loop:
			lw			t0,(sp)			# pop a number off
			add			sp,4			# deallocate push
			bge			t0,a0,skip		# branch if >= minimum
			move		a0,t0			# update minimum
skip:
			sub			t1,1			# decrement pushed count
			bgtz		t1,pop_loop		# loop until stack empty

			la			a1,ans
			jal			WR_str

			li			a2,10			# print in decimal format
			jal			WR_num			# a0 is already be setup

			la			a1,endl
			jal			WR_str

#---------------  end cut  -----------------------

			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		4, -6, 7, 8, 0, 6, 6, -8, 6, 6

ans:		.asciiz		"min is = "
endl:		.asciiz		"\n"
