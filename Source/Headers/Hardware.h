#ifndef __DCJAM_HARDWARE_H__
#define __DCJAM_HARDWARE_H__

#include <Memory.h>
#include <kamui2.h>
#include <shinobi.h>

#define HW_ERROR 1
#define HW_FATALERROR -1
#define HW_OK 0

#define KM_DITHER TRUE
#define KM_AAMODE FALSE

Sint32 HW_Initialise( KMBPPMODE p_BPP, SYE_CBL *p_pCableType,
	PNATIVE_MEMORY_FREESTAT p_pMemoryFreeStat );
void HW_Terminate( void );

void HW_Reboot( void );

#endif /* __DCJAM_HARDWARE_H__ */

