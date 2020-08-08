#
# logic1.S
#
# Question:
# "numbers" is an array of five words.
# Calculate the sum of all elements in "numbers"
# that are not multiples of 4.
# Use the and instruction, not div or rem for
# this question.
#
# Output format must be:
# "sum = 20"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#-------------- start cut -----------------------

			# Spencer`s solution:

			# the idea is:  if a number is
			# divisible by 2 and then half
			# that number is divible by 2
			# too, then it`s also divisble by 4.

			sub			sp,4
			sw			ra,(sp)

			xor			a0,a0			# zero total
			xor			t9,t9			# zero index
loop:		
			bge			t9,(4*5),exit	# for all elements in our array
			lw			t0,numbers(t9)	# load this element
			add			t9,4			# bump index by element size
			and			t7,t0,1			# is the number even?
			bnez		t7,sum			# if not, it`s not a multiple of 4
			sra			t0,1			# divide it by 2
			and			t7,t0,1			# is half the number even?
			beqz		t7,loop			# if so, it`s a multple of 4, continue
sum:
			add			a0,t0			# add to current sum
			j			loop			# continue processing
exit:
			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#--------------  end cut  -----------------------

			.data

numbers:	.word		3,4,12,28,17

ans:		.asciiz		"sum = "
endl:		.asciiz		"\n"
