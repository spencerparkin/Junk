#
# recur1.S
#
# Question:
#
# Write a function named search that will do a
# depth first search of a tree for a marked
# node.  A marked node is one that has a value
# field equal to 1.  Only one node in the tree is
# marked.
#
# The paramters to search are a pointer to the
# tree and the current depth.  On each recursive
# call add 1 to the depth.  This parameter is
# used to keep track of the path from the root
# to the marked node; as you visit a node, you
# will call a procedure named store_path to
# record the fact that you have visited this
# node.  The code for store_path and print_path
# have been written for you -- all you need to
# do is understand how to set up their paramters
# and make the call.
# 
# The code for search could look like:
#			call store_path
#			iff (value == 1)
#			  return 1
#			iff (left tree exists)
#			  iff (search(left tree, depth + 1))
#			    return 1
#			iff (right tree exists)
#			  return search(right tree, dept+1)
#			return 0
# 
# Output format must be:
# "apple-->orange-->plum-->grape-->star-->passion"
#

#include <asm/regdef.h>

			.text
			.globl		main
main:
			sub			sp,4
			sw			ra,(sp)

			la			a0,tree		# load address of root node
			li			a1,0		# begin at recurse level zero
			jal			search		# search the tree (depth first)

			jal			print_path	# print the path
									# to the node with val=1

			la			a1,endl		# load newline
			jal			WR_str		# print carriage return

			li			v0,10		# load return value

			lw			ra,(sp)
			add			sp,4

			jr			ra			# au revoir....whatever
			
#------------------------------------------
# store_path - store pointer at level n in the path
#		a0 - holds pointer to string
#		a1 - level to use in path
#------------------------------------------
store_path:
			mul			t0,a1,4		# each pointer is 4 bytes
			sw			a0,path(t0)	# save pointer to the name
			add			t0,4		# make the next extry
			sw			$0,path(t0)	#  equal to 0.
			jr			ra

#------------------------------------------
# print_path() - print the items stored in path
#------------------------------------------
print_path:
			sub			sp,16
			sw			ra,(sp)
			sw			a1,4(sp)
			sw			s0,8(sp)
			sw			s1,12(sp)

			la			s0,path				# load array of pointers
			lw			a1,(s0)				# get 1st pointer
			beqz		a1,path_printed		# branch if null
			jal			WR_str				# print string in path
print_loop:
			add			s0,4				# increment by pointer size
			lw			s1,(s0)				# get another pointer
			beqz		s1,path_printed		# branch if null
			la			a1,arrow			# more to come, draw the arrow
			jal			WR_str				# print arrow
			move		a1,s1				# setup string argument
			jal			WR_str				# print string in path
			j			print_loop			# continue processing
path_printed:
			lw			ra,(sp)
			lw			a1,4(sp)
			lw			s0,8(sp)
			lw			s1,12(sp)
			add			sp,16

			jr			ra

#-------------------- start cut ---------------------

			# Spencer`s solution:

			# Here is the pseudo code for 
			# the proposed solution.  It`s
			# basicaly the same as the
			# pseudo code in the question.
			# All we have to do is write it
			# in assembly language.
			#
			# search(node, depth) {
			#   iff (node isn`t null) {
			#     call store_path(node, depth)
			#     iff (node->flag == 1)
			#       return 1
			#     iff (search(node->left, depth+1) == 1)
			#       return 1
			#     iff (search(node->right, depth+1) == 1)
			#       return 1
			#     return 0
			#   }
			#   return ?
			# }

# The fallowing equates define offsets from a
# node pointer to its contents.  These give
# the node memory both structure and meaning.
# These also make the code much easier to read and write.
			.equ		NAME,0
			.equ		LBCH,4
			.equ		RBCH,8
			.equ		FLAG,12
# How does the .struct directive work?  Not very well.

#==================================
# search function:	find the first marked node
#  input -> a0 -> tree node (initial call passes root node)
#  input -> a1 -> depth count (initial call passes 0)
#  output -> path is setup indirectly through store_path function
#  output -> v0 -> return 0 if not found, 1 if found
#  note -> if given node is null on initial call, v0 is undefined
search:
			beqz		a0,ret_func		# abort if null node passed in

			sub			sp,12
			sw			ra,(sp)
			sw			s0,4(sp)
			sw			s1,8(sp)

			move		s0,a0			# use s0 for node pointer
			add			s1,a1,1			# use s1 for depth count

			# Note the strategy used here.  The appropriate
			# save registers (sx) are saved on the stack.  Then
			# we move our function arguments (ax) into those save
			# registers.  We can now freely use the save registers
			# (sx) without werying about them getting changed by
			# a function and we can use the argument registers (ax)
			# to setup other function calls.

			lw			a0,NAME(s0)		# specify string argument
			jal			store_path		# depth is already in a1

			lw			v0,FLAG(s0)		# load flag
			bnez		v0,exit_func	# did we find it?

			lw			a0,LBCH(s0)		# load left branch pointer
			move		a1,s1			# load recurse depth
			jal			search			# recurse left
			bnez		v0,exit_func	# did we find it?

			lw			a0,RBCH(s0)		# load right branch pointer
			move		a1,s1			# load recurse depth
			jal			search			# recurse right
										# did we find it?  we hope so!
exit_func:
			lw			ra,(sp)
			lw			s0,4(sp)
			lw			s1,8(sp)
			add			sp,12
ret_func:
			jr			ra

#--------------------  end cut  ---------------------

			.data

# The binary tree.  Note that each node has four
# words - a pointer to the name, pointers to
# left and right subtrees, and the integer
# value field.

path:		.space		40

tree:		.word		name0, node1, node2, 0
node1:		.word		name1, node3, node4, 0
node2:		.word		name2, node5, node6, 0
node3:		.word		name3, node7, 0, 0
node4:		.word		name4, node8, node9, 0
node5:		.word		name5, 0, 0, 0				# leaf node
node6:		.word		name6, node10, node11, 0
node7:		.word		name7, 0, 0, 0
node8:		.word		name8, 0, 0, 0
node9:		.word		name9, node12, node13, 0
node10:		.word		name10, 0, 0, 0
node11:		.word		name11, 0, 0, 0
node12:		.word		name12, node14, node15, 0
node13:		.word		name13, 0, 0, 0
node14:		.word		name14, 0, 0, 1				# target node (also a leaf)
node15:		.word		name15, node16, node17, 0
node16:		.word		name16, 0, 0, 0
node17:		.word		name17, 0, 0, 0

name0:		.asciiz		"apple"
name1:		.asciiz		"orange"
name2:		.asciiz		"banana"
name3:		.asciiz		"pear"
name4:		.asciiz		"plum"
name5:		.asciiz		"peach"
name6:		.asciiz		"nectarine"
name7:		.asciiz		"pineapple"
name8:		.asciiz		"grapefruit"
name9:		.asciiz		"grape"
name10:		.asciiz		"melon"
name11:		.asciiz		"avocado"
name12:		.asciiz		"star"
name13:		.asciiz		"mango"
name14:		.asciiz		"passion"
name15:		.asciiz		"canteloupe"
name16:		.asciiz		"watermelon"
name17:		.asciiz		"apricot"

arrow:		.asciiz		"-->"
endl:		.asciiz		"\n"
