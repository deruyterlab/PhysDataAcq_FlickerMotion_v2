#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include "NIDAQmx.h"
#include "FileCtrl.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------
ifstream* fnOpenLargeFileToRead(string strStimulusFilename, uInt64* ptrFrameNumElem) { 
	ifstream *ptrStimFile = new ifstream(); 
	ptrStimFile->open(strStimulusFilename.c_str(), ios::in|ios::binary);	// input, binary, move file pointer to end


	if (!ptrStimFile->good()) {
		cerr << " File could not be opened." << endl;
		exit(1);
	}
	else {
		cerr << "File opened successfully " << endl;
	}
	//cerr<<"BEFORE tellg"<<endl;
	//ptrStimFile->seekg(0,ios::end);
	////Calculate the number of points in the INPUT file

	//uInt64 slSizeFrameFile = ptrStimFile->tellg();
	//cerr<<"AFTER tellg"<<endl;
	//uInt64 slFrameNumElem = slSizeFrameFile/sizeof(double);
	//*ptrFrameNumElem = slFrameNumElem;
	//cout << "Stimulus File Size (bytes): " << slSizeFrameFile << endl;
	//cout << "# of Elements in Stimulus File: " << slFrameNumElem << endl;
	ptrStimFile->seekg(0, ios::beg);

	return ptrStimFile;
}

//--------------------------------------------------------------------------------------------------------------------------
ifstream* fnOpenFileToRead(string strStimulusFilename, uInt32* ptrFrameNumElem) {   // this is the function to open files that have been formatted to hold float values
	ifstream *ptrStimFile = new ifstream();											// define an ifstream pointer which we will use to point at the chosen strStimulusFilename
	ptrStimFile->open(strStimulusFilename.c_str(), ios::in|ios::binary|ios::ate);	// input, binary, move file pointer to end

	if (!ptrStimFile->good()) {
		cerr << "File could not be opened." << endl;
		exit(1);
	}
	else {
		cerr << "File opened successfully for read. " << endl;
	}

	//Calculate the number of points in the INPUT file
	uInt32 slSizeFrameFile = ptrStimFile->tellg();			// returns current pointer position in bytes of stimulus file pointer (ptrStimFile); (total size in bytes since ptrStimFile is moved to end of file)
	uInt32 slFrameNumElem = slSizeFrameFile/sizeof(float);  // returns number of elements (samples) counted through (in this case, elements are float type); (total since ptrStimFile counts through entire file)
	*ptrFrameNumElem = slFrameNumElem;						
	cout << "The file has: " << slFrameNumElem << " (samples), and " << slSizeFrameFile << " (bytes)" << endl << endl;
	ptrStimFile->seekg(0, ios::beg);						// sets the position of the stimulus file pointer to the beginning of the stimulus file stream

	return ptrStimFile;										// returns the pointer (now pointing at the beginning of the chosen stimulus file)
}

//--------------------------------------------------------------------------------------------------------------------------
ifstream* fnOpenFileToRead_uInt32(string strStimulusFilename, uInt32* ptrFrameNumElem) // this is the function to open files that have been formatted to hold uInt32 values
{
	ifstream *ptrStimFile = new ifstream(); 
	ptrStimFile->open(strStimulusFilename.c_str(), ios::in|ios::binary|ios::ate);	// input, binary, move file pointer to end

	if (!ptrStimFile->good()) {
		cerr << "File could not be opened." << endl;
		exit(1);
	}
	else {
		cerr << "File opened successfully for read. " << endl;
	}

	//Calculate the number of points in the INPUT file
	uInt32 slSizeFrameFile = ptrStimFile->tellg();
	uInt32 slFrameNumElem = slSizeFrameFile/sizeof(uInt32);
	*ptrFrameNumElem = slFrameNumElem;
	cout << "The file has: " << slFrameNumElem << " (samples), and " << slSizeFrameFile << " (bytes)" << endl << endl;
	ptrStimFile->seekg(0, ios::beg);

	return ptrStimFile;
}

//--------------------------------------------------------------------------------------------------------------------------
ifstream* fnOpenFileToRead_int16(string strStimulusFilename, uInt32* ptrFrameNumElem) // this is the function to open files that have been formatted to hold int16 values
{
	ifstream *ptrStimFile = new ifstream(); 
	ptrStimFile->open(strStimulusFilename.c_str(), ios::in|ios::binary|ios::ate);	// input, binary, move file pointer to end

	if (!ptrStimFile->good()) {
		cerr << "File could not be opened." << endl;
		exit(1);
	}
	else {
		cerr << "File opened successfully for read. " << endl;
	}

	//Calculate the number of points in the INPUT file
	uInt32 slSizeFrameFile = ptrStimFile->tellg();
	uInt32 slFrameNumElem = slSizeFrameFile/sizeof(int16);
	*ptrFrameNumElem = slFrameNumElem;
	cout << "The file has: " << slFrameNumElem << " (samples), and " << slSizeFrameFile << " (bytes)" << endl << endl;
	ptrStimFile->seekg(0, ios::beg);

	return ptrStimFile;
}


//--------------------------------------------------------------------------------------------------------------------------
ofstream* fnOpenFileToWrite(string strStimulusFilename) {

	ofstream *ptrStimFile = new ofstream(); 
	ptrStimFile->open(strStimulusFilename.c_str(), ios::out|ios::binary); // input, binary
	if (!ptrStimFile->good()) {
		cerr << "File could not be opened." << endl;
		exit(1);
	}
	else {
		cerr << "File opened successfully for write. " << endl << endl;
	}
	ptrStimFile->seekp(0, ios::beg);

	return ptrStimFile;
}

//--------------------------------------------------------------------------------------------------------------------------
void fnCloseFile(ifstream* ptrStimFile)
{
	ptrStimFile->close();

	if ( !ptrStimFile->good() ) {
		cerr << "File did not close." << endl;
		exit(1);
	}
	else {
		cerr << "File closed successfully " << endl;
	}
}	

//--------------------------------------------------------------------------------------------------------------------------
void LoadFullAOBuffer(double* pBuffer, int16 nbChannels, uInt32 nbSamplePerChannel, ifstream* ptrStimFile)
{
	//Initialize the arrays for the INPUT buffer
	double outputBuffer;

	//Read the data from the INPUT file into the INPUT buffer
	for(uInt64 i=0; i<nbSamplePerChannel; i++)
	{	
		for(int j=0; j<nbChannels; j++)
		{
			ptrStimFile->read( reinterpret_cast<char *>( &outputBuffer ), sizeof(double));
			if( ptrStimFile->bad() ) 
			{
				cerr << "Error reading data" << endl;
				exit( 0 );
			}	
			pBuffer[i*nbChannels+j] = outputBuffer; // assign the value of outputBuffer to the index [((current # sample per channel)*(total number of channels)) + (current # channel)] of array pBuffer
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
uInt32 ConstructAOBuffer(double* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, uInt32 NGridSamples, ifstream* ptrAOFile, double* ptrAO_X_File, double* ptrAO_Y_File) //changed first param. AND last two param.(s) from double* => ifstream* for compatability w/ fn calls
{
	//Initialize the arrays for the INPUT buffer
	uInt32 idx = 0; //added this line to fix 'undeclared identifier' 10/16/2017 [AWT]
	uInt32 idx2 = 0;
	uInt32 counter = 0;
	double outputBuffer = 0;

	//Read the data from the INPUT file into the INPUT buffer
	for(uInt32 idx=0; idx<nbSamplePerChannel; idx++)
	{	
		uInt32 idx2 = idx % NGridSamples;

		// X Signals
		pBuffer[counter++] = ptrAO_X_File[idx2]; // assign value at index [idx2] of array ptrAO_X_File to pBuffer[counter], increment counter by 1

		// Y signals
		pBuffer[counter++] = ptrAO_Y_File[idx2]; // assign value at index [idx2] of array ptrAO_Y_File to pBuffer[counter], increment counter by 1

		// Z signals
		ptrAOFile->read( reinterpret_cast<char *>( &outputBuffer ), sizeof(double));
		if( ptrAOFile->bad() ) {
			cerr << "Error reading data" << endl;
			exit( 0 );
		}	
		pBuffer[counter++] = outputBuffer; // assign value of outputBuffer (a double) to index [counter] of array pBuffer, increment counter by 1
	}
	return idx;
}	

//--------------------------------------------------------------------------------------------------------------------------
uInt32 ConstructAOBuffer_int16(int16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, 
								uInt32 NGridSamples, ifstream* ptrAOFile, int16* ptrAO_X_File, 
								int16* ptrAO_Y_File, uInt32 NZRpt, uInt32 TotNZSample)
{
	//Initialize the arrays for the INPUT buffer
	uInt32	idx			= 0;
	uInt32	idx2		= 0;
	uInt32	idx3		= 0;
	uInt32	counter		= 0;
	int16	outputBuffer= 0;

	//Read the data from the INPUT file into the INPUT buffer
	for( idx = 0; idx < nbSamplePerChannel; idx++ )
	{	
		idx2 = idx % NGridSamples;

		for ( idx3 = 0; idx3 < NZRpt; idx3++ )
		{
			// X Signals
			pBuffer[counter++] = ptrAO_X_File[idx2];

			// Y signals
			pBuffer[counter++] = ptrAO_Y_File[idx2];

			// Z signals
			if ( idx3 == 0 ) {
				pBuffer[counter++] = 0;
			}
			else if ( idx3 == 1 ) {
				if ( ( (idx + TotNZSample) % NZSigSamples ) == 0 ) {
					ptrAOFile->seekg(0, ios::beg );
					if (TotNZSample == 0)
						cout << endl << "First pass through: Z reset from FileCtrl" << endl;
					else
						cout << endl << "Z reset from FileCtrl" << endl;
				}

				ptrAOFile->read( reinterpret_cast<char *>( &outputBuffer ), sizeof(int16) );

				if( ptrAOFile->bad() ) {
					cerr << "Error reading data" << endl;
					exit( 0 );
				}	
				pBuffer[counter++] = outputBuffer; // Z signals
			}
			else {
				pBuffer[counter++] = outputBuffer;		 // Z signals
			}
		}
	}
	return idx;
}

//--------------------------------------------------------------------------------------------------------------------------
uInt32 ConstructAOBuffer_uInt16(uInt16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, uInt32 NGridSamples, ifstream* ptrAOFile, int16* ptrAO_X_File, int16* ptrAO_Y_File, uInt32 NZRpt, uInt32 TotNZSample)
{
	//Initialize the arras for the INPUT buffer
	uInt32	idx = 0;
	uInt32	idx2 = 0;
	uInt32	idx3 = 0;
	uInt32	counter = 0;
	uInt16	outputBuffer = 0;

	//Read the data from the INPUT file into the INPUT buffer
	for (idx = 0; idx < nbSamplePerChannel; idx++)
	{
		idx2 = idx % NGridSamples;

		for (idx3 = 0; idx3 < NZRpt; idx3++)
		{
			// X Signals
			pBuffer[counter++] = ptrAO_X_File[idx2];

			// Y signals
			pBuffer[counter++] = ptrAO_Y_File[idx2];

			// Z signals
			if (idx3 == 0) {
				pBuffer[counter++] = 0;
			}
			else if (idx3 == 1) {
				if (((idx + TotNZSample) % NZSigSamples) == 0) {
					ptrAOFile->seekg(0, ios::beg);
					if (TotNZSample == 0)
						cout << endl << "First pass through: Z reset from FileCtrl" << endl;
					else
						cout << endl << "Z reset from FileCtrl" << endl;
				}

				ptrAOFile->read(reinterpret_cast<char *>(&outputBuffer), sizeof(int16));

				if (ptrAOFile->bad()) {
					cerr << "Error reading data" << endl;
					exit(0);
				}
				pBuffer[counter++] = outputBuffer; // Z signals
			}
			else {
				pBuffer[counter++] = outputBuffer;		 // Z signals
			}
		}
	}
	return idx;
}

//--------------------------------------------------------------------------------------------------------------------------
/*
for(counter=0; counter<6000; counter++){
*ptrAO_X_File >> pBuffer[counter];
*ptrAO_Y_File >> pBuffer[counter];
*ptrAOFile >> pBuffer[counter];
}

ptrAO_X_File->seekg(0, ios::beg);
ptrAO_Y_File->seekg(0, ios::beg);

location = ptrAOFile->tellg();
if (location == ((NZSigSamples)*8)) {
cout << "idx: " << idx << ",  Pointer location: " << location << endl;	
ptrAOFile->seekg(0, ios::beg );
}

vector<double>	X;
X.assign(2000,0);
vector<double>	Y;
Y.assign(2000,0);
for(int idx=0; idx<2000; idx++){
*ptrAO_X_File >> X[idx];
*ptrAO_Y_File >> Y[idx];
}
ptrAO_X_File->seekg(0, ios::beg);
ptrAO_Y_File->seekg(0, ios::beg);

int16 counter=0;
for (uInt32 idx=0; idx<nbSamplePerChannel; idx++){
if (idx % 2000), temp=0; else temp=
pBuffer[counter]=X[temp];
}


}*/