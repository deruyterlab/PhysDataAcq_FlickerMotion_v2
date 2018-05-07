#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "Menu.h"

using namespace std;

Menu::Menu(){}
void Menu::fnReadUserInput()
{
	bool starting = false;
	while ( !starting )
	{
		fnMenuSettingsOption( mValues );

		// CHANGE THE MENU VARIABLES
		char sel;
		cin >> sel;

		switch ( sel )
		{
		case '1':
			cout << "Enter Recording Type -----------------------------> ";
			cin >> mValues.iRecordType;
			break;
		case '2':
			cout << "Enter Recording Pathname -------------------------> ";
			cin.ignore();
			getline(cin, mValues.strRecFileDirPath);
			break;
		case '3':
			cout << "Enter Recording Filename Prefix ------------------> ";
			cin >> mValues.strRecFileBaseName;
			break;
		case '4':
			cout << "Enter Recording Sampling Rate --------------------> ";
			cin >> mValues.dRecSampRate;
			break;
		case '5':
			cout << "Enter Minimum Recording Amplitude ----------------> ";
			cin >> mValues.dRecMinVoltVal;
			break;
		case '6':
			cout << "Enter Maximum Recording Amplitude ----------------> ";
			cin >> mValues.dRecMaxVoltVal;
			break;
		case '7':
			cout << "Enter Number of AI Channels ----------------------> ";
			cin >> mValues.iNumAIChans;
			break;
		case '8':
			cout << "Enter Number of Timestamp Channels	---------------> ";
			cin >> mValues.iNumCIChans;
			break;
		case 'A':
			cout << "Enter Stimulus Pathname --------------------------> ";
			cin.ignore();
			getline(cin, mValues.strStimFileDirPath, '\n');
			break;
		case 'B':
			cout << "Enter Stimulus Filename --------------------------------> ";
			cin >> mValues.strStimFileName0;
			break;
		case 'C':
			cout << "Enter X-Coords Filename --------------------------------> ";
			cin >> mValues.strStimFileName1;
			break;
		case 'D':
			cout << "Enter Y-Coords Filename --------------------------------> ";
			cin >> mValues.strStimFileName2;
			break;


		case 'E':
			cout << "Enter Minimum Stimulus Amplitude ----------------> ";
			cin >> mValues.dStimMinVoltVal;
			break;
		case 'F':
			cout << "Enter Maximum Stimulus Amplitude ----------------> ";
			cin >> mValues.dStimMaxVoltVal;
			break;


		case 'G':
			cout << "Enter Aggregate Sampling Rate (Hz) ---------------> ";
			cin >> mValues.dStimSampRate;
			break;
		case 'H':
			cout << "Enter Number of Time to Repeat Stimulus ----------> ";
			cin >> mValues.iNumRepeats;
			break;
		case 'I':
			cout << "Enter Number of NI AO Stimulus Channels ----------> ";
			cin >> mValues.iNumNIAOChans;
			break;
		case 'J':
			cout << "Select A Range of Channels (=0) or a Single Channel (=1) ---------> ";
			cin >> mValues.iNumUEIAOChans;
			break;
		case 'K':
			cout << "Select The Number of Points in The Raster ---------> ";
			cin >> mValues.iNRasterPoints;
			break;
		case 'L': 
			cout << "Select the Number of Frames that Persist ---------> ";
			cin >> mValues.iPersist;
			break;
		case 'M':
			cout << "Choose to average over 2 NN pixels ---------> ";
			cin >> mValues.merge;
			break;
		case 'N':
			cout << "PPP Active (=1) or Inactive (=0) ---------> ";
			cin >> mValues.PPPTrial;
			break;


		case 'q':
			cout << "Program execution halted. Exiting application!" << endl;
			exit(-1);
			break;
		case '0':
			cout << "Starting hardware set-up and data acquisition." << endl;
			starting = true;
			break;
		case '\n':
		case '\t':
		case ' ':
			break;
		default:
			cout << " -------- OPTION NOT VALID ! -------- " << endl << endl;
			break;
		}

	}
}

void Menu::fnMenuSettingsOption ( MenuReturnValues mValues )
{
	string recordOptions = " 1	Types of Recording \n"
		"\t 1=AO_Timestamps \n"
		"\t 2=DO(Trigger)_DI(Timestamps) \n"
		"\t 3=AO_AI_Timestamps_RTInterp \n"
		"\t 4=AO_Timestamps_RTInterp \n"
		"\t 5=Find Cell \n"
		"\t 6=CN+AN+/-PPP"; //[11/14/2017]

	cout																				<<	endl;
	cout << "Neurophysiology Data Acquisition Program v2.0"								<<	endl;
	//	DISPLAY PROMPT TO CHANGE THE MENU VARIABLES
	cout																				<<	endl;
	cout << "**********************************************************************"	<<	endl;
	cout																				<<	endl;
	cout << "Program Options. Select a parameter value. Then press Enter!"				<<	endl;
	cout																				<<	endl;
	cout << recordOptions																<<  endl;
	cout																				<<	endl;
	cout << " 2	Recording Pathname"														<<	endl;
	cout << " 3	Recording Filename Prefix"												<<	endl;
	cout << " 4	Recording Sampling Rate	- (total S/s)"									<<	endl;
	cout << " 5	Minimum Recording Amplitude		- (V)"									<<  endl;
	cout << " 6	Maximum Recording Amplitude		- (V)"									<<  endl;
	cout << " 7	Number of NI AI Recording Channels	- (maximum:  2)"					<<	endl;
	cout << " 8	Number of NI CI Recording Channels	- (maximum:  1)"					<<	endl;
	cout																				<<	endl;
	cout << " A	Stimulus Pathname"														<<	endl;
	cout << " B	Stimulus Filename"														<<  endl;
	cout << " C	X-Coords Filename"														<<  endl;
	cout << " D	Y-Coords Filename"														<<  endl;
	cout << " E	Minimum Stimulus Amplitude		- (V)"									<<  endl;
	cout << " F	Maximum Stimulus Amplitude		- (V)"									<<  endl;
	cout << " G	Stimulus Sampling Rate		- (total S/s)"								<<  endl;
	cout << " H	Number of Times to Repeat Stimulus	-	"								<<	endl;
	cout << " I	# of NI AO Stimulus Channels	- (maximum:  4)"						<<	endl;
	cout << " J	# of UEI AO Stimulus Channels	- (maximum:  32)"						<<	endl; 
	cout																				<<	endl;
	cout << " K	# of Frames that persist 			 "									<<	endl;
	cout << " L Select the Number of Frames that Persist "								<<	endl; // [11/14/2017]
	cout << " M Merge Intensities"														<< endl; // [11/14/2017]
	cout << " N PPP active (=1/=0) "													<<  endl; // [11/14/2017]

	cout << "0	<-------------------------------------- Start Acquisition"				<<	endl;
	cout << "q	<-------------------------------------- Quit"							<<	endl;
	cout << "**********************************************************************"	<<	endl;
	cout																				<<	endl;
	//	DISPLAY THE VALUES OF THE MENU VARIABLES
	cout << "Current Settings"															<<	endl;
	cout																				<<	endl;
	cout << "Recording Type"															<<	endl;
	cout << " 1	" << setiosflags(ios::left) << setw(20) <<"Type of Recording"			<<	setw(16) << setiosflags(ios::right) << mValues.iRecordType				<<	endl;
	cout																				<<	endl;
	cout << "Recording Properties"														<<	endl;
	cout << " 2	" << setiosflags(ios::left) << setw(20) <<"Recording Pathname - "			<< setw(10) <<  setiosflags(ios::right) << mValues.strRecFileDirPath			<<  endl;
	cout << " 3	" << setiosflags(ios::left) << setw(25) <<"Recording Filename Prefix"		<< setw(21) <<  setiosflags(ios::right) << mValues.strRecFileBaseName		<<  endl;
	cout << " 4	" << setiosflags(ios::left) << setw(25) <<"Recording Sampling Rate (S/s)"	<< setw(17) <<  setiosflags(ios::right) << mValues.dRecSampRate				<<  endl;
	cout <<	" 5	" << setiosflags(ios::left) << setw(25) <<"Minimum recording amplitude (V)"	<< setw(15) <<  setiosflags(ios::right) << mValues.dRecMinVoltVal			<<  endl;
	cout <<	" 6	" << setiosflags(ios::left) << setw(25) <<"Maximum recording amplitude (V)"	<< setw(15) <<  setiosflags(ios::right) << mValues.dRecMaxVoltVal			<<  endl;
	cout << " 7	" << setiosflags(ios::left) << setw(25) <<"# of AI Recording Channels"		<< setw(20) <<  setiosflags(ios::right) << mValues.iNumAIChans				<<	endl;
	cout << " 8	" << setiosflags(ios::left) << setw(25) <<"# of DI Recording Channels"		<< setw(20) <<  setiosflags(ios::right) << mValues.iNumCIChans				<<	endl;
	cout																				<<	endl;
	cout << "Stimulus Properties"														<<	endl;
	cout << " A	" << setiosflags(ios::left) << setw(20) <<"Stimulus Pathname - "			<< setw(26) << mValues.strStimFileDirPath	<<  endl;
	cout << " B	" << setiosflags(ios::left) << setw(20) <<"Stimulus Filename - "			<< setw(26) << mValues.strStimFileName0		<<  endl;
	cout << " C	" << setiosflags(ios::left) << setw(20) <<"X-Coords Filename - "			<< setw(26) << mValues.strStimFileName1		<<  endl;
	cout << " D	" << setiosflags(ios::left) << setw(20) <<"Y-Coords Filename - "			<< setw(26) << mValues.strStimFileName2		<<  endl;

	cout <<	" E	" << setiosflags(ios::left) << setw(25) <<"Minimum stimulus amplitude (V)"	<< setw(16) <<  setiosflags(ios::right) << mValues.dStimMinVoltVal			<<  endl;
	cout <<	" F	" << setiosflags(ios::left) << setw(25) <<"Maximum stimulus amplitude (V)"	<< setw(16) <<  setiosflags(ios::right) << mValues.dStimMaxVoltVal			<<  endl;

	cout << " G	" << setiosflags(ios::left) << setw(25) <<"Stimulus Generation Rate (S/s)"	<< setw(16) << mValues.dStimSampRate		<<  endl;
	cout << " H	" << setiosflags(ios::left) << setw(25) <<"Number of Stimulus Repeats"		<< setw(20) << mValues.iNumRepeats			<<  endl;
	cout << " I	" << setiosflags(ios::left) << setw(25) <<"# of NI AO Stimulus Channels"	<< setw(18) << mValues.iNumNIAOChans		<<	endl;
	cout << " J	" << setiosflags(ios::left) << setw(25) <<"# of UEI AO Stimulus Channels"	<< setw(17) << mValues.iNumUEIAOChans		<<	endl;
	cout << " K	" << setiosflags(ios::left) << setw(25) <<"# of Raster Points to Display"	<< setw(17) << mValues.iNRasterPoints		<<	endl;
	cout << " L	" << setiosflags(ios::left) << setw(24) <<"# of Frames that Persist"		<< setw(22) << mValues.iPersist				<<	endl;
	cout << " M	" << setiosflags(ios::left) << setw(24) <<"Choose to average over 2 NN pixels"		<< setw(12) << mValues.merge		<<	endl;
	cout << " N " << setiosflags(ios::left) << setw(24) <<"PPP active (=1/=0)"				<< setw(20) << mValues.PPPTrial				<<  endl;
	cout																				<<	endl;
	cout << "**********************************************************************"		<<	endl;

}
