#ifndef CORRELATED_NOISE_H
#define CORRELATED_NOISE_H

#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include "Menu.h" // added [11/14/2017]
#include "MenuReturnValues.h" // added [11/14/2017]

//* Using rectangular/square grid-----------------------
//int GenCorrelatedNoiseRect(const int sd, int nPixels, int nRows, int nCols, int frameCt, int frameLag, int xLag, int yLag, int framePersist, double *memAssignFrames, double *memAssignInt);

//------------------------------------------------------

//* Using hexagonal grid--------------------------------
//int GenCorrelatedNoiseHex(const int sd, int Pixels, int nRowsHigh, int nRowsLow, int nColsHigh, int nColsLow,  int frameCt, int frameLag, int xLag, int yLag, int framePersist, float alpha1, float alpha2, int *Loc1, int*Loc2, unsigned short int *memAssignRandInt, double *memAssignRand, double *memAssignDACOut);
int GenCorrelatedNoiseHex_v2(const int sd, int Pixels, int nRowsHigh, int nRowsLow, int nColsHigh, int nColsLow,  int frameCt, int frameLag, int xLag, int yLag, int framePersist, float alpha1, float alpha2, float alpha3, int16 *Loc1, int16*Loc2, unsigned short int *memAssignRandInt1, unsigned short int *memAssignRandInt2, unsigned short int *memAssignAddedNoise, unsigned short int *memAssignMask, float pairProp, int PPPTrial); //changed int* => int16* for Loc1, Loc2 to match rest of code [11/10/2017], int PPTrial added for PPP [11/14/2017]


#endif    