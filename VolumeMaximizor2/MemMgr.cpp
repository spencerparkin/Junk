// MemMgr.cpp

#include "MemMgr.h"
#include <string.h>
#include <assert.h>

//#define MEM_MGR_DEBUG

void RegisterMemPool(MemPool *mp, char *memory, int block_size, int num_blocks, int *free_block_stack)
{
	mp->memory = memory;
	mp->block_size = block_size;
	mp->num_blocks = num_blocks;
	mp->free_block_stack = free_block_stack;
	FreeMemPool(mp);
}

void FreeMemPool(MemPool *mp)
{
	int k;
	for(k = 0; k < mp->num_blocks; k++)
		mp->free_block_stack[k] = k;
	mp->stack_top = 0;
}

char *AllocBlock(MemPool *mp)
{
	if(mp->stack_top >= mp->num_blocks)
	{
//		assert(0);	// stack underflow
		return 0;
	}
	int k = mp->free_block_stack[mp->stack_top++];
	char *block = mp->memory + k * mp->block_size;
	memset((void *)block, 0, mp->block_size);
	return block;
}

// note that deallocating a block twice is
// a very bad thing to have happen.
int DeallocBlock(MemPool *mp, char *block)
{
	if(mp->stack_top <= 0)
	{
		assert(0);	// stack overflow
		return 0;
	}
	int k = (block - mp->memory) / mp->block_size;
#ifdef MEM_MGR_DEBUG
	int i;
	for(i = mp->stack_top; i < mp->num_blocks; i++)
		assert(mp->free_block_stack[i] != k);
#endif // MEM_MGR_DEBUG
	mp->free_block_stack[--mp->stack_top] = k;
	return 1;
}

// endof MemMgr.cpp