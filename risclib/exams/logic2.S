#
# logic2.S
#
# Question:
# "number" is a word.
# Write it out in base 2 as a sequence of 32 bits.
#
# Output format must be:
# "binary is = 00000000000000000000000000010001"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#----------- start cut ---------------------------

			# Spencer`s solution:

			sub			sp,16				# allocate 16 bytes on stack
			sw			ra,(sp)				# push return address
			sw			s0,4(sp)			# push used save registers
			sw			s5,8(sp)
			sw			s6,12(sp)

			la			a1,ans
			jal			WR_str				# begin to print answer

			lw			s0,(number)			# load the number
			li			s5,0x80000000		# load initial mask
loop:
			xor			a0,a0				# assume unset
			and			s6,s0,s5			# mask this bit
			beqz		s6,unset			# branch if not set
			add			a0,1				# bump 0 to 1
unset:
			jal			WR_num				# print bit
			srl			s5,1				# shift mask
			bnez		s5,loop				# until mask empty

			la			a1,endl
			jal			WR_str				# print newline

			lw			ra,(sp)				# pop return address
			lw			s0,4(sp)			# restore save registers
			lw			s5,8(sp)
			lw			s6,12(sp)
			add			sp,16				# deallocate 16 bytes off stack
			
			jr			ra

#-----------  end cut  ---------------------------

			.data

number:		.word		17
result:		.space		50,0

ans:		.asciiz		"binary is = "
endl:		.asciiz		"\n"
