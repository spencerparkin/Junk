// data.S	Copyright (c) 2002	By Spencer T. Parkin

// This file contains data used internally
// by various library functions.  The data
// is externed in the file, data.h.

#include <data.h>

			.data

			.align		2
scratch:	.space		PADLEN,0

			.align		2
digit:		.ascii		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
