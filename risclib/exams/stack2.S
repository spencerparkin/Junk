#
# stack2.S
#
# Question:
# The program must sum a sequence of numbers
# stored on the stack.  The word
# on the top of the stack tells you how many
# numbers are in the sequence.
#
# Do not rely on the existence of the "test"
# variable, or the code above the
# dashed line.
#
# Output format must be:
# "sum is = 23"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			t0,test		# This code sets us the stack
			lw			t1,(t0)		# Do not alter
			add			t0,4
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			add			t1,-1
			bnez		t1,loop
			la			t0,test
			lw			t1,(t0)
			sub			sp,4
			sw			t1,(sp)

									# Stack set up now....

#-------------- start cut -------------------

			# Spencer`s solution:

			xor			a0,a0		# zero sum
			lw			t9,(sp)		# pop the pop count
			add			sp,4		# deallocate push (now officialy popped)
			blez		t9,skip		# skip loop if stack empty
pop_loop:			
			lw			t0,(sp)		# pop a number
			add			sp,4		# deallocate push
			add			a0,t0		# keep a running total
			sub			t9,1		# decrement pop count
			bgtz		t9,pop_loop	# while stack not empty
skip:
			la			a1,ans
			jal			WR_str

									# a0 should already be setup
			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

#--------------  end cut  -------------------

			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		4, 6, 7, 8, 2
num:		.word		5

ans:		.asciiz		"sum is = "
endl:		.asciiz		"\n"
