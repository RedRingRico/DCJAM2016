#ifndef __DCJAM_STACK_H__
#define __DCJAM_STACK_H__

#include <Memory.h>

#define STK_OK 0
#define STK_ERROR 1
#define STK_UNDERFLOW 10
#define STK_FATALERROR -1
#define STK_MEMORYERROR -10
#define STK_OVERFLOW -11

typedef struct _tagSTACK
{
	/* Top and apacity are specifided in terms of the item size, i.e. if an
	 * item is 8 bytes, a two-item capable stack has a capacity of 16 and a max
	 * top of 16 (pStack-8, pStack-16)
	 */
	size_t			Top;
	size_t			Capacity;
	size_t			ItemSize;
	size_t			GrowableAmount;
	void			*pStack;
	PMEMORY_BLOCK	pMemoryBlock;
}STACK, *PSTACK;

Sint32 STK_Initialise( PSTACK p_pStack, PMEMORY_BLOCK p_pMemoryBlock,
	size_t p_Capacity, size_t p_ItemSize, size_t p_GrowableAmount,
	const char *p_pName );
void STK_Terminate( PSTACK p_pStack );

Sint32 STK_Push( PSTACK p_pStack, void *p_pItem );
Sint32 STK_Pop( PSTACK p_pStack, void *p_pItem );

void *STK_GetTopItem( PSTACK p_pStack );
size_t STK_GetCount( PSTACK p_pStack );
bool STK_IsFull( PSTACK p_pStack );
bool STK_IsEmpty( PSTACK p_pStack );

#endif /* __DCJAMC_STACK_H__ */

