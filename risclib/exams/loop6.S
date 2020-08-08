#
# loop6.S
#
# Question:
# "numbers" is an array of five words.
# Calculate the sum of all elements in "numbers"
# whose value is less than 1000.
#
# Output format must be:
# "sum = 11"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------- start cut ---------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			xor			a0,a0				# zero total
			move		t9,gp				# save global pointer
			la			gp,numbers			# address our array

			# We`ll always have 5 elements
			# so lets just un-roll the loop...
_1st_ele:
			lw			t0,0(gp)			# load 1st word
			bge			t0,1000,_2nd_ele
			add			a0,t0
_2nd_ele:
			lw			t0,4(gp)			# load 2nd word
			bge			t0,1000,_3rd_ele
			add			a0,t0
_3rd_ele:
			lw			t0,8(gp)			# load 3rd word
			bge			t0,1000,_4th_ele
			add			a0,t0
_4th_ele:
			lw			t0,12(gp)			# load 4th word
			bge			t0,1000,_5th_ele
			add			a0,t0
_5th_ele:
			lw			t0,16(gp)			# load 5th word
			bge			t0,1000,print_ans
			add			a0,t0
print_ans:
			move		gp,t9				# restore global pointer

			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#-------------  end cut  ---------------------

			.data

numbers:	.word		3,2000,2,6,2000

ans:		.asciiz		"sum = "
endl:		.asciiz		"\n"
