// buffer.cpp

// Programmed by Spencer T. Parkin

#include "buffer.h"

void WipeBuffer(void **buf, int buf_size)
{
	int k;
	for(k = 0; k < buf_size; k++)
		buf[k] = 0;
}

bool InsertIntoBuffer(void **buf, int buf_size, void *data)
{
	int k;
	for(k = 0; k < buf_size; k++)
	{
		if(!buf[k])
		{
			buf[k] = data;
			return true;
		}
	}

	return false;
}

bool RemoveFromBuffer(void **buf, int buf_size, void *data)
{
	int k;
	for(k = 0; k < buf_size; k++)
	{
		if(buf[k] == data)
		{
			buf[k] = 0;
			return true;
		}
	}

	return false;
}

int ScanBuffer(void **buf, int buf_size, int (* scan_func)(void *data))
{
	int k;
	int ret = 0;
	for(k = 0; k < buf_size && !ret; k++)
		if(buf[k])
			ret = scan_func(buf[k]);
	return ret;
}

// endof buffer.cpp