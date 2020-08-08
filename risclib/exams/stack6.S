#
# stack6.S
#
# Question:
#
# Count the number of words on
# the stack with at most four bits set
# by popping the stack until a
# word is found with five or more bits set,
# and print
# out the number of words popped.
#
# Do not rely on the existence of the "test"
# or "num" variables, or the code above the
# dashed lines.
#
# Output format must be:
# "Number is = 4"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)
			move		s0,sp		# so we can re-balance stack

			la			t0,test		# This code sets us the stack
			lw			t1,num		# Do not alter
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			add			t1,-1
			bnez		t1,loop

#------------ start cut ---------------------

			# Spencer`s solution:

			li			a0,-1			# zero minus one count
outer_loop:
			add			a0,1			# count this element no matter what
			lw			t0,(sp)			# pop a word
			add			sp,4			# dealloc a word
			xor			t9,t9			# zero set-bit count
inner_loop:
			and			t4,t0,1			# mask LSB
			beqz		t4,skip			# branch if LSB set
			add			t9,1			# increment set-bit count
skip:		srl			t0,1			# shift MSB toward LSB
			bnez		t0,inner_loop	# until no set-bits left

			ble			t9,4,outer_loop	# until element w/more than 4 bits set

			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

#------------  end cut  ---------------------

			move		sp,s0			# clean-up stack
			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		2,0xFF,4,1,1,1
num:		.word		6

ans:		.asciiz		"Number is = "
endl:		.asciiz		"\n"
