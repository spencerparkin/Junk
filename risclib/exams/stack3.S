#
# Stack3.S
#
# Question:
# the program must sum a sequence of numbers
# stored on the stack.  "num"
# tells you how many numbers are in the sequence.
#
# Do not rely on the existence of the "test"
# variable, or the code above the
# dashed line.
#
# Output format must be:
# "sum is 15"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			t0,test		# This code sets up the stack
			lw			t1,num
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			sub			t1,1
			bnez		t1,loop

									# Stack set up now....

#----------- start cut ----------------------

			# Spencer`s solution:

			xor			a0,a0		# zero total
			lw			t9,(num)	# get pushed count
			blez		t9,oops		# abort if num <= 0
pop_loop:
			lw			t0,(sp)		# pop a number
			add			sp,4		# deallocate push (actually pop)
			add			a0,t0		# keep a running total
			sub			t9,1		# decrement pushed count
			bgtz		t9,pop_loop	# while stack not empty
oops:
			la			a1,ans
			jal			WR_str

			li			a2,10		# display in decimal format
			jal			WR_num		# a0 already setup

			la			a1,endl
			jal			WR_str

#-----------  end cut  ----------------------

			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		4, -6, 7, 8, 2
num:		.word		5

ans:		.asciiz		"sum is = "
endl:		.asciiz		"\n"
