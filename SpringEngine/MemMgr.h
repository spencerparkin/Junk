// MemMgr.h

#ifndef __MemMgr_h__
#define __MemMgr_h__

struct MemPool
{
	char *memory;			// A byte pointer to the pool of memory.
	int block_size;			// In units of bytes.
	int num_blocks;			// Number of total blocks in the pool.
	int *free_block_stack;	// Stack of free blocks.
	int stack_top;			// Pointer to top of free-block stack.
};

void RegisterMemPool(MemPool *mp, char *memory, int block_size, int num_blocks, int *free_block_stack);
void FreeMemPool(MemPool *mp);
char *AllocBlock(MemPool *mp);
int DeallocBlock(MemPool *mp, char *block);

#endif //__MemMgr_h__

// endof MemMgr.h