/*
Current Version 2.02
*/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion_v2\PhysDataAcq_FlickerMotion\DynamicRandGen.h>
#include "MenuReturnValues.h"
#include "Menu.h"
#include "AOContDITimeStamp.h"
#include "DOContDITimeStamp.h"
#include "AOAIContDIStamp.h"
#include "AORepAISimul.h"
#include "AOContDITimeStamp_Int16.h"
#include "AO_TimeStamp_RTInterp.h"
#include "AOAI_TimeStamp_RTInterp.h"
#include "CorrelatedNoise.h"
#include "PPP+AN.h"

using namespace std;

//these external variables were added [11/16/2017] for an attempted solution to LNK2019 & LNK1120
extern int16* oddXstepY;
extern int16* oddYstepX;
extern int16 nTurns; //[11/10/2017]
extern int16* xLagChange; //[11/10/2017]
extern int16* yLagChange; //[11/10/2017]

int main(void)
{
	int16 iIndex = -1;
	Menu DAQmenu;
	MenuReturnValues mRetVal;

	while (true)
	{
		iIndex++;
		DAQmenu.fnReadUserInput(); // this line boots the menu UI
		mRetVal = DAQmenu.getMValues();

		/*********************************************/
		// Execute Appropriate DAQ Sub-routine
		/*********************************************/
		if (mRetVal.iRecordType == 1) {
			cout << "Continuous Analog Output (NI) and Timestamped DI." << endl;
			AOContDITimeStamp_Int16(mRetVal, iIndex); //Configuraiton for use with NI Analog output
		}

		if (mRetVal.iRecordType == 2) {
			cout << "Digital Output Trigger with Timestamped Digital Input." << endl;
			DOContDITimeStamp(mRetVal, iIndex); //Configuraiton for use with UEI-driven HEX Array
		}

		if (mRetVal.iRecordType == 3) {
			cout << "Continuous AO, AI, Timestamping, and RT Interpolation." << endl;
			AOAI_TimeStamp_RTInterp(mRetVal, iIndex);
			//AOAIContDIStamp( mRetVal, iIndex );
		}

		if (mRetVal.iRecordType == 4) {
			cout << "Continuous Analog Output, Timestamping, and RT Interpolation." << endl;
			AO_TimeStamp_RTInterp(mRetVal, iIndex); //Configuraiton to use for photoreceptor recordings
		}

		if (mRetVal.iRecordType == 5) {
			cout << "Find Cell. Continuous Presentation with No Analog Input." << endl;
			AOContDITimeStamp(mRetVal, iIndex);
			//AORepAISimul( mRetVal ); //Configuraiton to use for photoreceptor recordings
		}

		if (mRetVal.iRecordType == 6) {
			cout << "Flicker/AN/PPP." << endl;
			PPP_AN(mRetVal, iIndex);
		}

		cout << " ----------> End of trial <---------- " << endl;
		cout << endl;
		cout << endl;

	}
	return 0;
};