#
# stack1.S
#
# Question:
#
# Count the number of negative words on
# the stack by popping the stack until a
# non-negative word is found, and print
# out the number of words popped.
#
# Do not rely on the existence of the "test"
# or "num" variables, or the code above the
# dashed line.
#
# Output format must be:
# "Number is = 5"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			t0,test		# This code sets us the stack
			lw			t1,num		# Do not alter
loop:		
			lw			t2,(t0)		# load array element
			sub			sp,4		# allocate a word on the stack
			sw			t2,(sp)		# push array element
			add			t0,4		# bump array index by element size
			add			t1,-1		# decrement count
			bnez		t1,loop		# while count isn`t zero yet

#------------- start cut ----------------------

			# Spencer`s solution:

			la			a1,ans
			jal			WR_str

			xor			a0,a0		# zero our count
pop_loop:
			lw			t0,(sp)		# pop a word off stack
			add			sp,4		# deallocate a word off the stack
			bgez		t0,break	# are we finished yet?
			add			a0,1		# increment count
			j			pop_loop	# continue processing
break:
			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

#-------------  end cut  ----------------------

			lw			ra,(sp)
			add			sp,4
			
			jr			ra

			.data

test:		.word		2,0xffffffd5,0xfffabfff,-3,-4,-9
num:		.word		6

ans:		.asciiz		"Number is = "
endl:		.asciiz		"\n"
