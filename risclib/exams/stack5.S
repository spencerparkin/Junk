#
# stack5.S
#
# Question:
# The program must sum numbers
# stored on the stack
# that have bit 6 set.
# The word
# on the top of the stack tells you how
# many numbers are in the sequence.
# Do not include this first word in the sum.
#
# Do not rely on the existence of the "test"
# variable, or the code above the dashed line.
#
# Output fromat must be:
# "sum is = 169"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			t0,test		# This code sets up the stack
			lw			t1,(t0)
			add			t0,4
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			add			t1,-1
			bnez		t1,loop
			lw			t1,test
			sub			sp,4
			sw			t1,(sp)
			
									# Stack set up now....

#----------- start cut -------------------

			# Spencer`s solution:

			xor			a0,a0		# zero total
			lw			t9,(sp)		# get pushed count
			add			sp,4		# deallocate push
			blez		t9,oops		# abort if stack empty
sum_loop:
			lw			t0,(sp)		# pop a number
			add			sp,4		# deallocate push
			and			t5,t0,0x40	# mask all but 6th bit
			beqz		t5,skip		# branch if unset
			add			a0,t0		# keep a running total
skip:
			add			t9,-1		# decrement pushed count
			bgtz		t9,sum_loop	# branch until stack empty
oops:
			la			a1,ans
			jal			WR_str

			li			a2,10		# print in decimal format
			jal			WR_num		# a0 already setup

			la			a1,endl
			jal			WR_str

#-----------  end cut  -------------------

			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		4, 0x96, 0x47, 0x28, 0x62

ans:		.asciiz		"sum is = "
endl:		.asciiz		"\n"
