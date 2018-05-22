//%SUB2IND Linear index from multiple subscripts.
//%   SUB2IND is used to determine the equivalent single index
//%   corresponding to a given set of subscript values.
//%
//%   IND = SUB2IND(SIZ,I,J) returns the linear index equivalent to the
//%   row and column subscripts in the arrays I and J for an matrix of
//%   size SIZ. 
//%
//%   IND = SUB2IND(SIZ,I1,I2,...,IN) returns the linear index
//%   equivalent to the N subscripts in the arrays I1,I2,...,IN for an
//%   array of size SIZ.
//%
//%   I1,I2,...,IN must have the same size, and IND will have the same size
//%   as I1,I2,...,IN. For an array A, if IND = SUB2IND(SIZE(A),I1,...,IN)),
//%   then A(IND(k))=A(I1(k),...,IN(k)) for all k.
//%
//%==============================================================================
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <math.h>
#include <ctime>
#include <time.h>
#include <string>
#include <windows.h>
#include <vector>
#include <stdlib.h>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include "Interpolation2.h"
#define PI 3.14159265

using namespace std;

unsigned int Sub2Ind(float IMAGE_HEIGHT, float IMAGE_WIDTH, unsigned short Row, unsigned short Column)
{
	unsigned int ndx;

	// Need to put in error checking to make sure ROW and COLUMN are not out of bounds
	if (Row > IMAGE_HEIGHT || Column > IMAGE_WIDTH) // if: the number of rows in the grid is greater than the image's height OR the number of columns in the grid is greater than the image's width, then... (?)
		cout << "Sub2Ind: Out of range subscript." << endl; // then: return error message for function.

	//Compute linear indices
	ndx = (unsigned int) ((Column-1)*IMAGE_HEIGHT + Row); // assign uint ndx the value: ((number of grid columns - 1)*(image's height) + (number of grid rows))

	return ndx;
};



void CreateRandomFlicker_RT_int16(uInt32 frameCt, uInt32 Pixels, int16* tempLoc2, int16* tempLoc3, 
									uInt32* picBufSize, int16* frameLag, uInt32 ref_Zero, uInt32 numBlocks, uInt16 framePersist, 
									float* alpha0, float* alpha1, uInt32* ptr1, uInt32* ptr2, float *ptrZ, string merge)


{
	uInt32 idxT		= 0;
	float rndFlt1	= 0;
	float rndFlt2	= 0;
	float cont0		= *alpha0; 
	float cont1		= *alpha1; 

	//----------------------Populate picture buffer ----------------------------
	// NOTE: struct for memcpy is: void* memcpy( void* dest, const void* src, std::size_t count ); (similar for memmove)
	idxT = *frameLag + ref_Zero;	
	
	// Initialize buffer with library of pictures A
	if (frameCt==0){
		for (uInt32 j=0; j<numBlocks; j++){
			if (j%framePersist == 0){
				for (uInt32 i=0; i<Pixels; i++){
					picBufSize[i+j*Pixels] = rand();
				}
			}
			else{
				memcpy(picBufSize+Pixels*j, picBufSize+Pixels*(j-1), sizeof(uInt32)*Pixels); 
			}
		}
		memcpy(ptr1, picBufSize, sizeof(uInt32)*Pixels); 
		memcpy(ptr2, picBufSize, sizeof(uInt32)*Pixels);  
	}
	else{ 
	// Generate pictures A and B for current frame					
		memmove(picBufSize, picBufSize+Pixels, sizeof(uInt32)*Pixels*(numBlocks-1)); 
		memcpy(ptr1, picBufSize, sizeof(uInt32)*Pixels);			// I_0(x,t) 
		
		if (frameCt%framePersist==0){
         	for (uInt32 i=0; i<Pixels; i++){
				picBufSize[(numBlocks-1)*Pixels + i] = rand();
			}
		}
		else{
			memcpy(picBufSize+(numBlocks-1)*Pixels, picBufSize+(numBlocks-2)*Pixels, sizeof(uInt32)*Pixels); 
		}
		
		for (uInt32 i=0; i<Pixels; i++){
			if (tempLoc2[i] < 0){
				ptr2[i] = rand();									// I_0(x-dx,t-dt) 
			}
			else{	
				if (merge=="No"){
					ptr2[i] = picBufSize[idxT*Pixels + tempLoc2[i]];
				}
				else{
					ptr2[i] = (uInt32)((float)(picBufSize[idxT*Pixels + tempLoc2[i]] + 
						picBufSize[idxT*Pixels + tempLoc3[i]])/2.0);
				}
			}
		}
		
			
		// Check for the sign of "m1": -ve => reversal of motion direction
		if (cont1<0){
			for (uInt32 i=0; i<Pixels; i++){
				ptr2[i] = RAND_MAX-ptr2[i]; 
			}
		}
		
		// Create copy of "image2" to be repeated during PERSISTENCE time
		// memcpy(memRandInt,ptr2,sizeof(uInt32)*Pixels);		
	} 
		cont1 = fabs(cont1); 

		// Generate frame from picture A and picture B
		for (uInt32 i=0; i<Pixels; i++)
		{
			rndFlt1 = (float)ptr1[i]/(float)RAND_MAX ;			// [0,1]
			//rndFlt1 = rndFlt1*2.0 -1.0;						// [-1,1] : Uniform
			rndFlt1 = floor(rndFlt1*2.0)*2.0 -1.0;				// [-1,1] : Binary

			rndFlt2 = (float)ptr2[i]/(float)RAND_MAX ;			// [0,1]			
			//rndFlt2 = rndFlt2*2.0 -1.0;						// [-1,1] : Uniform
			rndFlt2 = floor(rndFlt2*2.0)*2.0 -1.0;				// [-1,1] : Binary

			ptrZ[i] = (float)((cont0*rndFlt1 + cont1*rndFlt2)/(float)2.0);	// in [-(c0+c1)/2, +(c0+c1)/2] 	
		}
	
	//	system("pause");
	
	return;
};





uInt32 ConstructAOBuffer_RT_int16( int16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, 
	uInt32 NGridSamples, uInt32 NZRpt, uInt32 TotNFrames, int16 NumAOChannels, int16* ptrXPixelPos,
	int16* ptrYPixelPos, float* ptrYawPosVec, float* ptrPitchPosVec, float* ptrRollPosVec, 
	float* ptrLED_XPos, float* ptrLED_YPos, float* WorldMapVec, float Height, float Width, 
	int16* Loc2, int16* tempLoc2, int16* Loc3, int16* tempLoc3, uInt32* picBufSize, uInt32* stimChange, 
	int16* deltaTChange, int16* xLagChange, int16* yLagChange, float* ptrCont1, float* ptrCont2, 
	uInt16 framePersist, int16* frameLag, uInt32* pTypeCt, uInt32 numBlocks, uInt32 ref_Zero, uInt32* memRandInt,
	uInt16 sd, float* alpha0, float* alpha1, int16 nSegs, string merge) 
{
		//Initialize the arrays for the INPUT buffer
		uInt32	nFrame	= (uInt32) nbSamplePerChannel/NumAOChannels; // the number of frames is set to the value (number of samples per channel / number of analog output channels)
		uInt32	idx1 = 0, idx2	= 0, idx3 = 0, idx4 = 0; // initialize our four counters (idx1 = sample counter, idx2 = per-frame raster number, idx3 = frame number, idx4 = number of Z signal repeats)
		int	counter = 0;								 // Counter for # of AO samples produced
		uInt32	temp	= -1; 	
		
		float WorldMap_XCoord = 0;	// X-coordinates for interpolated LED intensities
		float WorldMap_YCoord = 0;	// Y-coordinates for interpolated LED intensities
		float AlphaR		= 0;
		float AlphaC		= 0;
		float XPos			= 0;
		float YPos			= 0;	
		float Frames		= 0;	// this will hold the random pixel intensites (flicker values), derived from the Z signal array and stored in the pBuffer
		float Image			= 0;	// this will be the image to be displayed post-interpolation
		float ContPat		= 0.3;  // Set contrast of pattern image
		int16 tempvar		=  0; 
		unsigned short kR0	= 0; // linear weight for interpolation
		unsigned short kR1	= 0; // linear weight for interpolation
		unsigned short kC0	= 0; // linear weight for interpolation
		unsigned short kC1	= 0; // linear weight for interpolation
		unsigned int ind1	= 0, ind2 = 0, ind3 = 0, ind4 = 0; // indices on array to interpolate from
		bool cond = false; 
		


		// Flicker stimulus parameters
		uInt32* ptr1	= new uInt32 [(NGridSamples-6)];	// initializes a pointer ptr1 to an empty array of uInt32 values; range: [0, (NGridSamples-6)]
		uInt32* ptr2	= new uInt32 [(NGridSamples-6)];	// initializes a pointer ptr2 to an empty array of uInt32 values; range: [0, (NGridSamples-6)]
		float *ptrZ = new float [(NGridSamples-6)];			// initializes a pointer ptrZ to an empty array of float values; range: [0, (NGridSamples-6)]
		uInt32 ct;											// Counter for ptrZ				
		float n1 = 0;				 // used for keeping frame count when stimulus changes (adapt-test stim)
		uInt32 n2; 
		
		
		// Set seed for random number generator (occurs before the very first frame of a trial is called)
		if (TotNFrames==0){	
			srand(sd); // pseudo-random number generator with seed int 'sd'; pseudo-random implies that returned random list will be repeated
		}
		
		//Read the data from the INPUT file into the INPUT buffer
		for ( uInt32 idx1 = 0; idx1 < nbSamplePerChannel; idx1++ ) { // for loop: increment the sample count from 0 to (number of samples per channel) [0 -> 416500, counts through 1 time]
			idx2 = idx1 % NGridSamples ;							 // the raster number will equal (sample count) mod (number of grid samples) [0 -> 832, counts through 500 times]
			idx3 = (uInt32) floor( (float) idx1/NGridSamples ) ;	 // the frame number will equal the rounded down (i.e. floor) of the ((sample count) divided by (number of grid samples)) [0 -> 500, repeats each value 833 times]
			


			if (idx3 != temp) {		// if: the frame number is not the same as 'temp'	
				temp = idx3;		// Counter for frame change; set temp equal to the frame number
				
				/*
				// Case 1: Interleave adapting stimulus (2D pattern) and test stimulus (flicker motion)[2s, 1s, 2s, 1s]  
				n1 =  3000 * (uInt32)((TotNFrames+idx3+idx2)/3000.0);
				n2 = (uInt32)n1;
				if ((((uInt32)(TotNFrames+idx3+idx2)>=n2) & ((uInt32)(TotNFrames+idx3+idx2)<n2+1000)) | 
					(((uInt32)(TotNFrames+idx3+idx2)>=n2+1500) & ((uInt32)(TotNFrames+idx3+idx2)<n2+2500)) ){
					cond = true; // Adapting motion
				}
				else{
					cond = false; // Test motion
				} 	
				
				// Case 2: Interleave adapting stimulus (2D pattern) and test stimulus (flicker motion) [adapt: 2700 - 4750 frame #]  
				n1 =  3710*(uInt32)((TotNFrames+idx3+idx2)/3710.0);
				n2 = (uInt32)n1;
				if ((((uInt32)(TotNFrames+idx3+idx2)>=n2+1000) & ((uInt32)(TotNFrames+idx3+idx2)<n2+1855)) | 
					(((uInt32)(TotNFrames+idx3+idx2)>=n2+2855) & ((uInt32)(TotNFrames+idx3+idx2)<n2+3710)) ){
					cond = true; // Adapting motion
				}
				else{
					cond = false; // Test motion
				}
				
				// Case 3: Interleave step stimulus (pattern) and apparent motion stimulus [3.8 0.2 1.5 3.8 0.2 1.5]s
				n1 =  5500*(uInt32)((TotNFrames+idx3+idx2)/5500.0);
				n2 = (uInt32)n1;
				if ((((uInt32)(TotNFrames+idx3+idx2)>=n2) & ((uInt32)(TotNFrames+idx3+idx2)<n2+1900)) | 
					(((uInt32)(TotNFrames+idx3+idx2)>=n2+2750) & ((uInt32)(TotNFrames+idx3+idx2)<n2+4650)) ){
					cond = true; // Adapting motion
				}
				else{
					cond = false; // Test motion
				}

				// Case 4: Interleave step stimulus (pattern) and apparent motion stimulus [3.8 0.2 2 3.8 0.2 2]s
				n1 =  6000*(uInt32)((TotNFrames+idx3+idx2)/6000.0);
				n2 = (uInt32)n1;
				if ((((uInt32)(TotNFrames+idx3+idx2)>=n2) & ((uInt32)(TotNFrames+idx3+idx2)<n2+1900)) | 
					(((uInt32)(TotNFrames+idx3+idx2)>=n2+3000) & ((uInt32)(TotNFrames+idx3+idx2)<n2+4900)) ){
					cond = true; // Adapting motion
				}
				else{
					cond = false; // Test motion
				}
				
				// Case 5: Interleave step stimulus (pattern) and apparent motion stimulus [3.8 0.5 2 3.8 0.5 2]s
				n1 =  6300*(uInt32)((TotNFrames+idx3+idx2)/6300.0);
				n2 = (uInt32)n1;
				if ((((uInt32)(TotNFrames+idx3+idx2)>=n2) & ((uInt32)(TotNFrames+idx3+idx2)<n2+1900)) | 
					(((uInt32)(TotNFrames+idx3+idx2)>=n2+3150) & ((uInt32)(TotNFrames+idx3+idx2)<n2+5050)) ){
					cond = true; // Adapting motion
				}
				else{
					cond = false; // Test motion
				}*/
				
				
				// Frame # when stimulus changes 
				// IMP :: [Loc2, Loc3] => correspond to different (odd) or same (even) pixel locations of the same delayed image
				// NOTE: struct for memcpy is: void* memcpy( void* dest, const void* src, std::size_t count );
				tempvar = (int16)(*pTypeCt%nSegs); // set value for tempvar equal to (*pTypeCt mod (number of segments in trial))
				if ((TotNFrames+idx3+idx2) == stimChange[*pTypeCt])	{ // if: ((total # of frames) + (current frame number) + (current raster point)) = (value in stimChange matrix pointed at by pointer *pTypeCt)...
					memcpy(tempLoc2, Loc2+(tempvar)*(NGridSamples-6), sizeof(int16)*(NGridSamples-6));	// then: ...copy (sizeof(int16)*(# Grid Samples - 6)) bytes of memory from (Loc2+(tempvar*(# Grid Samples - 6))) to tempLoc2			
					memcpy(tempLoc3, Loc3+(tempvar)*(NGridSamples-6), sizeof(int16)*(NGridSamples-6));	// then: ...copy (sizeof(int16)*(# Grid Samples - 6)) bytes of memory from (Loc3+(tempvar*(# Grid Samples - 6))) to tempLoc3			
					*frameLag = deltaTChange[tempvar]; // point frameLag at the index [tempvar] of array deltaTChange
					*alpha0	= ptrCont1[tempvar];	   // point alpha0 at the index [tempvar] of array ptrCont1
					*alpha1 = ptrCont2[tempvar];       // point alpha1 at the index [tempvar] of array ptrCont2
					(*pTypeCt)++;					   // increment the pTypeCt array by 1
				}
				// Correction for any out of range indices 
				for (int i=0; i<NGridSamples-6; i++){								 // checks each raster point value
					tempLoc2[i] = (tempLoc2[i]>NGridSamples-6-1) ? -1 : tempLoc2[i]; // sets tempLoc2[i] to -1 if () is true, does not change tempLoc2[i] if false
					tempLoc3[i] = (tempLoc3[i]>NGridSamples-6-1) ? -1 : tempLoc3[i]; // sets tempLoc3[i] to -1 if () is true, does not change tempLoc3[i] if false
				}

				
				// Call the flicker generating function 
				ct = 0;							// Counter for pixel 
				CreateRandomFlicker_RT_int16(TotNFrames+idx3+idx2, NGridSamples-6, &tempLoc2[0], &tempLoc3[0], 
									&picBufSize[0], frameLag, ref_Zero, numBlocks, framePersist, 
									alpha0, alpha1, &ptr1[0], &ptr2[0], &ptrZ[0], merge);


				// This one extra point is defined by an (X,Y,Z) triplet. This ensures the AOBuffer has 7500 values, or 2500 triplets.
				// Number of Z-Values = ( 833 * NZSignalRepeat + 1 )
				// NOTE: counter++ is a post-increment operator; i.e. FOLLOWING each line it is evaluated, counter = counter + 1
				pBuffer[counter++] = ptrXPixelPos[0]; // sets the initial value for XPixelPos to pBuffer, increments counter by 1
				pBuffer[counter++] = ptrYPixelPos[0]; // sets the initial value for YPixelPos to pBuffer, increments counter by 1
				pBuffer[counter++] = 0;				  // sets the value 0 to pBuffer, increments counter by 1
				// altogether, the above three lines of code will put [...,XPixelPos[0],YPixelPos[0],0,...] into the pBuffer array
			}

 			for ( uInt32 idx4 = 0; idx4 < NZRpt; ++idx4 ) { // idx4 counts the number of repeats for the Z signal
				pBuffer[counter++] = ptrXPixelPos[idx2];	// X Signal; sets pBuffer[counter] to value at index [idx2] of XPixelPos array, increments counter by 1
				pBuffer[counter++] = ptrYPixelPos[idx2];	// Y signal; sets pBuffer[counter] to value at index [idx2] of YPixelPos array; increments counter by 1

				// Z signal
				if ( idx4 == 0 || idx2 == 0 || idx2 > (NGridSamples - 6) ) { // if: there have been no repeats OR this is the first raster displayed of a frame OR if the raster number is greater than the number of grid samples...
					pBuffer[counter++] = 0; // ...then: set pBuffer[counter++] to 0, increment counter by 1
				}

				else if ( idx4 == 1 ) // else if: this is the first repeat of the Z signal...
				{
					
					//-----------------FIND COORDINATES OF WORLD MAP CLOSEST TO LED POSITIONS------------------------
					//XPos =  ( ptrLED_XPos[idx2] * cos(2 * PI * ptrRollPosVec[TotNFrames+idx3]/360) ) + ( ptrLED_YPos[idx2] * sin(2 * PI * ptrRollPosVec[TotNFrames+idx3]/360) );
					//YPos = -( ptrLED_XPos[idx2] * sin(2 * PI * ptrRollPosVec[TotNFrames+idx3]/360) ) + ( ptrLED_YPos[idx2] * cos(2 * PI * ptrRollPosVec[TotNFrames+idx3]/360) );
					//WorldMap_XCoord = fmod ( ( XPos + ptrYawPosVec[TotNFrames+idx3] - 1 ) , Width ) + 1;
					//WorldMap_YCoord = fmod ( ( YPos + ptrPitchPosVec[TotNFrames+idx3] - 1 ) , Height ) + 1;

					XPos = ptrLED_XPos[idx2]; // set XPos = the current raster number's value stored in the array LED_XPos[]
					YPos = ptrLED_YPos[idx2]; // set YPos = the current raster number's value stored in the array LED_YPos[]
					// THEN: set the WorldMap_XCoord to ( (XPos + (the index [Total # of frames + current frame #] in the array YawPosVec)) - (XPos + ((the index [Total # of frames + current frame #] in the array YawPosVec)/Width)) * Width
					WorldMap_XCoord = (XPos+ptrYawPosVec[TotNFrames+idx3]) - (int16)((XPos+ptrYawPosVec[TotNFrames+idx3])/Width)*Width; 
					// THEN: set the WorldMap_XCoord to ( (XPos + (the index [Total # of frames + current frame #] in the array PitchPosVec)) - (XPos + ((the index [Total # of frames + current frame #] in the array PitchPosVec)/Height)) * Height
					WorldMap_YCoord = (YPos+ptrPitchPosVec[TotNFrames+idx3]) - (int16)((YPos+ptrPitchPosVec[TotNFrames+idx3])/Height)*Height;
					

					//These lines of code are required to convert negative values back to a legal positive values (e.g. mod(-190,3600)=3410)
					if (WorldMap_XCoord < 0 )	   // if: WorldMap_XCoord is less than 0...
						WorldMap_XCoord += Width;  // then: ...increment WorldMap_XCoord by the Width
					if (WorldMap_YCoord < 0 )	   // if: WorldMap_YCoord is less than 0...
						WorldMap_YCoord += Height; // then: ...increment WorldMap_YCoord by the Height

					// Set values for Row and Column displacements to be used during interpolation
					AlphaC = WorldMap_XCoord - (int16)WorldMap_XCoord; // first instance of WorldMap is float, second is int; returns decimal value
					AlphaR = WorldMap_YCoord - (int16)WorldMap_YCoord; // first instance of WorldMap is float, second is int; returns decimal value
					
					
					kR0 = (unsigned short) ((WorldMap_YCoord - (int16) (WorldMap_YCoord/Height) * Height) + 1); // kR0 is given the value (WorldMap_YCoord - (WorldMap_YCoord/Height)*Height + 1)
					kR1 = (unsigned short)(kR0 % (int16)Height + 1);                                            // kR1 is given the value: kR0 mod Height + 1
					kC0 = (unsigned short) ((WorldMap_XCoord - (int16) (WorldMap_XCoord/Width) * Width) + 1);   // kC0 is given the value (WorldMap_XCoord - (WorldMap_XCoord/Width)*Width + 1)
					kC1 = (unsigned short) (kC0 % (int16)Width + 1);                                            // kC1 is given the value: kC0 mod Width + 1
					

					// CREATE THE VECTOR OF WEIGHTED IINTENSITIES THAT DEFINES A LINEARLY INTERPOLATED VALUE.
					ind1 = Sub2Ind( Height, Width, kR0, kC0 ); // sets value of index for kR0/kC0 displacements
					ind2 = Sub2Ind( Height, Width, kR1, kC0 ); // sets value of index for kR1/kC0 displacements
					ind3 = Sub2Ind( Height, Width, kR0, kC1 ); // sets value of index for kR0/kC1 displacements
					ind4 = Sub2Ind( Height, Width, kR1, kC1 ); // sets value of index for kR1/kC1 displacements
					//cout << kR0 << "\t" << kR1 << "\t" << kC0 << "\t" << kC1 << endl;
					//cout << ind1 << "\t" << ind2 << "\t" << ind3 << "\t" << ind4 << endl;
					if (ind1 > 12960000 || ind2 > 12960000 || ind3 > 12960000 || ind4 > 12960000 ) // returns error message for interpolation process if indices are extremely out of bounds
						cout << "Problem\n" << endl;
					
					// this is the final step in the interpolation process, where our pixel index is compared to four surrounding indices in the array.
					Image = ( (1-AlphaR) * (1-AlphaC) * WorldMapVec[ ind1 ] + AlphaR * (1-AlphaC) * WorldMapVec[ ind2 ] + 
						(1-AlphaR) * AlphaC * WorldMapVec[ ind3 ] + AlphaR * AlphaC * WorldMapVec[ ind4 ] ) ; 
						// i.e. (1-AlphaR)(1-AlphaC)(WorldMapVec[ind1]) + (AlphaR)(1-AlphaC)(WorldMapVec[ind2])
						//    + (1-AlphaR)(AlphaC)(WorldMapVec[ind3])   + (AlphaR)(AlphaC)(WorldMapVec[ind4]) = Image 

					Image = ((Image*2.0/0.7 - 1.0) * ContPat) + 1.0; 	// Sets contrast to 2D pattern: [1-ContPat,1+ContPat] 
					    // NOTE: ContPat is 'Contrast of Pattern Image'
					


					//__________________ADD WEIGHTED FLICKER INTENSITIES TO THE WORLD MAP INTENSITIES__________________
						
					//-----------------Case 1 : Pattern + Flicker Motion--------------------
					//Frames = (cond)*(Image*0.7/2.0) + (!cond)*(Image + Image*ptrZ[ct])*0.7/2.0;		// Flicker + 2D Pattern (superposed)
					//Frames = (cond)*(Image*0.7/2.0) + (!cond)*((ptrZ[ct] + 1.0)*0.7/2.0);				// Flicker 
					//Frames = Frames * pow(2.0, 15.0) / 5.0;
					
					//-----------------Case 2 : Pure Flicker Miotion--------------------------

					Frames = ((ptrZ[ct] + 1.0)/2.0) * 0.7 * pow(2.0, 15.0) / 5.0;						// Flicker
					// i.e. Frames = (((value at index [ct] of Z signal array) + 1)/2.0)*(0.7)*((2.0^15.0)/5.0) 

					//Frames = ((ptrZ[ct]*0.7 + 0.35)) * pow(2.0, 15.0) / 5.0;							// Flicker (2 times the contrast)

					//-----------------Case 4 : Pattern--------------------------------------
					//Frames = Image * (0.35) * pow(2.0, 15.0) / 5.0;									// 2D Pattern

					//-----------------Case 5 : Flicker Motion + Added Noise-----------------			// [5/22/2018]
					//-----------------Case 6 : Flicker Motion + Added Noise + PPP-----------			// [5/22/2018]
					//_________________________________________________________________________________________________



					// Assign random values to pixel intensities (counter increments only if "Frames=0" 
					pBuffer[counter++] = (int16) (Frames);
				}
				
				else {										// if: this is not the first Z signal repeat... 
					pBuffer[counter++] = (int16) (Frames);  // then: ...set value Frames (defined in previous if statement) to pBuffer array
					ct += 1;		                        // then: ...and increment ct by 1			
				}
			}
		} // loop for idx1 iteration ends. 
		
		//cout << "Count  = " << ct << endl;   

	// empty all pointers as well as the frame counter idx3
	delete [] ptrZ;
	delete [] ptr1;
	delete [] ptr2;
	return idx3;
};






