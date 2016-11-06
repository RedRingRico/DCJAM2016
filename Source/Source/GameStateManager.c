#include <GameStateManager.h>
#include <GameState.h>
#include <Log.h>
#include <string.h>

Sint32 GSM_Initialise( PGAMESTATE_MANAGER p_pGameStateManager,
	PGAMESTATE_MEMORY_BLOCKS p_pMemoryBlocks )
{
	p_pGameStateManager->pRegistry = NULL;
	p_pGameStateManager->pRegistry->pNext = NULL;

	p_pGameStateManager->pTopGameState = NULL;
	p_pGameStateManager->Running = false;

	memcpy( &p_pGameStateManager->MemoryBlocks, p_pMemoryBlocks,
		sizeof( GAMESTATE_MEMORY_BLOCKS ) );

	if( STK_Initialise( &p_pGameStateManager->GameStateStack,
		p_pGameStateManager->MemoryBlocks.pSystemMemoryBlock, 10,
		sizeof( GAMESTATE ), 8, "GSM: Game State Stack" ) != STK_OK )
	{
		LOG_Debug( "GSM_Initialise <ERROR> Failed to allocate memory for the "
			"game state stack" );

		return GSM_STACKINIT;
	}

	return GSM_OK;
}

void GSM_Terminate( PGAMESTATE_MANAGER p_pGameStateManager )
{
	PGAMESTATE_REGISTRY pRegistryItr = p_pGameStateManager->pRegistry;

	/* Pop all states */
	while( STK_GetCount( &p_pGameStateManager->GameStateStack ) )
	{
		GSM_PopGameState( p_pGameStateManager );
	}

	while( pRegistryItr != NULL )
	{
		PGAMESTATE_REGISTRY pNext = pRegistryItr->pNext;
		GAMESTATE_MEMORY_BLOCKS MemoryBlocks;

		MemoryBlocks = p_pGameStateManager->MemoryBlocks;

		MEM_FreeFromMemoryBlock( MemoryBlocks.pSystemMemoryBlock,
			pRegistryItr->pName );
		MEM_FreeFromMemoryBlock( MemoryBlocks.pSystemMemoryBlock,
			pRegistryItr->pGameState );
		MEM_FreeFromMemoryBlock( MemoryBlocks.pSystemMemoryBlock,
			pRegistryItr );

		pRegistryItr = pNext;
	}

	STK_Terminate( &p_pGameStateManager->GameStateStack );
}

Sint32 GSM_ChangeGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, void *p_pGameStateLoadArguments,
	void *p_pGameStateInitialiseArguments )
{
	PGAMESTATE pGameState;
	PGAMESTATE_REGISTRY pRegistryItr;

	/* Pop all states */
	while( STK_GetCount( &p_pGameStateManager->GameStateStack ) )
	{
		GSM_PopGameState( p_pGameStateManager );
	}

	if( GSM_IsGameStateInRegistry( p_pGameStateManager, p_pGameStateName,
		&pGameState ) == false )
	{
		LOG_Debug( "GSM_ChangGameeState <ERROR> Could not locate game state \""
			"%s\"\n", p_pGameStateName );

		return GSM_STATENOTFOUND;
	}

	if( GSM_PushGameState( p_pGameStateManager, p_pGameStateName,
		p_pGameStateLoadArguments, p_pGameStateInitialiseArguments ) !=
			STK_OK )
	{
		LOG_Debug( "GSM_ChangeGameState <ERROR> Something went wrong pushing "
			"the state onto the stack\n" );

		return GSM_PUSHSTATE;
	}

	p_pGameStateManager->Running = true;
	p_pGameStateManager->pTopGameState->Update( p_pGameStateManager );

	return GSM_OK;
}

Sint32 GSM_PushGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, void *p_pGameStateLoadArguments,
	void *p_pGameStateInitialiseArguments )
{
	PGAMESTATE pGameState;

	if( GSM_IsGameStateInRegistry( p_pGameStateManager, p_pGameStateName,
		&pGameState ) == false )
	{
		LOG_Debug( "GSM_PushGameeState <ERROR> Could not locate game state \""
			"%s\"\n", p_pGameStateName );

		return GSM_STATENOTFOUND;
	}

	if( p_pGameStateManager->pTopGameState != NULL )
	{
		GS_Pause( p_pGameStateManager->pTopGameState );
	}

	if( STK_Push( &p_pGameStateManager->GameStateStack, pGameState ) !=
		STK_OK )
	{
		LOG_Debug( "GSM_PushGameState <ERROR> Something went wrong pushing "
			"the game state \"%s\" onto the stack\n", p_pGameStateName );

		return GSM_PUSHSTATE;
	}

	p_pGameStateManager->pTopGameState = STK_GetTopItem(
		&p_pGameStateManager->GameStateStack );

	/* Good time to garbage collect */
	MEM_GarbageCollectMemoryBlock(
		p_pGameStateManager->MemoryBlocks.pSystemMemoryBlock );
	MEM_GarbageCollectMemoryBlock(
		p_pGameStateManager->MemoryBlocks.pGraphicsMemoryBlock );
	MEM_GarbageCollectMemoryBlock(
		p_pGameStateManager->MemoryBlocks.pAudioMemoryBlock );

	p_pGameStateManager->pTopGameState->Load( p_pGameStateLoadArguments );
	p_pGameStateManager->pTopGameState->Initialise(
		p_pGameStateInitialiseArguments );

	return GSM_OK;
}

Sint32 GSM_PopGameState( PGAMESTATE_MANAGER p_pGameStateManager )
{
	if( STK_Pop( &p_pGameStateManager->GameStateStack, NULL ) != 0 )
	{
		LOG_Debug( "GSM_PopGameState <ERROR> Failed to pop game state from "
			"stack\n" );

		return GSM_POPSTATE;
	}

	/* Terminate game state */
	p_pGameStateManager->pTopGameState->Terminate( NULL );
	p_pGameStateManager->pTopGameState->Unload( NULL );

	/* Reacquire top state */
	p_pGameStateManager->pTopGameState = STK_GetTopItem(
		&p_pGameStateManager->GameStateStack );

	if( p_pGameStateManager->pTopGameState != NULL )
	{
		GS_Resume( p_pGameStateManager->pTopGameState );
	}

	return GSM_OK;
}

Sint32 GSM_Run( PGAMESTATE_MANAGER p_pGameStateManager )
{
	Uint32 StartTime = syTmrGetCount( );

	p_pGameStateManager->pTopGameState->Update( p_pGameStateManager );
	p_pGameStateManager->pTopGameState->Render( NULL );

	p_pGameStateManager->pTopGameState->ElapsedGameTime +=
		syTmrCountToMicro( syTmrDiffCount( StartTime, syTmrGetCount( ) ) );

	return GSM_OK;
}

Sint32 GSM_Quit( PGAMESTATE_MANAGER p_pGameStateManager )
{
	p_pGameStateManager->Running = false;

	return GSM_OK;
}

bool GSM_IsRunning( PGAMESTATE_MANAGER p_pGameStateManager )
{
	return p_pGameStateManager->Running;
}

Sint32 GSM_RegisterGameState( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, PGAMESTATE p_pGameState )
{
	PGAMESTATE_REGISTRY pRegistryItr = p_pGameStateManager->pRegistry;
	PGAMESTATE_REGISTRY pNewEntry = NULL, pAppendTo = NULL;
#if defined ( DCJAM_BUILD_DEBUG )
	char Name[ 64 ] = "GS Name: ";
	char Registry[ 64 ] = "GS Registry: ";
	char GameState[ 64 ] = "GS: ";
	strcat( Name, p_pGameStateName );
	strcat( Registry, p_pGameStateName );
	strcat( GameState, p_pGameStateName );
#else
	char *Name = p_pGameStateName;
	char *Registry = p_pGameStateName;
	char *GameState = p_pGameStateName;
#endif /* DCJAM_BUILD_DEBUG */

	/* First item in registry */
	if( p_pGameStateManager->pRegistry == NULL )
	{
		p_pGameStateManager->pRegistry =
			MEM_AllocateFromMemoryBlock(
				p_pGameStateManager->MemoryBlocks.pSystemMemoryBlock,
				sizeof( GAMESTATE_REGISTRY ), Registry );

		pNewEntry = p_pGameStateManager->pRegistry;
	}
	else
	{
		while( pRegistryItr != NULL )
		{
			if( strcmp( p_pGameStateName, pRegistryItr->pName ) == 0 )
			{
				LOG_Debug( "GSM_RegisterGameState <WARN> Attempting to re-"
					"register game state \"%s\"\n", p_pGameStateName );

				return GSM_STATEALREADYREGISTERED;
			}

			pAppendTo = pRegistryItr;
			pRegistryItr = pRegistryItr->pNext;
		}
	}

	pNewEntry->pName = MEM_AllocateFromMemoryBlock(
		p_pGameStateManager->MemoryBlocks.pSystemMemoryBlock,
		strlen( p_pGameStateName ) + 1, Name );
	strncpy( pNewEntry->pName, p_pGameStateName,
		strlen( p_pGameStateName ) );
	pNewEntry->pName[ strlen( p_pGameStateName ) ] = '\0';

	pNewEntry->pGameState = MEM_AllocateFromMemoryBlock(
		p_pGameStateManager->MemoryBlocks.pSystemMemoryBlock,
		sizeof( GAMESTATE ), GameState );

	GS_Copy( pNewEntry->pGameState, p_pGameState );

	pNewEntry->pNext = NULL;

	if( pAppendTo != NULL )
	{
		pAppendTo->pNext = pNewEntry;
	}

	return GSM_OK;
}

bool GSM_IsGameStateInRegistry( PGAMESTATE_MANAGER p_pGameStateManager,
	const char *p_pGameStateName, struct _tagGAMESTATE **p_ppGameState )
{
	PGAMESTATE_REGISTRY pRegistryItr = p_pGameStateManager->pRegistry;
	bool GameStatePresent = false;
	size_t Itr = 0;

	while( pRegistryItr != NULL )
	{
		if( strcmp( p_pGameStateName, pRegistryItr->pName ) == 0 )
		{
			GameStatePresent = true;
			break;
		}
		pRegistryItr = pRegistryItr->pNext;
		++Itr;
	}

	if( ( pRegistryItr != NULL ) && ( p_ppGameState != NULL ) )
	{
		( *p_ppGameState ) = pRegistryItr->pGameState;
	}

	return GameStatePresent;
}

