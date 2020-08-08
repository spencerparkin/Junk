#
# math2.S
#
# Question:
# calculate 5*X^2-3
#
# Output format must be:
# "answer = 242"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------ start cut ---------------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			lw			t0,(X)		# load X
			mul			t0,t0		# square X
			mul			t0,5		# multiply square by 5
			sub			a0,t0,3		# subtract 3 from product

			la			a1,ans
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#------------  end cut  ---------------------------

			.data

X:			.word		7

ans:		.asciiz		"answer = "
endl:		.asciiz		"\n"
