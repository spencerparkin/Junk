#
# stack7.S
#
# Question:
#
# Pop the stack until a
# word is found which if printed out
# in hex would contain a "C", and print
# out that word in decimal.
#
# Do not rely on the existence of the "test"
# or "num" variables, or the code above the
# dashed line.
#
# Output format must be:
# "Number is = 192"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)
			move		s0,sp		# So we can re-balance the stack later

			la			t0,test		# This code sets up the stack
			lw			t1,num		# Do not alter
loop:
			lw			t2,(t0)
			sub			sp,4
			sw			t2,(sp)
			add			t0,4
			add			t1,-1
			bnez		t1,loop
									# Stack set up now....

#------------- start cut -------------------

			# Spencer`s solution:

outer_loop:
			lw			a0,(sp)				# pop off a word
			add			sp,4				# deallocate word
			move		t0,a0				# keep a0 safe while using t0
inner_loop:
			and			t5,t0,0xF			# isolate LSN (least sig. nibble)
			beq			t5,0xC,found		# branch if C found
			srl			t0,4				# shift nibbles right
			bnez		t0,inner_loop		# loop while nibbles to test
			bne			sp,fp,outer_loop	# loop while stack not empty
			j			oops				# not found, abort
found:
			la			a1,ans
			jal			WR_str

			li			a2,10				# print in decimal format
			jal			WR_num				# a0 is already setup

			la			a1,endl
			jal			WR_str
oops:

#-------------  end cut  -------------------

			move		sp,s0				# clean-up stack
			lw			ra,(sp)
			add			sp,4

			jr			ra

			.data

test:		.word		0x99, 0xc0, 0xf, 0xa, 1, 0xd
num:		.word		6

ans:		.asciiz		"Number is = "
endl:		.asciiz		"\n"
