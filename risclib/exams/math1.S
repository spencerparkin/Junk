#
# math1.S
#
# Question:
# calculate A*X^2+B*X+C
#
# Output format must be:
# "answer = 180"
#

#include <asm/regdef.h>

			.globl		main
main:

#------------- start cut ------------------------

			# Spencer`s solution:

			sub			sp,4		# allocate a word on the stack
			sw			ra,(sp)		# push return address

			lw			t0,(X)		# load X
			lw			t1,(A)		# load A
			lw			t2,(B)		# load B
			lw			t3,(C)		# load C

			mul			t2,t0		# B=B*X
			mul			t0,t0		# X=X^2
			mul			t1,t0		# A=A*X

			add			t1,t2		# A=A+B
			add			a0,t1,t3	# a0=A+C

			la			a1,ans		# load address of ans string
			jal			WR_str		# print "answer = "

			li			a2,10		# format answer in base 10
			jal			WR_num		# print the answer

			la			a1,endl		# load carriage return string
			jal			WR_str		# print newline

			lw			ra,(sp)		# pop return address
			add			sp,4		# deallocate a word off the stack

#-------------  end cut  ------------------------			

			jr			ra

			.data

X:			.word		7
A:			.word		3
B:			.word		4
C:			.word		5

ans:		.asciiz		"answer = "
endl:		.asciiz		"\n"

