//Things to do:
// Need to add a way to re-read from a small x, y coordinate file since these values are repeated
//Add the input to the SCB to record a second channel and test the values recorded


#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <stdio.h>
#include <algorithm>
#include <windows.h>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>

#include "AOAI_TimeStamp_RTInterp.h"
#include "FileCtrl.h"
//#include "Interpolation.h"
#include "Interpolation2.h"
#include "MenuReturnValues.h"
#include "ArrayIndexRelocate.h"


#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

using namespace std;

void AOAI_TimeStamp_RTInterp( MenuReturnValues mValues, int idx )
{
	bool32		done=0;
	char		errBuff[2048]={'\0'};
	double		NHalfBufs=0;
	int32		totalReadCh0=0;
	int32		totalReadCh1=0;
	int32		error=0;
	uInt32		NZSample=0;
	uInt32		TotNFrames=0;
	float		outputBuffer = 0;
	int16		outputBuffer_16 = 0;
	uInt32		outputBuffer_32 = 0;
	uInt32		jj = 0;

	// Initialize variables for the Analog Input
	double		RecMaxAmp		= mValues.dRecMaxVoltVal;
	double		RecMinAmp		= mValues.dRecMinVoltVal;
	double		RecSampRate		= mValues.dRecSampRate;
	float64		AIRecTimeout	= mValues.dAIRecTimeOut;
	int16		NumAIChannels	= mValues.iNumAIChans;
	int32		NumAISampRead	= 0;
	ofstream*	ptrAIFile		= 0;
	uInt32		totalAISampRead	= 0;
	uInt32		AIOneChanBufSiz	= 200000;
	uInt32		AIBuffer_Siz	= AIOneChanBufSiz * NumAIChannels * 6;	//KEY FOR THIS CODE WORKING IS THAT THE DIGITAL BUFFER BE > 2046 SAMPLES
	uInt32		AIBufferSpaceFilled=0;
	string		strAIFileName;


	// Initialize variables for the Counters, and Digital Input Timestamps
	int32		samplesPerChanRead = 0;
	int32		numSampsPerChan = mValues.slNumSampsPerChan; //This # is chosen using an estimated H1 firing rate of 500 spikes/s & => at least 4 seconds before acquiring 2000 samples
	uInt32		ReadBufferSize  = mValues.ulReadBufferSize; //This # is chosen based on 2 estimates: 1)Smallish frequency of disk writes, 2)H1 firing rate ~500 Spikes/s
	double		CIRecTimeout	= mValues.dCIRecTimeOut;
	ofstream*	ptrAIFileCh0	= 0;
	ofstream*	ptrAIFileCh1	= 0;
	string		strAIFileNameCh0;
	string		strAIFileNameCh1;

	// Initialize variables for the Analog Output (Z-signal for 608 oscilloscope)
	int16		NumAOChannels	= mValues.iNumNIAOChans;
	int16		NRepeats		= mValues.iNumRepeats;
	int16		NRasterPoints	= mValues.iNRasterPoints;
	int32 		NumAOSampWritten= 0;
	uInt32		NZSignalRepeat	= 3;
	ifstream*	ptrAOFile		= 0;
	double		StimMaxAmp		= mValues.dStimMaxVoltVal;
	double		StimMinAmp		= mValues.dStimMinVoltVal;
	double		StimSampRate	= mValues.dStimSampRate;
	string		strAOFileName;
	double		TotalHalfBufs	= 0;

	bool		AOAutoStart		= mValues.bStimAutoStart;
	float64		AOStimTimeout	= mValues.dStimTimeOut;
	float64		ActualSampRate	= 0;
	uInt32		AOBufferSpaceFree=0;
	uInt32		AOOneChanBufSiz	= NRasterPoints * 250;  // This is to accommodate 250 frames per half buffer
	uInt32		AOHalfBuf_Siz	= AOOneChanBufSiz * NumAOChannels * NZSignalRepeat + 250 * 3; //The 3 is to permit (833*3+1)=2500 points in each frame
	uInt32		AOBuffer_Siz	= AOHalfBuf_Siz * 2;	//KEY FOR THIS CODE WORKING IS THAT THE DIGITAL BUFFER BE > 2046 SAMPLES

	int			framePersist	= mValues.iPersist;		// Number of frames over which sptial intensity profile does not change.		
	//float		alpha0			= mValues.alpha0;
	//float		alpha1			= mValues.alpha1;
	string		merge			= mValues.merge;
	

	// Temporary definitions of parameters to be define later by the user  			
	uInt16		nRowsHigh	= 29;							// Max # of rows  [4, 5, 29]
	uInt16		nRowsLow	= 28;							// Min # of rows  [3, 4, 28]
	uInt16		nColsHigh	= 15;							// Max # of columns  [3, 3, 15]
	uInt16		nColsLow	= 14;							// Min # of columns  [2, 2, 14]
	uInt16		Pixels		= (nColsHigh*nRowsHigh)+(nRowsLow*nColsLow);		
	uInt16		sd			= 0;							// Seed for random engine	

	
	int SingleChannel = mValues.iNumUEIAOChans;
	char PhysicalAOChannels[15];
	if (SingleChannel == 1)
		sprintf_s(PhysicalAOChannels, "/Dev2/ao%i", NumAOChannels-1); //sprintf() => sprintf_s() 10/18/2017 [AWT]
	else if (SingleChannel == 0)
		sprintf_s(PhysicalAOChannels, "/Dev2/ao0:%i", NumAOChannels-1); //sprintf() => sprintf_s() 10/18/2017 [AWT]
	else {
		cout << "Menu Option J Set Incorrectly" << endl;
		return;
	}

	// Create and initialize arrays and vectors
	vector<uInt32>	readArrayCICh0;
	vector<uInt32>	readArrayCICh1;
	vector<int16>	iAOBuffer;
	vector<double>	dAIBuffer;
	readArrayCICh0.assign(ReadBufferSize,0);
	readArrayCICh1.assign(ReadBufferSize,0);
	iAOBuffer.assign(AOBuffer_Siz,0);
	dAIBuffer.assign(AIBuffer_Siz,0);

	// Initialize the handle to the NI tasks
	TaskHandle  CO1Handle = 0;
	TaskHandle  CO2Handle = 0;
	TaskHandle  AOHandle  = 0;
	TaskHandle  AIHandle = 0;


	/*********************************************/	
	// Initialize the Response files
	/*********************************************/
	char fileIndex[65];
	_itoa_s(idx,fileIndex,10); //itoa() => _itoa_s() 10/18/2017 [AWT]
	cout << mValues.strRecFileDirPath << "\\" << mValues.strRecFileBaseName << fileIndex << "_Ch1.dat" << endl;
	strAIFileNameCh0 = mValues.strRecFileDirPath + "\\" + mValues.strRecFileBaseName + fileIndex + "_Ch1.dat";
	ptrAIFileCh0	 = fnOpenFileToWrite( strAIFileNameCh0 );

	cout << mValues.strRecFileDirPath << "\\" << mValues.strRecFileBaseName << fileIndex << "_Ch2.dat" << endl;
	strAIFileNameCh1 = mValues.strRecFileDirPath + "\\" + mValues.strRecFileBaseName + fileIndex + "_Ch2.dat";
	ptrAIFileCh1	 = fnOpenFileToWrite( strAIFileNameCh1 );

	strAIFileName = mValues.strRecFileDirPath + "\\" + mValues.strRecFileBaseName + "_AI_Samps.dat";
	ptrAIFile = fnOpenFileToWrite( strAIFileName );



	cout << "1. Loading World Map File." << endl;
	//---------------------------------------------
	// LOAD WORLD MAP
	string WorldMapFileName;
	WorldMapFileName = mValues.strStimFileDirPath + "\\" + "WorldMap.dat";
	uInt32 filesize_WorldMapVec;
	ifstream* ptrWorldMapVec = 0;
	ptrWorldMapVec = fnOpenFileToRead(WorldMapFileName,&filesize_WorldMapVec);

	float* WorldMapVec;
	WorldMapVec = new float [ filesize_WorldMapVec ];
	ptrWorldMapVec->read(reinterpret_cast<char *> (WorldMapVec), filesize_WorldMapVec * sizeof (float) );
	ptrWorldMapVec->close();
	float Height= sqrt( (float) filesize_WorldMapVec);
	float Width	= sqrt( (float) filesize_WorldMapVec);

	cout << "2. Loading Yaw Jitter File." << endl;
	//---------------------------------------------
	// LOAD YAW POSITION
	string YawPosFileName;
	YawPosFileName = mValues.strStimFileDirPath + "\\" + "YawPos.dat";
	uInt32 filesize_YawPos;
	vector<float> YawPosVec;
	ifstream* ptrYawPos = 0;
	ptrYawPos = fnOpenFileToRead(YawPosFileName,&filesize_YawPos);
	YawPosVec.assign(filesize_YawPos,0);
	for(jj = 0; jj < filesize_YawPos; jj++) {
		ptrYawPos -> read( reinterpret_cast<char *>( &outputBuffer ) , sizeof(float) );
		YawPosVec[jj] = outputBuffer;
	}
	ptrYawPos->close();

	cout << "3. Loading Pitch Jitter File." << endl;
	//---------------------------------------------
	// LOAD PITCH POSITION
	string PitchPosFileName;
	PitchPosFileName = mValues.strStimFileDirPath + "\\" + "PitchPos.dat";
	uInt32 filesize_PitchPos;
	vector<float> PitchPosVec;
	ifstream* ptrPitchPos = 0;
	ptrPitchPos = fnOpenFileToRead(PitchPosFileName,&filesize_PitchPos);
	PitchPosVec.assign(filesize_PitchPos,0);
	for(jj = 0; jj < filesize_PitchPos; jj++) {
		ptrPitchPos -> read( reinterpret_cast<char *>( &outputBuffer ) , sizeof(float) );
		PitchPosVec[jj] = outputBuffer;
	}
	ptrPitchPos->close();

	cout << "4. Loading Roll Jitter File." << endl;
	//---------------------------------------------
	// LOAD ROLL POSITION
	string RollPosFileName;
	RollPosFileName = mValues.strStimFileDirPath + "\\" + "RollPos.dat";
	uInt32 filesize_RollPos;
	vector<float> RollPosVec;
	ifstream* ptrRollPos = 0;
	ptrRollPos = fnOpenFileToRead(RollPosFileName,&filesize_RollPos);
	RollPosVec.assign(filesize_YawPos,0);
	for(jj = 0; jj < filesize_YawPos; jj++) {
		ptrRollPos -> read( reinterpret_cast<char *>( &outputBuffer ) , sizeof(float) );
		RollPosVec[jj] = outputBuffer;
	}
	ptrPitchPos->close();

	cout << "5. Loading Sampling Array Yaw Positions." << endl;
	//---------------------------------------------
	// 4. Loading Sampling Array Yaw Positions.
	string LED_XPosFileName;
	LED_XPosFileName = mValues.strStimFileDirPath + "\\" + "LED_XPos.dat";
	uInt32 filesize_LED_XPos;
	vector<float> LED_XPos;
	ifstream* ptrLED_XPos = 0;
	ptrLED_XPos = fnOpenFileToRead(LED_XPosFileName,&filesize_LED_XPos);
	LED_XPos.assign(filesize_LED_XPos,0);
	for(jj=0; jj<filesize_LED_XPos; jj++) {
		ptrLED_XPos -> read( reinterpret_cast<char *>( &outputBuffer ) , sizeof(float) );
		LED_XPos[jj] = outputBuffer;
	}
	ptrLED_XPos->close();

	cout << "6. Loading Sampling Array Pitch Positions." << endl;
	//---------------------------------------------
	// 5. Loading Sampling Array Pitch Positions.
	string LED_YPosFileName;
	LED_YPosFileName = mValues.strStimFileDirPath + "\\" + "LED_YPos.dat";
	uInt32 filesize_LED_YPos;
	vector<float> LED_YPos;
	ifstream* ptrLED_YPos = 0;
	ptrLED_YPos = fnOpenFileToRead(LED_YPosFileName,&filesize_LED_YPos);
	LED_YPos.assign(filesize_LED_YPos,0);
	for(jj=0; jj<filesize_LED_YPos; jj++) {
		ptrLED_YPos -> read( reinterpret_cast<char *>( &outputBuffer ) , sizeof(float) );
		LED_YPos[jj] = outputBuffer;
	}
	ptrLED_YPos->close();


	cout << "7. Loading X Pixels Positions." << endl;
	//---------------------------------------------
	// 6. Loading X Pixels Positions.
	ifstream* ptrXPixelPos = 0;
	uInt32 filesize_XPixelPos = 0;
	string strAO_X_FileName;
	vector<int16> X;

	strAO_X_FileName = mValues.strStimFileDirPath + "\\" + "XCoords.dat";
	ptrXPixelPos = fnOpenFileToRead_int16(strAO_X_FileName, &filesize_XPixelPos); //Call function to open Analog Output file
	X.assign(filesize_XPixelPos,0);
	for( uInt32 idx2=0; idx2<filesize_XPixelPos; idx2++ ) {
		ptrXPixelPos -> read( reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		X[idx2] = outputBuffer_16;
	}
	ptrXPixelPos->close();

	cout << "8. Loading Y Pixels Positions." << endl;
	//---------------------------------------------
	// 7. Loading Y Pixels Positions.
	ifstream* ptrYPixelPos = 0;
	uInt32 filesize_YPixelPos = 0;
	string strAO_Y_FileName;
	vector<int16> Y;

	strAO_Y_FileName = mValues.strStimFileDirPath + "\\" + "YCoords.dat";	//Name of Analog Stimulus file
	ptrYPixelPos = fnOpenFileToRead_int16(strAO_Y_FileName, &filesize_YPixelPos); //Call function to open Analog Output file
	Y.assign(filesize_YPixelPos,0);
	for( uInt32 idx2=0; idx2<filesize_YPixelPos; idx2++ ) {
		ptrYPixelPos->read( reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		Y[idx2] = outputBuffer_16;
	}
	ptrYPixelPos->close();

	cout << "9. Loading frame counter for flicker stimulus change." << endl;
	//---------------------------------------------
	// Loading frame # for flicker stimulus change
	string strAO_FlickerChange;
	ifstream* ptrFlickerChange;
	uInt32 filesize_FlickerChange;	
	vector<uInt32> stimChange;
	
	strAO_FlickerChange = mValues.strStimFileDirPath + "\\" + "Frame@StimChange.dat";
	ptrFlickerChange = fnOpenFileToRead_uInt32(strAO_FlickerChange, &filesize_FlickerChange);
	stimChange.assign(filesize_FlickerChange,0);
	for (uInt32 idx2=0; idx2<filesize_FlickerChange; idx2++ ) {
		ptrFlickerChange->read(reinterpret_cast<char *>( &outputBuffer_32 ), sizeof(uInt32));
		stimChange[idx2] = outputBuffer_32;
	}
	ptrFlickerChange->close();
	
	cout << "10. Loading X lags." << endl;
	//---------------------------------------------
	// Loading X lags corresponding to spatial correlations
	string strAO_XLag;
	ifstream* ptrXLag = 0;
	uInt32 filesize_XLag;
	vector<int16> xLagChange;

	strAO_XLag = mValues.strStimFileDirPath + "\\" + "xLag.dat";
	ptrXLag = fnOpenFileToRead_int16(strAO_XLag, &filesize_XLag);
	xLagChange.assign(filesize_XLag,0);
	for ( uInt32 idx2=0; idx2<filesize_XLag; idx2++ ) {
		ptrXLag->read(reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		xLagChange[idx2] = outputBuffer_16;
	}
	ptrXLag->close();
	
	cout << "11. Loading Y lags." << endl;
	//---------------------------------------------
	// Loading Y lags corresponding to spatial correlations
	string strAO_YLag;
	ifstream* ptrYLag = 0;
	uInt32 filesize_YLag;
	vector<int16> yLagChange;

	strAO_YLag = mValues.strStimFileDirPath + "\\" + "yLag.dat";
	ptrYLag = fnOpenFileToRead_int16(strAO_YLag, &filesize_YLag);
	yLagChange.assign(filesize_YLag,0);
	for ( uInt32 idx2=0; idx2<filesize_YLag; idx2++ ) {
		ptrYLag->read(reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		yLagChange[idx2] = outputBuffer_16;		
	}
	ptrYLag->close();
	

	cout<< "12. Loading shift directions." << endl;
	//---------------------------------------------
	// Loading step direction for odd # number of columns shifts
	string strStepX;
	ifstream *ptrStepX = 0;
	uInt32 filesize_StepX;
	vector<int16> oddYstepX; 

	strStepX = mValues.strStimFileDirPath + "\\" + "StepX.dat"; 
	ptrStepX = fnOpenFileToRead_int16(strStepX, &filesize_StepX);
	oddYstepX.assign(filesize_StepX,0);
	for ( uInt32 idx2=0; idx2<filesize_StepX; idx2++ ) {        		
		ptrStepX->read(reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		oddYstepX[idx2] = outputBuffer_16;		
	}
	ptrStepX->close();
	
	//---------------------------------------------
	// Loading step direction for odd # number of row shifts
	string strStepY;
	ifstream *ptrStepY = 0;
	uInt32 filesize_StepY;
	vector<int16> oddXstepY; 

	strStepY = mValues.strStimFileDirPath + "\\" + "StepY.dat"; 
	ptrStepY = fnOpenFileToRead_int16(strStepY, &filesize_StepY);
	oddXstepY.assign(filesize_StepY,0);
	for ( uInt32 idx2=0; idx2<filesize_StepY; idx2++ ) {        		
		ptrStepY->read(reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		oddXstepY[idx2] = outputBuffer_16;		
	}
	ptrStepY->close();


	cout<< "13. Loading Time lags." << endl;
	//---------------------------------------------
	// Loading time lags corresponding to delta time correlation
	string strAO_TimeLag;
	ifstream* ptrTimeLag = 0;	
	uInt32 filesize_TimeLag;
	vector<int16> deltaTChange; // is this correct (?) vector array had been deleted, but deletion left deltaTChange undefined [6/7/2018]

	strAO_TimeLag = mValues.strStimFileDirPath + "\\" + "tLag.dat";
	ptrTimeLag = fnOpenFileToRead_int16(strAO_TimeLag, &filesize_TimeLag);
	deltaTChange.assign(filesize_TimeLag,0);	
	for ( uInt32 idx2=0; idx2<filesize_TimeLag; idx2++ ) {
		ptrTimeLag->read(reinterpret_cast<char *>( &outputBuffer_16 ), sizeof(int16));
		//cout << outputBuffer_16 << "\t" << sizeof(outputBuffer_16) << endl;
		deltaTChange[idx2] = outputBuffer_16;
	}
	ptrTimeLag->close();



	cout<< "14. Loading Contrast1" << endl;
	//---------------------------------------------
	// Loading contrast1 for 1st image
	string strAO_Cont1;
	ifstream* ptrCont1 = 0;	
	uInt32 filesize_Cont1;
	vector<float> cont1;

	strAO_Cont1 = mValues.strStimFileDirPath + "\\" + "Cont1.dat";
	ptrCont1 = fnOpenFileToRead(strAO_Cont1, &filesize_Cont1);
	cont1.assign(filesize_Cont1,0);	
	for ( uInt32 idx2=0; idx2<filesize_Cont1; idx2++ ) {
		ptrCont1->read(reinterpret_cast<char *>( &outputBuffer ), sizeof(float));
		cont1[idx2] = outputBuffer;
	}
	ptrCont1->close();


	cout<< "15. Loading Contrast2" << endl;
	//---------------------------------------------
	// Loading contrast2 for 2st image
	string strAO_Cont2;
	ifstream* ptrCont2 = 0;	
	uInt32 filesize_Cont2;
	vector<float> cont2;

	strAO_Cont2 = mValues.strStimFileDirPath + "\\" + "Cont2.dat";
	ptrCont2 = fnOpenFileToRead(strAO_Cont2, &filesize_Cont2);
	cont2.assign(filesize_Cont2,0);	
	for ( uInt32 idx2=0; idx2<filesize_Cont2; idx2++ ) {
		ptrCont2->read(reinterpret_cast<char *>( &outputBuffer ), sizeof(float));
		cont2[idx2] = outputBuffer;
	}
	ptrCont2->close();




	/*****************************************************************/
	// Load indices of pixel location before and after shift
	/*****************************************************************/
	int16	nTurns		= filesize_FlickerChange;			// # of times the stimulus change during experiment	
	int16*	Loc2		= new int16 [(int16)Pixels*nTurns];		// New pixel locations
	int16*	tempLoc2	= new int16 [(NRasterPoints-6)];	// Current pixel locations
	int16*	Loc3		= new int16 [(int16)Pixels*nTurns];		// New pixel locations
	int16*	tempLoc3	= new int16 [(NRasterPoints-6)];		// Current pixel locations

	int16 *sortArray = new int16 [nTurns];
	int16 min_deltaT, max_deltaT;
	memcpy(sortArray, &deltaTChange[0], sizeof(int16)*nTurns);
	std::sort(sortArray, sortArray+nTurns);	
	min_deltaT = sortArray[0];	
	max_deltaT = sortArray[nTurns-1];
	//cout << "\n" << min_deltaT << "\t " << max_deltaT << endl;
	delete [] sortArray;

	IndexRelocate(nTurns, &xLagChange[0], &yLagChange[0], nRowsHigh, nRowsLow, nColsHigh, nColsLow, Pixels, &oddXstepY[0], &oddYstepX[0], Loc2);	
	for (uInt32 i=0; i<filesize_StepX; i++){ //changed i from int => uInt32 to avoid signed/unsigned mismatch [11/10/2017]
		oddXstepY[i]=-oddXstepY[i];
		oddYstepX[i]=-oddYstepX[i];
	}
	IndexRelocate(nTurns, &xLagChange[0], &yLagChange[0], nRowsHigh, nRowsLow, nColsHigh, nColsLow, Pixels, &oddXstepY[0], &oddYstepX[0], Loc3);	
	
	//for (int i=0; i<Pixels; i++){cout << "Loc1["<<i<<"]="<<Loc1[i]<<"\t Loc2["<<i<<"]="<<Loc2[i]<< "\n";}
	

	/*********************************************/	
	// Load Analog Output Buffer
	/*********************************************/
	uInt32 dTBlocks		 = abs(min_deltaT)*(min_deltaT<0) + abs(max_deltaT)*(max_deltaT>0) + 1;  // Blocks for dT; gives # of timelags 
	uInt32 numBlocks	 = (dTBlocks > (uInt32)framePersist) ? dTBlocks : (uInt32)framePersist;	 // # Blocks determined by whichever is greater
	uInt32 *picBufSize	 = new uInt32 [Pixels * numBlocks];			// Memory block for storing picture set 	
	uInt32 *memRandInt	 = new uInt32 [(NRasterPoints-6)];			// Memory block for storing time-lagged picture	
	uInt32 ref_Zero		 = abs(min_deltaT)*(min_deltaT<0);				// Set zero-reference for "picBufSize"

	uInt32 TypeCt		= 0;
	int16 frameLag		= 0;
	float alpha0		 = 1.0; 
	float alpha1		 = 1.0; 
	cout << "NGridSamples = " << filesize_XPixelPos << endl;


	NZSample = ConstructAOBuffer_RT_int16( &iAOBuffer[0],
		AOOneChanBufSiz * 2, filesize_YawPos, filesize_XPixelPos, NZSignalRepeat, TotNFrames, NumAOChannels,
		&X[0], &Y[0], &YawPosVec[0], &PitchPosVec[0], &RollPosVec[0], &LED_XPos[0], &LED_YPos[0], 
		&WorldMapVec[0], Height, Width, Loc2, tempLoc2, Loc3, tempLoc3, picBufSize, &stimChange[0], &deltaTChange[0], &xLagChange[0], 
		&yLagChange[0], &cont1[0], &cont2[0], framePersist, &frameLag, &TypeCt, numBlocks, ref_Zero, memRandInt, sd, &alpha0, &alpha1, 
		filesize_TimeLag, merge, mValues);

	TotNFrames = TotNFrames + NZSample + 1;
	cout << "Number of Z samples written: " << TotNFrames << endl << endl;
	NHalfBufs += 2;

	/*********************************************/
	// DAQmx Reset Device
	/*********************************************/
	DAQmxErrChk (DAQmxResetDevice("Dev2"));

	/*********************************************/
	// DAQmx Configure Code for Counter 1 - Timestamp channel 1
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("CO1",&CO1Handle));
	DAQmxErrChk (DAQmxCreateCICountEdgesChan(CO1Handle,"Dev2/ctr0","",DAQmx_Val_Rising,0,DAQmx_Val_CountUp));
	DAQmxErrChk (DAQmxCfgSampClkTiming(CO1Handle,"/Dev2/PFI9",100000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,2000));
	DAQmxErrChk (DAQmxSetCICountEdgesTerm(CO1Handle,"","/Dev2/100kHzTimebase"));

	/*********************************************/
	// DAQmx Configure Code for Counter 2 - Timestamp channel 2
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("CO2",&CO2Handle));
	DAQmxErrChk (DAQmxCreateCICountEdgesChan(CO2Handle,"Dev2/ctr1","",DAQmx_Val_Rising,0,DAQmx_Val_CountUp));
	DAQmxErrChk (DAQmxCfgSampClkTiming(CO2Handle,"/Dev2/PFI4",100000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,2000));
	DAQmxErrChk (DAQmxSetCICountEdgesTerm(CO2Handle,"","/Dev2/100kHzTimebase"));

	/*********************************************/
	// DAQmx Configure Code for Analog Output
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("AO",&AOHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(AOHandle,PhysicalAOChannels,"",StimMinAmp,StimMaxAmp,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(AOHandle,"",StimSampRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,AOOneChanBufSiz*NZSignalRepeat));
	DAQmxErrChk (DAQmxCfgOutputBuffer(AOHandle,AOBuffer_Siz/NumAOChannels)); // LOOK AT THIS!@!! Made this change and need to check. Switched AOHalfBuf_Siz for AOOneChanBufSiz
	DAQmxErrChk (DAQmxSetWriteRegenMode(AOHandle,DAQmx_Val_DoNotAllowRegen));
	DAQmxErrChk (DAQmxGetSampClkRate(AOHandle, &ActualSampRate));	//Read the actual sample clock rate (eventually coerced depending on the hardware used).
	cout << "The actual sample clock rate is: " << ActualSampRate << endl << endl;

	/*********************************************/
	// DAQmx Configure Code for AI
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("AI",&AIHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(AIHandle,"/Dev2/ai0","",DAQmx_Val_RSE,RecMinAmp,RecMaxAmp,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(AIHandle,"",RecSampRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,AIBuffer_Siz));
	DAQmxErrChk (DAQmxGetSampClkRate(AIHandle, &ActualSampRate));	//Read the actual sample clock rate (eventually coerced depending on the hardware used).
	cout << "The actual AI clock rate is: " << ActualSampRate << endl << endl;


	/*********************************************/
	// DAQmx Configure Start Trigger
	/*********************************************/
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(AOHandle,"/Dev2/PFI7",DAQmx_Val_Rising)); // PFI6 -> PFI7 [11/13/2017]
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(AIHandle,"/Dev2/PFI7",DAQmx_Val_Rising)); // PFI6 -> PFI7 [11/13/2017]
	
	DAQmxSetArmStartTrigType(CO1Handle,DAQmx_Val_DigEdge);
	DAQmxSetDigEdgeArmStartTrigSrc(CO1Handle,"/Dev2/PFI7"); // PFI6 -> PFI7 [11/13/2017]
	DAQmxSetDigEdgeArmStartTrigEdge(CO1Handle,DAQmx_Val_Rising);
	
	DAQmxSetArmStartTrigType(CO2Handle,DAQmx_Val_DigEdge);
	DAQmxSetDigEdgeArmStartTrigSrc(CO2Handle,"/Dev2/PFI7"); // PFI6 -> PFI7 [11/13/2017]
	DAQmxSetDigEdgeArmStartTrigEdge(CO2Handle,DAQmx_Val_Rising);


	/*********************************************/
	// DAQmx Initial Analog Output Write Code
	/*********************************************/
	//DAQmxErrChk (DAQmxWriteBinaryI16(AOHandle,AOOneChanBufSiz*NZSignalRepeat*2,AOAutoStart,AOStimTimeout,DAQmx_Val_GroupByScanNumber,&iAOBuffer[0],&NumAOSampWritten,NULL));
	DAQmxErrChk (DAQmxWriteBinaryI16(AOHandle,AOBuffer_Siz/3,AOAutoStart,AOStimTimeout,DAQmx_Val_GroupByScanNumber,&iAOBuffer[0],&NumAOSampWritten,NULL));

	/*********************************************/
	// Everything is prepared, start operation
	/*********************************************/
	cout << "Continuously Producing Raster for Tektronix 608." << endl;
	cout << "Continuously Timestamping Data." << endl;
	cout << "Press any key to TERMINATE trial." << endl << endl;
	
	TotalHalfBufs = ( filesize_YawPos * (NRasterPoints + 1) ) / AOOneChanBufSiz * NRepeats;
	TotalHalfBufs = ceil( TotalHalfBufs ) + 0;
	cout << "Number of half buffers to play: " << TotalHalfBufs << endl;

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(AOHandle));
	DAQmxErrChk (DAQmxStartTask(AIHandle));
	DAQmxErrChk (DAQmxStartTask(CO2Handle));
	DAQmxErrChk (DAQmxStartTask(CO1Handle));

	cout << "Reached main while loop." << endl;

	// CREATE FILE TO WRITE INTERPOLATED VALUES
	//ofstream* ptrInterpPixelVals = 0;
	//ptrInterpPixelVals = fnOpenFileToWrite( mValues.strRecFileDirPath + "\\" + mValues.strRecFileBaseName + "_FrameValues.dat" );
	//cout << "File for interpolated values opened." << endl;

	while ( !done && !_kbhit() && NHalfBufs < TotalHalfBufs ) //kbhit() => _kbhit() 10/18/2017 [AWT]
	{
		//********************************************
		// DAQmx record the digital events
		//********************************************
		DAQmxErrChk (DAQmxReadCounterU32(CO1Handle,DAQmx_Val_Auto,CIRecTimeout,&readArrayCICh0[0],ReadBufferSize,&samplesPerChanRead,NULL));
		if (samplesPerChanRead > 0) {
			totalReadCh0 += samplesPerChanRead;
			ptrAIFileCh0->write(reinterpret_cast<char*>(&readArrayCICh0[0]),sizeof(uInt32)*samplesPerChanRead);
			if (totalReadCh0%numSampsPerChan == 0) {
				cout << "Total # of Spikes Recorded on Ch 1: \t" << totalReadCh0 << endl;
			}
		}

		DAQmxErrChk (DAQmxReadCounterU32(CO2Handle,DAQmx_Val_Auto,CIRecTimeout,&readArrayCICh1[0],ReadBufferSize,&samplesPerChanRead,NULL));
		if (samplesPerChanRead > 0) {
			totalReadCh1 += samplesPerChanRead;
			ptrAIFileCh1->write(reinterpret_cast<char*>(&readArrayCICh1[0]),sizeof(uInt32)*samplesPerChanRead);
			if (totalReadCh1%numSampsPerChan == 0) {
				cout << "Total # of Spikes Recorded on Ch 2: \t" << totalReadCh1 << endl;
			}
		}


		//********************************************
		// DAQmx Check output buffer space available
		//********************************************
		DAQmxErrChk (DAQmxGetWriteSpaceAvail(AOHandle, &AOBufferSpaceFree));
		if(AOBufferSpaceFree > AOOneChanBufSiz) {
			++NHalfBufs;

			NZSample = ConstructAOBuffer_RT_int16( &iAOBuffer[0],
				AOOneChanBufSiz, filesize_YawPos, filesize_XPixelPos, NZSignalRepeat, TotNFrames, NumAOChannels,
				&X[0], &Y[0], &YawPosVec[0], &PitchPosVec[0], &RollPosVec[0], &LED_XPos[0], &LED_YPos[0], 
				&WorldMapVec[0], Height, Width, Loc2, tempLoc2, Loc3, tempLoc3, picBufSize, &stimChange[0], &deltaTChange[0], &xLagChange[0], 
				&yLagChange[0], &cont1[0], &cont2[0], framePersist, &frameLag, &TypeCt, numBlocks, ref_Zero, memRandInt, sd, &alpha0, &alpha1,
				filesize_TimeLag, merge, mValues); 

			TotNFrames = TotNFrames + NZSample + 1;

			DAQmxErrChk (DAQmxWriteBinaryI16(AOHandle,AOHalfBuf_Siz/3,0,1,DAQmx_Val_GroupByScanNumber,&iAOBuffer[0],&NumAOSampWritten,NULL));
			//DAQmxErrChk (DAQmxWriteBinaryI16(AOHandle,AOOneChanBufSiz*NZSignalRepeat,0,1,DAQmx_Val_GroupByScanNumber,&iAOBuffer[0],&NumAOSampWritten,NULL));

			cout << NHalfBufs << " ; AO Buffer Space: " << AOBufferSpaceFree << " ; AO Samples Written: " << NumAOSampWritten << endl << endl;

			//ptrInterpPixelVals->write( reinterpret_cast<char *> (&iAOBuffer[0]), AOHalfBuf_Siz * sizeof (int16) );
		}

		//********************************************
		// DAQmx Check Analog Input space available
		//********************************************
		DAQmxErrChk (DAQmxReadAnalogF64(AIHandle,-1,AIRecTimeout,DAQmx_Val_GroupByScanNumber,&dAIBuffer[0],AIBuffer_Siz,&NumAISampRead,NULL));
		if( NumAISampRead>0 ) {
			totalAISampRead += NumAISampRead;
			cout<<" Total Samples Read Per Channel : \t" << totalAISampRead << endl;
			ptrAIFile->write(reinterpret_cast<char*>(&dAIBuffer[0]),sizeof(double)*NumAISampRead*NumAIChannels);
		}

	}
	getchar();
	cout << "Number of half buffer transfered: " << NHalfBufs << endl;
	cout << "Number of samples transfered : " << TotNFrames << endl;


Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( CO1Handle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(CO1Handle);
		DAQmxClearTask(CO1Handle);
	}
	if( CO2Handle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(CO2Handle);
		DAQmxClearTask(CO2Handle);
	}
	if( AOHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(AOHandle);
		DAQmxClearTask(AOHandle);
	}

	if( AIHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(AIHandle);
		DAQmxClearTask(AIHandle);
	}

	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);

	/*********************************************/
	// DAQmx Reset Device
	/*********************************************/
	ptrAIFileCh0->close();
	ptrAIFileCh1->close();
	//ptrInterpPixelVals->close();
	ptrAIFile->close();

	DAQmxErrChk (DAQmxResetDevice("Dev2"));

	return;

}