#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <math.h>
#include <random>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include <ctime>
#include <time.h>
#include <conio.h>
#include "DynamicRandGen.h"
#include "ArrayIndexRelocate.h"
#include "FileCtrl.h"
//#include "RandVec.h"
#include "CorrelatedNoise.h"
#include "MenuReturnValues.h"
#include "Menu.h"

extern int16* oddXstepY;
extern int16* oddYstepX;
extern int16 nTurns; //[11/10/2017]
extern int16* xLagChange; //[11/10/2017]
extern int16* yLagChange; //[11/10/2017]

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else // added [11/14/2017]

using namespace std;

// Initialize Menu Return Variables
void DynamicRandGen(MenuReturnValues mValues, int idx) // added [11/14/2017]
{
	{
		int PPPTrial = mValues.PPPTrial; // added [11/14/2017]
		return;

		//extern int PPPTrial; // commented out, multiple initalization error [4/18/2018]
		const int sd = 0;


		/*typedef mersenne_twister_engine<class uint_fast32_t, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253> mt19937;
		mt19937 eng(static_cast<unsigned long>(0));
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(sd);
		mersen
		cout << "default seed = " << generator.default_seed << "\n";
		cout << "max = " << generator.max() << "\n";
		cout << "min = " << generator.min() << endl;
		*/

		//------------------------------------------------------------------------------------------------------------------------
		/*
		// Test program for index search of pixels after relocation caused for X-shift or Y-shift
		int nrh = 8;
		int nrl = nrh-1;
		int shiftX = -5;
		int shiftY = 0;
		int nPx = 68;
		IndexRelocate(shiftX, shiftY, nrh, nrl, nPx);

		//------------------------------------------------------------------------------------------------------------------------

		// This program generates correlated noise signal for a square/rectangular grid
		int nPixels = 16;				// start with 16 pixels first (4x4 matrix)
		int nCols = 4;
		int nRows = 4;
		int nFrames = 10;				// Total # of frames
		int frameLag = 3;				// The set of "framePersist" frames lag with the same time delay
		const int xLag = 2;				// # of columns to shift along X (+/-) direction == spatial X correlation
		const int yLag = 0;				// # of rows to shift along Y (+/-) direction == spatial Y correlation
		int framePersist = 3;           // # of frames with same intensity persisting on pixels

		double *memAssignRandInt = new double [framePersist * nPixels * frameLag];  // Assign static memory block - displays random values in array
		double *memAssignRand = new double [framePersist * nPixels * frameLag];   // Assign static memory block - stores normalized random values in raneg [0-1]
		double *memAssignDACOut = new double [framePersist * Pixels * frameLag];   // Assign dynamic memory block - stores DAC output
		for (int num=0; num<nFrames; num++){
			if (nFrames<frameLag)
			{
				cout << " \"nFrames\" cannot be less than \"frameLag\". Terminating execution!" << endl;
				abort();
			}
			GenCorrelatedNoiseRect(sd, nPixels, nRows, nCols, num, frameLag, xLag, yLag, framePersist, memAssignRandInt, memAssignRand, memAssignDACOut);
		}
		*/

		//------------------------------------------------------------------------------------------------------------------------

		//[11/28/2017]
		bool32		done = 0;
		char		errBuff[2048] = { '\0' };
		int32		totalReadCh0 = 0;
		int32		totalReadCh1 = 0;
		int32		error = 0;
		uInt32		NHalfBufs = 0;
		uInt32		NZSample = 0;
		uInt32		TotNZSample = 0;
		// bool32		done = 0;			  //commented out, multiple initalization error [4/18/2018]
		// char		errBuff[2048] = { '\0' }; //commented out, multiple initalization error [4/18/2018]
		// int32		error = 0;			  //commented out, multiple initalization error [4/18/2018]
		uInt32		jj = 0;
		// uInt32		NZSample = 0;		  //commented out, multiple initalization error [4/18/2018]
		// double		NHalfBufs = 0;		  //commented out, multiple initalization error [4/18/2018]
		// int32		totalReadCh0 = 0;	  //commented out, multiple initalization error [4/18/2018]
		// int32		totalReadCh1 = 0;	  //commented out, multiple initalization error [4/18/2018]
		uInt32		TotNFrames = 0;
		float		outputBuffer = 0;
		int16		outputBuffer_16 = 0;
		uInt32		outputBuffer_32 = 0;

		// Initialize variables for the Analog Output (Z-signal for 608 oscilloscope)
		int16		NumAOChannels = mValues.iNumNIAOChans;
		int16		NRepeats = mValues.iNumRepeats;
		int16		NRasterPoints = mValues.iNRasterPoints;
		int32 		NumAOSampWritten = 0;
		uInt32		NZSignalRepeat = 3;
		ifstream*	ptrAOFile = 0;
		double		StimMaxAmp = mValues.dStimMaxVoltVal;
		double		StimMinAmp = mValues.dStimMinVoltVal;
		double		StimSampRate = mValues.dStimSampRate;
		string		strAOFileName;
		double		TotalHalfBufs = 0;
		bool		AOAutoStart = mValues.bStimAutoStart;
		float64		AOStimTimeout = mValues.dStimTimeOut;
		float64		ActualSampRate = 0;
		uInt32		AOBufferSpaceFree = 0;
		uInt32		NFramesPerHalfBufer = 250;
		uInt32		AOOneChanBufSiz = NRasterPoints * NFramesPerHalfBufer; // This is to accommodate 250 frames per half buffer
		uInt32		AOHalfBuf_Siz = AOOneChanBufSiz * NumAOChannels * NZSignalRepeat + 250 * 3; //The last 3 is for a (X,Y,Z) point, included to permit (833*3+1)=2500 points in each frame
		uInt32		AOBuffer_Siz = AOHalfBuf_Siz * 2;	//KEY FOR THIS CODE WORKING IS THAT THE DIGITAL BUFFER BE > 2046 SAMPLES

		// Initialize variables for the Counters, and Digital Input Timestamps //[11/29/2017]
		int32		samplesPerChanRead = 0;
		int32		numSampsPerChan = mValues.slNumSampsPerChan; //This # is chosen using an estimated H1 firing rate of 500 spikes/s & => at least 4 seconds before acquiring 2000 samples
		uInt32		ReadBufferSize = mValues.ulReadBufferSize; //This # is chosen based on 2 estimates: 1)Smallish frequency of disk writes, 2)H1 firing rate ~500 Spikes/s
		double		CIRecTimeout = mValues.dCIRecTimeOut;
		ofstream*	ptrAIFileCh0 = 0;
		ofstream*	ptrAIFileCh1 = 0;
		string		strAIFileNameCh0;
		string		strAIFileNameCh1;


		//[11 / 29 / 2017]
		int SingleChannel = mValues.iNumUEIAOChans;
		char PhysicalAOChannels[15];
		if (SingleChannel == 1)
			sprintf_s(PhysicalAOChannels, "/Dev2/ao%i", NumAOChannels - 1); //sprintf() => _sprintf_s() 10/18/2017 [AWT]
		else if (SingleChannel == 0)
			sprintf_s(PhysicalAOChannels, "/Dev2/ao0:%i", NumAOChannels - 1); //sprintf() => _sprintf_s() 10/18/2017 [AWT]
		else {
			cout << "Menu Option J Set Incorrectly" << endl;
			return;
		}

		// Create and initialize arrays and vectors [11/29/2017]
		vector<uInt32>	readArrayCICh0;
		vector<uInt32>	readArrayCICh1;
		vector<int16>	iAOBuffer;
		readArrayCICh0.assign(ReadBufferSize, 0);
		readArrayCICh1.assign(ReadBufferSize, 0);
		iAOBuffer.assign(AOBuffer_Siz, 0);


		// Initialize the handle to the NI tasks [11/29/2017]
		TaskHandle  CO1Handle = 0;
		TaskHandle  CO2Handle = 0;
		TaskHandle  AOHandle = 0;

		// This program generates correlated noise signal for a hexagonal grid 		
		int nRowsHigh = 29;				// Max # of rows  [4, 5, 29] 
		int nRowsLow = 28;				// Min # of rows  [3, 4, 28]
		int nColsHigh = 15;				// Max # of columns  [3, 3, 15]
		int nColsLow = 14;				// Min # of columns  [2, 2, 14]
		int Pixels = (nColsHigh*nRowsHigh) + (nRowsLow*nColsLow);				// start with 16 pixels first (4x4 matrix)	
		int nFrames = 100;				// Total # of frames 
		int framePersist = 5;           // Each individual pixel retain its intensity over "framePersist" frames :: Time of persist=framePersist*2 ms
		int frameLag = 2;				// The lag between frames that are correlated (in uints of 2 ms)
		int16 xLag = 2;					// # of columns to shift along X (+/-) direction ==> spatial X correlation; int => int16 10/31/2017 [AWT]
		int16 yLag = 0;					// # of rows to shift along Y (+/-) direction ==> spatial Y correlation; int => int16 10/31/2017 [AWT]
		float alpha1 = 1.0;				// Normalizing factor 
		float alpha2 = 0.0;				// Normalizing factor
		float alpha3 = 0.5;				// Normalizing factor for Noise //[11/10/2017]
		float pairProp = 1.0;			// Proportion of Paired Pixels //[11/10/2017] //obsolete (?)


		//char *dirShift = "Up";	    // Choose "Up" or "Down" 


		// Find old pixel locations in the place of new pixel locations after displacement "shiftX" or "shiftY" or both.
		int16 *Loc1 = new int16[Pixels];   // Old pixel locations //changed from int => int16 10/19/2017 [AWT] (then changed back); int16 *, int16 10/31/2017 [AWT]
		int16 *Loc2 = new int16[Pixels];	// New pixel locations //changed from int => int16 10/19/2017 [AWT] (then changed back); int16 *, int16 10/31/2017 [AWT]
		if (xLag != 0)
		{
			if (abs(xLag) > (nColsHigh + nColsLow))
			{
				cout << "Spatial displacement cannot be greater than the number of columns. \nTerminating execution!" << endl;
				abort();
			}

			//int version
			//IndexRelocate(xLag, yLag, nRowsHigh, nRowsLow, nColsHigh, nColsLow, Pixels, oddXstepY, oddYstepX, Loc1, Loc2);

			//void version
			IndexRelocate(nTurns, xLagChange, yLagChange, nRowsHigh, nRowsLow, nColsHigh, nColsLow, Pixels, oddXstepY, oddYstepX, Loc2);
		}
		//for (int i=0; i<Pixels; i++){cout << "Loc1["<<i<<"]="<<Loc1[i]<<"\t Loc2["<<i<<"]="<<Loc2[i]<< "\n";}

		// Generate data for analog output buffer 
		unsigned short int *memAssignRandInt1 = new unsigned short int[Pixels * frameLag];	// Memory block for storing picture 1 
		unsigned short int *memAssignRandInt2 = new unsigned short int[Pixels];				// Memory block for storing picture 2
		unsigned short int *memAssignAddedNoise = new unsigned short int[Pixels];				//[11/10/2017]
		unsigned short int *memAssignMask = new unsigned short int[Pixels];						//[11/10/2017]
		//unsigned short int *memAssignPPP = new unsigned short int[ Pixels ]; //[11/10/2017]
		//unsigned short int *memAssignUnpairedP = new unsigned short int[ Pixels ]; //[11/10/2017]
		//unsigned short int *memAssignPairedP = new unsigned short int[ Pixels ]; //[11/10/2017]
		for (int num = 0; num < nFrames; num++)
		{
			if (nFrames < frameLag)
			{
				cout << " \"nFrames\" cannot be less than \"frameLag\". \nTerminating execution!" << endl;
				abort();
			}

			//GenCorrelatedNoiseHex(sd, Pixels, nRowsHigh, nRowsLow, nColsHigh, nColsLow, num, frameLag, xLag, yLag, framePersist, alpha1, alpha2, Loc1, Loc2, memAssignRandInt, memAssignRand, memAssignDACOut);
			GenCorrelatedNoiseHex_v2(sd, Pixels, nRowsHigh, nRowsLow, nColsHigh, nColsLow, num, frameLag, xLag, yLag, framePersist, alpha1, alpha2, alpha3, Loc1, Loc2, memAssignRandInt1, memAssignRandInt2, memAssignAddedNoise, memAssignMask, pairProp, PPPTrial, nFrames);
		}

		//DAQ ROUTINES [11/29/2017] 

		/*********************************************/
		// DAQmx Reset Device
		/*********************************************/
		DAQmxErrChk(DAQmxResetDevice("Dev2"));

		/*********************************************/
		// DAQmx Configure Code for Counter 1 - Timestamp channel 1
		/*********************************************/
		DAQmxErrChk(DAQmxCreateTask("CO1", &CO1Handle));
		DAQmxErrChk(DAQmxCreateCICountEdgesChan(CO1Handle, "Dev2/ctr0", "", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
		DAQmxErrChk(DAQmxCfgSampClkTiming(CO1Handle, "/Dev2/PFI9", 100000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 2000));
		DAQmxErrChk(DAQmxSetCICountEdgesTerm(CO1Handle, "", "/Dev2/100kHzTimebase"));

		/*********************************************/
		// DAQmx Configure Code for Counter 2 - Timestamp channel 2
		/*********************************************/
		DAQmxErrChk(DAQmxCreateTask("CO2", &CO2Handle));
		DAQmxErrChk(DAQmxCreateCICountEdgesChan(CO2Handle, "Dev2/ctr1", "", DAQmx_Val_Rising, 0, DAQmx_Val_CountUp));
		DAQmxErrChk(DAQmxCfgSampClkTiming(CO2Handle, "/Dev2/PFI4", 100000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 2000));
		DAQmxErrChk(DAQmxSetCICountEdgesTerm(CO2Handle, "", "/Dev2/100kHzTimebase"));
		DAQmxErrChk(DAQmxGetSampClkRate(CO2Handle, &ActualSampRate));	//Read the actual sample clock rate (eventually coerced depending on the hardware used).
		cout << "The time stamp rate is: " << ActualSampRate << endl << endl;

		/*********************************************/
		// DAQmx Configure Code for Analog Output
		/*********************************************/
		DAQmxErrChk(DAQmxCreateTask("AO", &
			AOHandle));
		DAQmxErrChk(DAQmxCreateAOVoltageChan(AOHandle, PhysicalAOChannels, "", StimMinAmp, StimMaxAmp, DAQmx_Val_Volts, NULL));
		DAQmxErrChk(DAQmxCfgSampClkTiming(AOHandle, "", StimSampRate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, AOOneChanBufSiz*NZSignalRepeat));
		DAQmxErrChk(DAQmxCfgOutputBuffer(AOHandle, AOBuffer_Siz / NumAOChannels)); // LOOK AT THIS!@!! Made this change and need to check. Switched AOHalfBuf_Siz for AOOneChanBufSiz
		DAQmxErrChk(DAQmxSetWriteRegenMode(AOHandle, DAQmx_Val_DoNotAllowRegen));
		DAQmxErrChk(DAQmxGetSampClkRate(AOHandle, &ActualSampRate));	//Read the actual sample clock rate (eventually coerced depending on the hardware used).
		cout << "The actual sample clock rate is: " << ActualSampRate << endl << endl;

		/*********************************************/
		// DAQmx Configure Start Trigger
		/*********************************************/
		DAQmxErrChk(DAQmxCfgDigEdgeStartTrig(AOHandle, "/Dev2/PFI7", DAQmx_Val_Rising)); // PFI6 -> PFI7 [11/13/2017]

		DAQmxSetArmStartTrigType(CO1Handle, DAQmx_Val_DigEdge);
		DAQmxSetDigEdgeArmStartTrigSrc(CO1Handle, "/Dev2/PFI7"); // PFI6 -> PFI7 [11/13/2017]
		DAQmxSetDigEdgeArmStartTrigEdge(CO1Handle, DAQmx_Val_Rising);

		DAQmxSetArmStartTrigType(CO2Handle, DAQmx_Val_DigEdge);
		DAQmxSetDigEdgeArmStartTrigSrc(CO2Handle, "/Dev2/PFI7"); // PFI6 -> PFI7 [11/13/2017]
		DAQmxSetDigEdgeArmStartTrigEdge(CO2Handle, DAQmx_Val_Rising);


		/*********************************************/
		// DAQmx Initial Analog Output Write Code
		/*********************************************/
		//DAQmxErrChk (DAQmxWriteBinaryI16(AOHandle,AOOneChanBufSiz*NZSignalRepeat*2,AOAutoStart,AOStimTimeout,DAQmx_Val_GroupByScanNumber,&iAOBuffer[0],&NumAOSampWritten,NULL));
		DAQmxErrChk(DAQmxWriteBinaryI16(AOHandle, AOBuffer_Siz / 3, AOAutoStart, AOStimTimeout, \
			DAQmx_Val_GroupByScanNumber, &iAOBuffer[0], &NumAOSampWritten, NULL));


		/*********************************************/
		// Everything is prepared, start operation
		/*********************************************/
		cout << "Continuously Producing Raster for Tektronix 608." << endl;
		cout << "Continuously Timestamping Data." << endl;
		cout << "Press any key to TERMINATE trial." << endl << endl;

		TotalHalfBufs = ((Pixels/ NFramesPerHalfBufer) * NRepeats);   // This part does not match with "AOAI_TimeStamp_RTInterp.cpp"
		TotalHalfBufs = ceil(TotalHalfBufs) + 0;
		cout << "Number of half buffers to play: " << TotalHalfBufs << endl;

		/*********************************************/
		// DAQmx Start Code
		/*********************************************/
		DAQmxErrChk(DAQmxStartTask(AOHandle));
		DAQmxErrChk(DAQmxStartTask(CO2Handle));
		DAQmxErrChk(DAQmxStartTask(CO1Handle));

		cout << "Reached main while loop." << endl;

		// CREATE FILE TO WRITE INTERPOLATED VALUES
		//ofstream* ptrInterpPixelVals = 0;
		//ptrInterpPixelVals = fnOpenFileToWrite( "C:\\Data\\Physiology\\FrameValues.dat" );
		//cout << "File for interpolated values opened." << endl;


		while (!done && !_kbhit() && NHalfBufs < TotalHalfBufs) //kbhit() => _kbhit() 10/18/2017 [AWT]
		{
			//********************************************
			// DAQmx record the digital events
			//********************************************
			DAQmxErrChk(DAQmxReadCounterU32(CO1Handle, DAQmx_Val_Auto, CIRecTimeout, &readArrayCICh0[0], ReadBufferSize, &samplesPerChanRead, NULL));
			if (samplesPerChanRead > 0) {
				totalReadCh0 += samplesPerChanRead;
				ptrAIFileCh0->write(reinterpret_cast<char*>(&readArrayCICh0[0]), sizeof(uInt32)*samplesPerChanRead);
				if (totalReadCh0%numSampsPerChan == 0) {
					cout << "Total # of Spikes Recorded on Ch 1: \t" << totalReadCh0 << endl;
				}
			}

			DAQmxErrChk(DAQmxReadCounterU32(CO2Handle, DAQmx_Val_Auto, CIRecTimeout, &readArrayCICh1[0], ReadBufferSize, &samplesPerChanRead, NULL));
			if (samplesPerChanRead > 0) {
				totalReadCh1 += samplesPerChanRead;
				ptrAIFileCh1->write(reinterpret_cast<char*>(&readArrayCICh1[0]), sizeof(uInt32)*samplesPerChanRead);
				if (totalReadCh1%numSampsPerChan == 0) {
					cout << "Total # of Spikes Recorded on Ch 2: \t" << totalReadCh1 << endl;
				}
			}

			//********************************************
			// DAQmx Check output buffer space available
			//********************************************
			DAQmxErrChk(DAQmxGetWriteSpaceAvail(AOHandle, &AOBufferSpaceFree));
			if (AOBufferSpaceFree > AOOneChanBufSiz) {
				cout << AOBufferSpaceFree << " \t" << AOOneChanBufSiz << "\t" << iAOBuffer[0] << endl;
				++NHalfBufs;
				clock_t start;
				start = clock();

				//changed num => nFrames in GenCorrelatedNoiseHex_v2()
				NZSample = GenCorrelatedNoiseHex_v2(sd, Pixels, nRowsHigh, nRowsLow, nColsHigh, nColsLow, nFrames, frameLag, xLag, yLag, framePersist, alpha1, alpha2, alpha3, Loc1, Loc2, memAssignRandInt1, memAssignRandInt2, memAssignAddedNoise, memAssignMask, pairProp, PPPTrial, nFrames);
				TotNFrames = TotNFrames + NZSample + 1;

				DAQmxErrChk(DAQmxWriteBinaryI16(AOHandle, AOHalfBuf_Siz / 3, 0, 1, DAQmx_Val_GroupByScanNumber, &iAOBuffer[0], \
					&NumAOSampWritten, NULL));

				cout << NHalfBufs << " ; AO Buffer Space: " << AOBufferSpaceFree << " ; AO Samples Written: " << NumAOSampWritten << endl;

				start = clock() - start; //////////////
				printf("\%f seconds \n", (float(start)) / CLOCKS_PER_SEC); /////////


																		   //ptrInterpPixelVals->write( reinterpret_cast<char *> (&iAOBuffer[0]), AOHalfBuf_Siz * sizeof (int16) );
			}
		}

		getchar();
		cout << "Number of half buffer transfered: " << NHalfBufs << endl;
		cout << "Number of samples transfered : " << TotNFrames << endl;



	Error:
		if (DAQmxFailed(error))
			DAQmxGetExtendedErrorInfo(errBuff, 2048);
		if (CO1Handle != 0) {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(CO1Handle);
			DAQmxClearTask(CO1Handle);
		}
		if (CO2Handle != 0) {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(CO2Handle);
			DAQmxClearTask(CO2Handle);
		}
		if (AOHandle != 0) {
			/*********************************************/
			// DAQmx Stop Code
			/*********************************************/
			DAQmxStopTask(AOHandle);
			DAQmxClearTask(AOHandle);
		}

		if (DAQmxFailed(error))
			printf("DAQmx Error: %s\n", errBuff);

		/*********************************************/
		// DAQmx Reset Device
		/*********************************************/
		ptrAIFileCh0->close();
		ptrAIFileCh1->close();
		//ptrInterpPixelVals->close();

		DAQmxErrChk(DAQmxResetDevice("Dev2"));

		return;


		//////////////////////

		delete[] memAssignRandInt1;
		delete[] memAssignRandInt2;
		delete[] memAssignAddedNoise;
		delete[] memAssignMask;
		//delete [] memAssignPPP; //[11/10/2017]
		//delete [] memAssignUnpairedP; //[11/10/2017]
		//delete [] memAssignPairedP; //[11/10/2017]
		delete[] Loc1;
		delete[] Loc2;

		//-------------------------------------------------------------------------------------------------------------------------

		cout << "\n" << "Press any key to exit." << endl;
		getchar();

		return;
	}
}
