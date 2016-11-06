#ifndef __DCJAM_GAMESTATES_ASPECTRATIOSELECT_H__
#define __DCJAM_GAMESTATES_ASPECTRATIOSELECT_H__

#include <sg_xpt.h>
#include <GameStateManager.h>

static const char *GAME_STATE_ASPECTRATIOSELECT = "Aspect ratio select";

Sint32 GS_ARS_RegisterWithGameStateManager(
	PGAMESTATE_MANAGER p_pGameStateManager );

#endif /* __DCJAM_GAMESTATES_ASPECTRATIOSELECT_H__ */

