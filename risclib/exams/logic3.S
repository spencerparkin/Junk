#
# logic3.S
#
# Question:
# "number" is a word.
# If bits 3 and 6 are set then
# set bits 0, 1, and 2 and print out
# the result.
#
# If bits 3 and 6 are not set then
# print out the number unchanged.
#
# Output format must be:
# "result is 79"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#-------------- start cut --------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			lw			a0,(number)		# load our test subject
			and			t1,a0,0x08		# mask all but 3rd bit
			beqz		t1,skip			# branch if unset
			and			t2,a0,0x40		# mask all but 6th bit
			beqz		t2,skip			# branch if unset
			or			a0,0x07			# both bits set, or-in bits 0, 1, and 2
skip:
			la			a1,ans			# load address of result string
			jal			WR_str			# print "result is = "

			li			a2,10			# print in base 10 format
										# a0 already holds our answer
			jal			WR_num			# print the answer

			la			a1,endl			# load newline string
			jal			WR_str			# perform carrage return

			lw			ra,(sp)
			add			sp,4

			jr			ra

#--------------  end cut  --------------------

			.data

number:		.word		0x48

ans:		.asciiz		"result is = "
endl:		.asciiz		"\n"
