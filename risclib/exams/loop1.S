#
# loop1.S
#
# Question:
# calculate the sum of the elements in "array"
# "count" holds the number of elements in "array"
#
# Output format must be:
# "sum = 15"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:

#------------- start cut -------------------

			# Spencer`s solution:

			sub			sp,4
			sw			ra,(sp)

			xor			a0,a0			# zero total
			lw			t9,(count)		# load array element count
			blez		t9,print_ans	# just in case...
loop:
			sub			t9,1			# decrement index
			mul			t5,t9,4			# scale index by element size
			lw			t0,array(t5)	# load an array element
			add			a0,t0			# keep a running total
			bgtz		t9,loop			# while we have elements left to do
print_ans:
			la			a1,ans1
			jal			WR_str

			li			a2,10
			jal			WR_num

			la			a1,endl
			jal			WR_str

			lw			ra,(sp)
			add			sp,4

			jr			ra

#-------------  end cut  -------------------

			.data

array:		.word		3,4,2,6
count:		.word		4

ans1:		.asciiz		"sum = "
endl:		.asciiz		"\n"
