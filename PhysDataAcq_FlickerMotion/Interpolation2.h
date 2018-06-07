#ifndef INTERPOLATION2_H
#define INTERPOLATION2_H

#include <iostream>
#include <fstream>
#include "Menu.h"
#include "MenuReturnValues.h"
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>

using namespace std;

unsigned int Sub2Ind(float IMAGE_HEIGHT, float IMAGE_WIDTH, unsigned short ROW, unsigned short COLUMN);

void CreateRandomFlicker_RT_int16(uInt32 frameCt, uInt32 Pixels, int16* tempLoc2, int16* tempLoc3, 
									uInt32* picBufSize, int16* frameLag, uInt32 ref_Zero, uInt32 numBlocks, uInt16 framePersist, 
									float* alpha0, float* alpha1, uInt32* ptr1, uInt32* ptr2, float *ptrZ, uInt32* ptrNoise, string merge, MenuReturnValues mValues);



uInt32 ConstructAOBuffer_RT_int16(  int16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, 
	uInt32 NGridSamples, uInt32 NZRpt, uInt32 TotNFrames, int16 NumAOChannels, int16* ptrXPixelPos,
	int16* ptrYPixelPos, float* ptrYawPosVec, float* ptrPitchPosVec, float* ptrRollPosVec, 
	float* ptrLED_XPos, float* ptrLED_YPos, float* WorldMapVec, float Height, float Width, 
	int16* Loc2, int16* tempLoc2, int16* Loc3, int16* tempLoc3, uInt32* picBufSize, uInt32* stimChange, 
	int16* deltaTChange, int16* xLagChange, int16* yLagChange, float* ptrCont1, float* ptrCont2, 
	uInt16 framePersist, int16* frameLag, uInt32* pTypeCt, uInt32 numBlocks, uInt32 ref_Zero, uInt32* memRandInt,
	uInt16 sd, float* alpha0, float* alpha1, int16 nSegs, string merge, MenuReturnValues mValues);


#endif