// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H


#include <Windows.h>	//Has lots of Windows Definitions in it


//For Internal Enumeration
#include <setupapi.h>	//Has all the SetupDi functions
#include <cfgmgr32.h>	//Get device ID - to get status code
#include <WinBase.h>	//System Power Management
#include <stdio.h>		//Printf etc
#include <cfg.h>

//Process Enumeration
#include <Psapi.h>		//General Process Enumeration
#include <cstdio>		//Sorting
#include<algorithm>		//SOrting
#include <WinUser.h>	//Wait for Input Idle
#include <RestartManager.h>		//Restart manager attempt

#endif //PCH_H
