// buffer.h

// Programmed by Spencer T. Parkin

#ifndef __buffer_h__
#define __buffer_h__

void WipeBuffer(void **buf, int buf_size);
bool InsertIntoBuffer(void **buf, int buf_size, void *data);
bool RemoveFromBuffer(void **buf, int buf_size, void *data);
int ScanBuffer(void **buf, int buf_size, int (* scan_func)(void *data));

#endif __buffer_h__

// endof buffer.h