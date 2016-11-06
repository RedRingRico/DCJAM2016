#ifndef __DCJAM_GAMESTATEMANAGER_H__
#define __DCJAM_GAMESTATEMANAGER_H__

#include <Memory.h>
#include <Stack.h>

#define GSM_OK 0
#define GSM_ERROR 1
#define GSM_STATEALREADYREGISTERED 1000
#define GSM_FATALERROR -1
#define GSM_STACKINIT -1000
#define GSM_POPSTATE -2000
#define GSM_PUSHSTATE -3000
#define GSM_STATENOTFOUND -4000

typedef struct _tagGAMESTATE_MEMORY_BLOCKS
{
	PMEMORY_BLOCK	pSystemMemoryBlock;
	PMEMORY_BLOCK	pGraphicsMemoryBlock;
	PMEMORY_BLOCK	pAudioMemoryBlock;
}GAMESTATE_MEMORY_BLOCKS, *PGAMESTATE_MEMORY_BLOCKS;

/* A simple linked list of game states to push/pop */
typedef struct _tagGAMESTATE_REGISTRY
{
	char							*pName;
	struct _tagGAMESTATE			*pGameState;
	struct _tagGAMESTATE_REGISTRY	*pNext;
}GAMESTATE_REGISTRY, *PGAMESTATE_REGISTRY;

typedef struct _tagGAMESTATE_MANAGER
{
	GAMESTATE_MEMORY_BLOCKS	MemoryBlocks;
	PGAMESTATE_REGISTRY		pRegistry;
	struct _tagGAMESTATE	*pTopGameState;
	STACK					GameStateStack;
	bool					Running;
}GAMESTATE_MANAGER, *PGAMESTATE_MANAGER;

Sint32 GSM_Initialise( PGAMESTATE_MANAGER p_pGameStateManager,
	PGAMESTATE_MEMORY_BLOCKS p_pMemoryBlocks );
void GSM_Terminate( PGAMESTATE_MANAGER p_pGameStateManager );

Sint32 GSM_ChangeGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, void *p_pGameStateLoadArguments,
	void *p_pGameStateInitialiseArguments );
Sint32 GSM_PushGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_GamepStateName, void *p_pGameStateLoadArguments,
	void *p_pGameStateInitialiseArguments );
Sint32 GSM_PopGameState( PGAMESTATE_MANAGER p_pGameStateManager );

Sint32 GSM_Run( PGAMESTATE_MANAGER p_pGameStateManager );
Sint32 GSM_Quit( PGAMESTATE_MANAGER p_pGameStateManager );
bool GSM_IsRunning( PGAMESTATE_MANAGER p_pGameStateManager );
 
Sint32 GSM_RegisterGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, struct _tagGAMESTATE *p_pGameState );
bool GSM_IsGameStateInRegistry( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, struct _tagGAMESTATE **p_ppGameState );

#endif /* __DCJAM_GAMESTATEMANAGER_H__ */

