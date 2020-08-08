// risclib.h	Copyright (c) 2002-2003	By Spencer T. Parkin

			# write functions:

			.extern		WR_str		# write string to stdout
			.extern		WR_num		# write number to stdout
			.extern		WR_fnum		# write float to stdout
			.extern		WR_endl		# write newline char

			# read functions:

			.extern		RD_str		# read string from stdin
			.extern		RD_num		# read number from stdin
			.extern		RD_fnum		# read float from stdin

			# convertion routines:

			.extern		str2num		# convert string to number
			.extern		num2str		# convert number to string
			.extern		str2fnum	# convert string to float
			.extern		fnum2str	# convert float to string
			.extern		str2upper	# convert all lower-case to upper
			.extern		str2lower	# convert all upper-case to lower

			# common string operations:

			.extern		str_len		# compute string length
			.extern		str_cat		# concatinate strings
			.extern		str_cmp		# compare strings
			.extern		str_cpy		# copy strings
			.extern		del_char	# delete old char from string
			.extern		new_char	# make room for new char in string
