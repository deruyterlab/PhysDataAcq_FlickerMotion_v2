#ifndef ARRAY_INDEX_RELOCATE_H
#define ARRAY_INDEX_RELOCATE_H

#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>

//int  IndexRelocate(int xLag, int yLag, int nrh, int nrl, int nch, int ncl, int Pixels, int oddXstepY, int oddYstepX, int *Loc1, int *Loc2);
void IndexRelocate(int16 nTurns, int16 *xLagChange, int16 *yLagChange, uInt16 nrh, uInt16 nrl, uInt16 nch, uInt16 ncl,
					  uInt16 Pixels, int16* oddXstepY, int16* oddYstepX, int16* Loc2);

//switched to old int definition 10/30/2017 [AWT]; then returned to void version

#endif
