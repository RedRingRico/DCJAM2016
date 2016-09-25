#ifndef __DCJAM_PERIPHERAL_H__
#define __DCJAM_PERIPHERAL_H__

#include <shinobi.h>

#define PER_OK			0
#define PER_ERROR		1
#define PER_FATALERROR	-1

extern PDS_PERIPHERAL g_Peripherals[ 4 ];

Sint32 PER_Initialise( void );
void PER_Terminate( );

#endif /* __DCJAM_PERIPHERAL_H__ */

