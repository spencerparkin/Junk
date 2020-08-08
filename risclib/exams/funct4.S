#
# funct4.S
#
# Question:
# Write a function "findpat" that takes a
# long binary patter in $a0 and a smaller
# binary pattern in $a1, and returns true
# iff the long binary patter contains the
# smaller one.
# For example 100111 base two contains 1001.
#
# Output format must be
# "Pattern found"
#

#include <asm/regdef.h>

#ifndef _MODULE	// This is done so that we can treat this file like a module
			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			li			a0,39
			li			a1,9
			jal			findpat			# call function

			la			a1,yes			# be optimistic
			bnez		v0,print_result
			la			a1,no			# should have been pesimistic
print_result:
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#endif _MODULE

#------------- start cut --------------

			# Spencer`s solution:

			# We can use the function
			# written in funct3.S to help
			# solve this problem.  So
			# lets declare the "setbits"
			# symbol as "extern".

			.extern		setbits

			# Note that it would be more convenient and
			# efficient to swap the roles played by a0
			# and a1, but i wont do that here in strict
			# obediance to the question.
#=========================================
# findpath function: search for a small pattern in a big one
#  input -> a0 -> big pattern
#  input -> a1 -> small pattern
#  output -> v0 -> 1 if small pattern exists inside the big pattern, else 0
			.globl		findpat
findpat:
			sub			sp,12
			sw			ra,(sp)
			sw			a0,4(sp)
			sw			a1,8(sp)

			# iff the small pattern is zero and
			# the big pattern is not -1,
			# return success.
			beqz		a1,findit
			bne			a0,-1,findit
			j			found
findit:
			# swap registers a0 and a1 for setbits function call
			xor			a0,a1
			xor			a1,a0
			xor			a0,a1

			jal			setbits			# get the scope of our search pattern
pat_loop:
			and			t0,a1,v0		# mask bits outside of pattern scope
#			xor			t0,a0			# diff`em (we could use sub here too)
#			beqz		t0,found		# if no dif, we found it!
			beq 		t0,a0,found		# this works too
			and			t0,v0,0x80000000	# is MSB set? (could also use a0)
			bnez		t0,not_found		# if yes, we`ve done all we can
			sll			a0,1				# shift search pattern
			sll			v0,1				# shift scope to match pattern
			j			pat_loop
not_found:
			move		v0,$0			# return failure
			j			exit_findpat
found:
			li			v0,1			# return kupla!  (success)
exit_findpat:
			
			lw			ra,(sp)
			lw			a0,4(sp)
			lw			a1,8(sp)
			add			sp,12

			jr			ra

#-------------  end cut  --------------

#ifndef _MODULE

			.data

yes:		.asciiz		"Pattern found\n"
no:			.asciiz		"Pattern not found\n"

#endif _MODULE
