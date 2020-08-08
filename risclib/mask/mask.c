// mask.c

// Author: Spencer T. Parkin
// Copyright (c) 2003

// This is a simple program that helps
// you compose a mask to use with the
// ".mask" psuedo-op.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int regbit(char *regname)
{
	if(*regname == '$')
	{
		return(atoi(regname + 1));
	}

	switch(*regname)
	{
		case '$':
		{
			return(atoi(regname + 1));
		}
		case 'v':
		{
			return(atoi(regname + 1) + 2);
		}
		case 'a':
		{
			return(atoi(regname + 1) + 4);
		}
		case 't':
		{
			int t = atoi(regname + 1);

			if(t < 7)
			{
				return(t + 8);
			}

			return(t + 16);
		}
		case 's':
		{
			return(atoi(regname + 1) + 16);
		}
		case 'k':
		{
			return(atoi(regname + 1) + 26);
		}
		default:
		{
			if(!strcmp(regname, "zero"))
			{
				return(0);
			}
			else if(!strcmp(regname, "at"))
			{
				return(1);
			}
			else if(!strcmp(regname, "gp"))
			{
				return(28);
			}
			else if(!strcmp(regname, "sp"))
			{
				return(29);
			}
			else if(!strcmp(regname, "fp"))
			{
				return(30);
			}
			else if(!strcmp(regname, "ra"))
			{
				return(31);
			}
		}
	}

	return(-1);
}

main(int argc, char **argv)
{
	int mask = 0, bit;

	if(argc < 2)
	{
		fprintf(stderr,
				"\n"
				"Usage: mask [ reg1, reg2, reg3, ... ]\n"
				"Example: mask sp fp ra s0 '$15'\n"
				"\n"
				"Programmed by Spencer T. Parkin\n\n");

		return(-1);
	}

	while(argc-- > 1)
	{
		bit = regbit(argv[argc]);

		if(bit < 0)
		{
			fprintf(stderr, "bogus register: %s\n", argv[argc]);
			return(-1);
		}

		mask |= 1 << bit;
	}

	printf("\n.mask\t0x%08x\n\n", mask);

	return(0);
}
