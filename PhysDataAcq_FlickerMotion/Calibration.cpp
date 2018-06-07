
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <conio.h>
//#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>

#include "NIDAQmx.h"
#include "AORepAISimul.h"
#include "FileCtrl.h"

//#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

using namespace std;

extern TaskHandle hAItask; //added to accept hAItask as defined 10/31/2017 [AWT] Happy Halloween!
extern vector<double> AIBuffer; //added to accept AIBuffer as defined 10/31/2017 [AWT] Happy Halloween!
extern int16 ii; //added to accept i, ii (?) as defined 10/31/2017 [AWT] Happy Halloween!





//using namespace std; //added the #define since DAQmxErrChk was not being recognized as a function as well as the 'using namespace std;' to ensure readability of function 10/31/2017 [AWT] Happy Halloween!

// get the coefficients
//float64 ai0CoeffVals[4];
//DAQmxErrChk( DAQmxGetAIDevScalingCoeff(hAItask, "Dev1/ai1", ai0CoeffVals, 4) ) ;    // 4 = NUM_COEFF_VALS

// get the uncalibrated binary value
//float64 UnCalBinVal = AIBuffer[ii]; //capialized the 'b' in AIbuffer => AIBuffer (AIBuffer appears elsewhere in sln but AIbuffer does not) 10/31/2017 [AWT] Happy Halloween! // // switched to float64, undefined type [04/27/2018]

// get the calibrated, scaled voltage
//int dCalVoltVal = 0; // 10/19/2017 [AWT]; changed float64 to int 10/30/2017 [AWT]
//float64 dCalVoltVal =  ai0CoeffVals[0] + (ai0CoeffVals[1] * UnCalBinVal) + (ai0CoeffVals[2] * (UnCalBinVal*UnCalBinVal) ) + (ai0CoeffVals[3] * (UnCalBinVal*UnCalBinVal*UnCalBinVal) ); // switched to float64, undefined type [04/27/2018] // // switched UnCalBinVal^x to UnCalBinVal * UnCalBinVal * UnCalBinVal ('^' operator does not work with float64) [4/27/2018]

// then convert the voltage back to a CALIBRATED binary value (10.0 is 1/2 the p-to-p voltage).
//This is a bit of a kludge to be worked on later but should be only off  by 1 LSB
//float64 CalBinVal = 0; // 10/19/2017 [AWT]
//float64 CalBinVal = ((dCalVoltVal / 10.0) * 32767); // switched to float64, undefined type [04/27/2018]




//duplicate code EXCEPT for ao0 (below) v ai0 (above); commented out 10/19/2017 [AWT]

//float64 ao0CoeffVals[4];
//DAQmxErrChk( DAQmxGetAODevScalingCoeff(hAItask, "Dev1/ao1", ao0CoeffVals, 4) );    // 4 = NUM_COEFF_VALS

//AWT Notes [10/30/2017]:
//calibration to fly to screen? --> should work for raster geometry
//calibration for voltage? Intensity? Position?
//removed DAQmxErrChk from DAQmxGetA_DevScalingCoeff() functions; put coefficients into new arrays [5/7/2018]