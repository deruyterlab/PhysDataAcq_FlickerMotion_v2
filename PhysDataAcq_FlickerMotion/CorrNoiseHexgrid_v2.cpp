#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <string>
#include <algorithm> //added to include random_shuffle() [11/9/2017]
#include <cmath>
#include <math.h>
#include <iomanip>
#include "CorrelatedNoise.h"
#include "MenuReturnValues.h"
#include "Menu.h"
#include "ArrayIndexRelocate.h"

using namespace std;

//extern int Loc1; 
//extern int Loc2;



int GenCorrelatedNoiseHex_v2(const int sd, int Pixels, int nRowsHigh, int nRowsLow, int nColsHigh, int nColsLow, int frameCt, int frameLag, int xLag, int yLag, int framePersist, float alpha1, float alpha2, float alpha3, int *Loc1, int *Loc2, unsigned short int *memAssignRandInt1, unsigned short int *memAssignRandInt2, unsigned short int *memAssignAddedNoise, unsigned short int *memAssignMask, float pairProp, int PPPTrial, int nframes) // float pairProp added for PPP [11/9/2017]; int PPPTrial added for PPP [11/14/2017]; int nframes added [5/18/2018]
{

	unsigned short int *ptr1 = new unsigned short int [Pixels*sizeof(unsigned short int)]; // array of random integer values; range: [0, RAND_MAX]
	unsigned short int *ptr2 = new unsigned short int [Pixels*sizeof(unsigned short int)]; // array of random integer values; range: [0, RAND_MAX]
	unsigned short int *ptrSet1 = new unsigned short int[Pixels * sizeof(unsigned short int)]; // array of integer values to be populated with x1 subset for PPP [5/18/2018]
	unsigned short int *ptrSet2 = new unsigned short int[Pixels * sizeof(unsigned short int)]; // array of integer values to be populated with x2 subset for PPP [5/18/2018]
	unsigned short int *ptr12 = new unsigned short int [Pixels*sizeof(unsigned short int)]; // array of random integer values <ptr1, ptr2>; range: [0, RAND_MAX]
	double *ptrZ = new double [Pixels*sizeof(double)];		// DAC output in int16 precision
	float rndFlt;
	int i = 0; // added this index, was not initialized; initializing as int since Pixels is an int and i counts up to Pixels [AWT], [4/27/2018]

	// Initialize Added Noise terms [11/10/2017]
	unsigned short int *ptr3 = new unsigned short int[Pixels * sizeof(unsigned short int)]; // array of random integer values; range: [0, RAND_MAX]

	// Seed the generator for Added Noise random pixel values [11/10/2017]
	//because this is a custom seed, it should not correlate with the rand(sd) function used later in the program.
	std::random_device rd; //obtain a random number from hardware
	std:mt19937_64 ANSeed(rd()); //seed the generator
	std::uniform_int_distribution<> distr(0, 1);

	// Seed the generator for PPP random pixel values [11/10/2017]
	//because this is a custom seed, it should not correlate with the rand(sd) function used later in the program.
	/* std::random_device rd; // obtain a random number from hardware
	std::mt19937_64 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(1, 833); // define the range */ //COMMENTED block out as random_device and distr() were being redefined [4/18/2018]

	//Work-around for Added Noise + PPP functionality [11/10/2017]
	unsigned short *mask = new unsigned short int[Pixels * sizeof(unsigned short int)];
	for (int i; i < Pixels; i++) 
	{
		mask[distr(ANSeed)] = distr(ANSeed);		//takes values of either 0 or 1 at a random index in the 'mask' array; does this avoid repeated random indices? [11/10/2017]
	}

	// Initialize PPP terms [11/9/2017]
	//int NPairsMax = Pixels;
	//int NPairs = ceil(pairProp * NPairsMax); // use ceil() or floor() here? [11/9/2017]
	//int nNonPairs = (NPairsMax - NPairs + 1);

	//unsigned short int *pairList = NULL;
		//cin >> NPairsMax;
		//pairList = new unsigned short int [NPairsMax]; //[11/9/2017]
		//for (int p = 0; p < NPairsMax; p++)
		//{
			//pairList[p] = distr(eng); // generate numbers
		//}

	//unsigned short int *pairListMotion = NULL; 
		//cin >> NPairs;
		//pairListMotion = new unsigned short int [NPairs];
		//for (int i = 0; i < NPairs; i++)
		//{
			//pairListMotion[i] = pairList[i];
		//}

	//unsigned short int *pairListFlicker = NULL;
		//cin >> nNonPairs;
		//pairListFlicker = new unsigned short int[nNonPairs];
		//for (int i = 0; i < nNonPairs; i++)
		//{
			//pairListFlicker[i] = pairList[i + NPairs];
		//}

	// Write random values to data file in hard disc to analyze later using Matlab; should these be motion steps (?)
	const char * path = "D:\Scripts\PhysDataAcqScripts\Correlator Model"; // C:\\Users\\Suva\\Documents\\MATLAB\\CorrelatorModel\\RandVals_cpp_ + X.dat
	ofstream fout; 
	if (frameCt==0)
	{
		fout.open(path, ios_base::out | ios_base::binary);
	}
	else
	{
		fout.open(path, ios_base::out | ios_base::app | ios_base::binary);
	}	

	// Set seed for random number generator
	if (frameCt==0)
	{
		srand(sd);
	}

	//----------------------Populate buffer for the first set of frames----------------------------
	if (frameCt< (int) (frameLag))
	{
		if ( frameCt % framePersist == 0 )  // first frame of every set of persisting frames
		{
			cout << "Frame = " << frameCt  << endl; 
			for (int i=0; i<Pixels; i++)
			{				
				ptr1[i] = rand();
				ptr2[i] = rand();
				ptrSet1[i] = (*ptr1 + nframes) % nframes + 1; // PPP term for x1, time shifted [5/18/2018]
				ptrSet2[i] = (*ptr1 + (nframes / 3) - 1) % nframes + 1; // PPP term for x2 [5/18/2018]
				ptr3[i] = distr(ANSeed);
				memAssignRandInt1[frameCt*Pixels + i] = ptr1[i];
				memAssignRandInt2[i] = ptr2[i];		//[11/10/2017]
				memAssignAddedNoise[i] = ptr3[i];	//[11/10/2017]
				memAssignMask[i] = mask[i];			//[11/10/2017]
				if (PPPTrial = 1) //case with AN + PPP active [11/14/2017]/[5/18/2018]
				{
					ptr12[i] = (alpha1 * ptr1[i]) + (alpha2 * (ptrSet1[i] + ptrSet2[i] )) + (alpha3 * ptr3[i]);		// [11/10/2017] // mask factors added [11/13/2017] // altered for PPP [5/18/2018]
				}
				else //case with Added Noise but no PPP [11/14/2017]
				{
					ptr12[i] = (alpha1 * ptr1[i]) + (alpha2 * ptr2[i]) + (alpha3 * ptr3[i]);
				}
				rndFlt = (float) ptr12[i]/((float) RAND_MAX/0.7);           
				ptrZ[i] = 2.0 * rndFlt * pow(2.0,15.0) / 10.0;							// "pBuffer" in the interpolation script
				//cout << "RandInt1[" << i << "]=" << ptr1[i] << "\t RandInt2[" << i << "]=" << ptr2[i] << "\n"; 
			}
		}
		else
		{
			cout << "Frame = " << frameCt  << endl; 
			memmove(ptr1, memAssignRandInt1 + (frameCt-1)*Pixels, Pixels*sizeof(unsigned short int)); 
			memmove(memAssignRandInt1 + frameCt*Pixels, memAssignRandInt1 + (frameCt-1)*Pixels, Pixels*sizeof(unsigned short int)); 
			memmove(ptr2, memAssignRandInt2, Pixels*sizeof(unsigned short int)); 
						
			for (int i=0; i<Pixels; i++)
			{
				if (PPPTrial = 1) //case with AN + PPP active [11/14/2017]/[5/18/2018]
				{
					ptr12[i] = (alpha1 * ptr1[i]) + (mask[i] * alpha2 * ptr2[i]) + ((1 - mask[i]) * alpha3 * ptr3[i]);		// [11/10/2017] // mask factors added [11/13/2017]
				}
				else //case with Added Noise but no PPP [11/14/2017]
				{
					ptr12[i] = (alpha1 * ptr1[i]) + (alpha2 * ptr2[i]) + (alpha3 * ptr3[i]);
				}
				rndFlt = (float) ptr12[i]/((float) RAND_MAX/0.7); 
				ptrZ[i] = 2.0 * rndFlt * pow(2.0,15.0) / 10.0;							 // "pBuffer" in the interpolation script
				//cout << "RandInt1[" << i << "]=" << memAssignRandInt1[i+frameCt*Pixels] << "\t RandInt2[" << i << "]=" << memAssignRandInt2[i] << "\n"; 
			}			
		}
		fout.write(reinterpret_cast<const char*>(ptr12), sizeof(unsigned short int)*Pixels);			
	}

	//-----------------Populate buffer for the correlated frames---------------------------------------
	else 
	{
		cout << setfill('-') << setw(80) << "\n" << endl;  
		cout << "FrameCt = " << frameCt << endl;
		
		int idxDest;
		int idxSource;
		int numBlocks;
	
		// Column shift can be +ve'ly or -ve'ly directed. Even or odd number of column shift is accounted for by "Loc2" parameter.
		
		// Generate picture 1
		if (frameCt%framePersist != 0)
		{
			memmove(ptr1, memAssignRandInt1+(frameLag-1)*Pixels, sizeof(unsigned short int)*Pixels);	// copy previous frame rand() values
		}
		else 
		{
			for (int i=0; i<Pixels; i++) 
			{ 
				ptr1[i] = rand(); 
			}
		}

		// Generate picture 2
		if ((frameCt-frameLag)%framePersist == 0)
		{
			for (int i=0; i<Pixels; i++) 
			{
				if (Loc2[i]==-1)
				{
					ptr2[i] = rand(); 
				}
				else 
				{
					ptr2[i] = memAssignRandInt1[Loc2[i]];
				}
			}
			memmove(memAssignRandInt2, ptr2, sizeof(unsigned short int)*Pixels);
		}
		else if ((frameCt-frameLag)%framePersist != 0)
		{
			memmove(ptr2, memAssignRandInt2, sizeof(unsigned short int)*Pixels);
		}

		// Generated Added Noise
		if ((frameCt) % framePersist != 0)	//if this is not the beginning of a new frame....
		{
			memmove(ptr3, memAssignAddedNoise+(frameLag-1)*Pixels, sizeof(unsigned short int)*Pixels); //copy previous frames randomized pixel values for noise
		}
		else
		{
			for (int i = 0; i < Pixels; i++)
			{
				ptr3[i] = distr(ANSeed);
			}
		}

		// Generate frame from picture 1 and picture 2 w/ Added Noise
		for (int i = 0; i < Pixels; i++)
		{

			if (PPPTrial = 1) //case with PPP active [11/14/2017]
			{
				ptr12[i] = (alpha1 * ptr1[i]) + (mask[i] * alpha2 * ptr2[i]) + ((1 - mask[i]) * alpha3 * ptr3[i]);		// [11/10/2017] // mask factors added [11/13/2017]
			}

			else //case with Added Noise but no PPP [11/14/2017]
			{
				ptr12[i] = (alpha1 * ptr1[i]) + (alpha2 * ptr2[i]) + (alpha3 * ptr3[i]);
			}

			rndFlt = (float)ptr12[i] / ((float)RAND_MAX / 0.7);								// 0.7 approx. to 1/2 sqrt(2); or voltage ? [11/9/2017]
			ptrZ[i] = 2.0 * rndFlt * pow(2.0, 15.0) / 10.0;									// This is "pBuffer" in the interpolation script //max output of generator 10 V ? [11/9/2017]
																							//cout << "ptr1[" << i << "]=" << ptr1[i] << "\t ptr2[" << i << "]=" << ptr2[i] << "\t Loc1["<<i<<"]="<<Loc1[i]<<"\t Loc2["<< i<<"]="<<Loc2[i]<<"\n"; 
			
		}

		// Write data to file. Shift data chunks by 1 memory block.
		fout.write(reinterpret_cast<const char*>(ptr12), sizeof(unsigned short int)*Pixels);
		memmove(memAssignRandInt1, memAssignRandInt1+Pixels, sizeof(unsigned short int)*frameLag*Pixels);
		memmove(memAssignRandInt1+(frameLag-1)*Pixels, ptr1, sizeof(unsigned short int)*Pixels);

		//for (int i=0; i<Pixels; i++){cout<<"mem1["<<i<<"]="<<memAssignRandInt1[i]<<";  mem1["<<i+Pixels<<"]="<<memAssignRandInt1[i+Pixels]<<";  mem1["<<i+2*Pixels<<"]="<<memAssignRandInt1[i+Pixels*2]<<"\n";}
		
	}


	fout.close();
	delete [] ptr1;
	delete [] ptr2;
	delete [] ptr3; //[11/10/2017]
	delete [] mask; //[11/10/2017]
	delete [] ptr12;
	//delete [] pairList; //[11/10/2017]
	//delete [] pairListFlicker; //[11/10/2017]
	//delete [] pairListMotion; //[11/10/2017]
	return 0;
}




