# case.S - test str_cpy, str2upper & str2lower functions

#include <asm/regdef.h>

			.data

string:		.asciiz		"a vERy WaCkY StRInG\n"

upper:		.space		100,0
lower:		.space		100,0

			.text
			.globl		main
main:		
			sub			sp,4
			sw			ra,(sp)

			la			a0,string	# load address of source string

			la			a1,upper	# address upper-case destination string
			jal			str_cpy		# copy work string over
			jal			str2upper	# convert to all upper-case
			jal			WR_str		# print the string

			la			a1,lower	# address lower-case destination string
			jal			str_cpy		# copy work string over
			jal			str2lower	# convert to all lower-case
			jal			WR_str		# print the string

			lw			ra,(sp)
			add			sp,4

			jr			ra
