//**** This function assigns  the new locations of pixels caused by an odd number of column shift along X direction ("yaw") as elements of an array. Another array is assigned pixel numbers of the old locations. The number of elements of the array is equal to the total number of pixels. *// 


#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <typeinfo>
#include <vector>
#include <math.h>
#include <cmath>
#include <d:\Scripts\PhysDataAcqScripts\Visual Studio Projects\PhysDataAcq_FlickerMotion\PhysDataAcq_FlickerMotion\NIDAQmx.h>
#include "ArrayIndexRelocate.h"
using namespace std;

//int IndexRelocate(const int shiftX, const int shiftY, int nrh, int nrl, int nch, int ncl, int Pixels, int *Loc1, int *Loc2)
void IndexRelocate(int16 nTurns, int16 *xLagChange, int16 *yLagChange, uInt16 nrh, uInt16 nrl, uInt16 nch, uInt16 ncl,
					  uInt16 Pixels, int16* oddXstepY, int16* oddYstepX, int16* Loc) 
{
	
	
	int16 i, j, k;
	int16* LocX = new int16 [Pixels];						// X coordinates of pixels
	int16* LocY = new int16 [Pixels];						// Y coordinates of pixels
	int16* LocXtemp = new int16 [Pixels];					// X coordinates of pixels (in trasition between X and Y shifts)
	int16* LocYtemp = new int16 [Pixels];					// Y coordinates of pixels (in trasition between X and Y shifts)
	int16* LocXnew = new int16 [Pixels];					// new X coordinates of pixels 
	int16* LocYnew = new int16 [Pixels];					// new Y coordinates of pixels 
	int16  xLag; 
	int16  yLag;
	
	
	for (k=0; k<nTurns; k++)
	{
		xLag = xLagChange[k];
		yLag = yLagChange[k];
		// Array for pixel locations "Loc"
		for (i=0; i<Pixels; i++)
		{
			Loc[i+k*Pixels]=-1;						// After shift [-1 assigned for out of range coordinates]
		}


		// Generate X, Y position coordinates for all the pixels 
		int16 temp = nrh+nrl; 
		for (i=0; i<nch; i++)          // For [0,2,4,...,28] columns
		{
			for (j=0; j<nrh; j++)
			{
				LocX[j + temp*i] = i*2;
				LocY[j + temp*i] = j*2; 
			}
		}

		for (i=0; i<ncl; i++)          // For [1,3,5,...,27] columns
		{
			for (j=0; j<nrl; j++)
			{
				LocX[j + nrh + temp*i] = 1 + i*2;
				LocY[j + nrh + temp*i] = (nrh-1)*2 - 1 - j*2; 
			}
		}


		
		// Assign -1 to denote pixel locations outside the range that result from the spatial shift
		for (i=0; i<Pixels; i++)
		{
			LocXnew[i] = -1; 
			LocYnew[i] = -1;
			LocXtemp[i] = -1; 
			LocYtemp[i] = -1;
		}

	
		// Depending on magnitude and direction of displacement along X or Y axis, find the indices of the relocated pixels.
		
		//------------------ NO Motion -----------------------------// 
		if (xLag==0 && yLag==0)
		{
			memcpy(LocXnew, LocX, sizeof(int16)*Pixels);
			memcpy(LocYnew, LocY, sizeof(int16)*Pixels);
		}
		
		//------------------ Motion along X and/or Y ------------------//
		else 
		{
			// Step 1: Perform x-shift
			if (abs(xLag)%2 == 0)								// EVEN # of column shifts
			{
				for (i=0; i<Pixels; i++)
				{
					if (LocX[i]-xLag>=0 && LocX[i]-xLag<(nch+ncl))
					{
						LocXtemp[i] = LocX[i] - xLag;
					}
				}
				memcpy(LocYtemp, LocY, sizeof(int16)*Pixels);
			}
			else if (abs(xLag)%2 != 0)							// ODD # of column shifts
			{
				for (i=0; i<Pixels; i++)
				{
					if (LocX[i]-xLag>=0 && LocX[i]-xLag<(nch+ncl))
					{
						LocXtemp[i] = LocX[i] - xLag;
						//cout << "i=" << i << "\t LocX[i]="<< LocX[i] << "\tLocXtemp[i]="<< LocXtemp[i] <<endl;
					}
					if (LocY[i]+oddXstepY[k]>=0 && LocY[i]+oddXstepY[k]<(nrh+nrl))
					{
						LocYtemp[i] = LocY[i] + oddXstepY[k];
						//cout << "i=" << i << "\t LocY[i]="<< LocY[i] << "\tLocYtemp[i]="<< LocYtemp[i] <<endl;
					}
				}
			}

			
			// Step 2: Perform y-shift
			if (abs(yLag)%2 == 0)									// EVEN # of row shifts
			{
				for (i=0; i<Pixels; i++)
				{
					if (LocYtemp[i]+yLag>=0 && LocYtemp[i]+yLag<(nrh+nrl))
					{
						LocYnew[i] = LocYtemp[i] + yLag;
					}
				}
				memcpy(LocXnew, LocXtemp, sizeof(int16)*Pixels);
			}
			else if (abs(yLag)%2 != 0)								// ODD # of row shifts
			{
				for (i=0; i<Pixels; i++)
				{
					if (LocYtemp[i]+yLag>=0 && LocYtemp[i]+yLag<(nrh+nrl))
					{
						LocYnew[i] = LocYtemp[i] + yLag;
					}
					if (LocXtemp[i]+oddYstepX[k]>=0 && LocXtemp[i]+oddYstepX[k]<(nrh+nrl))
					{
						LocXnew[i] = LocXtemp[i] + oddYstepX[k];
					}
				}
			}
		}



		//------------------ Map coordinate of spatially displaced pixel to pixel index, of time-lagged frame-----------------//
		for (i=0; i<Pixels; i++)
		{
			if (!(LocXnew[i]<0) && !(LocYnew[i]<0))
			{
				Loc[i+k*Pixels] = (int16) (nrh*floor((LocXnew[i]+1)/2.0) + nrl*floor(LocXnew[i]/2.0) - 1 + 
					abs( (LocYnew[i]%2) * ((nrh*2+1)/2.0) - ((LocYnew[i]+2.0)/2.0) ) );
			}
		}


		////------------------ Correct for any pixel indices that lie outside the range [0, 826]--------------------------------//
		//for (i=0; i<Pixels; i++)
		//{
		//	Loc[i] = (Loc[i]>=Pixels) ? -1 : Loc[i]; 
		//}


		//cout << "---------------------------"<< endl;
		//for (i=0; i<100; i++) {
		//	cout << "i="<<i<<"\tLocXnew["<<i<<"]="<<LocXnew[i]<<"\tLocYnew["<<i<<"]="<<LocYnew[i]<<"\tLoc["<<i<<"]="<<Loc[i]<<endl;
		//}
		//cout << "---------------------------"<< endl;
		//for (i=727; i<Pixels; i++) {
		//	cout << "i="<<i<<"\tLocXnew["<<i<<"]="<<LocXnew[i]<<"\tLocYnew["<<i<<"]="<<LocYnew[i]<<"\tLoc["<<i<<"]="<<Loc[i]<<endl;
		//}
		//cout << "---------------------------"<< endl;
		
	}


	delete [] LocX;
	delete [] LocY;
	delete [] LocXtemp;
	delete [] LocYtemp;
	delete [] LocXnew;
	delete [] LocYnew;

	return;

};