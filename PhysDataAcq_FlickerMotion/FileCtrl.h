#ifndef FILECTRL_H
#define FILECTRL_H

#include <iostream>
#include <string>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include "NIDAQmx.h"

using namespace std;

ifstream* fnOpenLargeFileToRead(string strStimulusFilename, uInt64* ptrFrameNumElem);

ifstream* fnOpenFileToRead(string strStimulusFilename, uInt32* ptrFrameNumElem);

ofstream* fnOpenFileToWrite(string strStimulusFilename);

void fnCloseFile(ifstream* ptrStimFile);

void LoadFullAOBuffer(double* pBuffer, int16 nbChannels, uInt32 nbSamplePerChannel, ifstream* ptrStimFile);

uInt32 ConstructAOBuffer(double* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, uInt32 NGridSamples, ifstream* ptrStimFile, double* ptrAO_X_File, double* ptrAO_Y_File); // Changed last two parameters from double => ifstream [AWT] [4/30/2018] --> this caused further errors as these values are input as doubles, vector[double], AND as ifstream* at various points in the code.

ifstream* fnOpenFileToRead_int16(string strStimulusFilename, uInt32* ptrFrameNumElem);

ifstream* fnOpenFileToRead_uInt16(string strStimulusFilename, uInt32* ptrFrameNumElem); //added 10/19/2017 [AWT]

ifstream* fnOpenFileToRead_uInt32(string strStimulusFilename, uInt32* ptrFrameNumElem);

uInt32 ConstructAOBuffer_int16(int16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, uInt32 NGridSamples, ifstream* ptrAOFile, int16* ptrAO_X_File, int16* ptrAO_Y_File, uInt32 NZSignalRepeat, uInt32 TotNZSample);

uInt32 ConstructAOBuffer_uInt16(uInt16* pBuffer, uInt32 nbSamplePerChannel, uInt32 NZSigSamples, uInt32 NGridSamples, ifstream* ptrAOFile, ifstream* ptrAO_X_File, ifstream* ptrAO_Y_File, uInt32 NZSignalRepeat, uInt32 TotNZSample); //added 10/19/2017 [AWT]
//changed ptrAO_i_File parameters from uInt16 -> ifstream*

#endif