#include "pch.h"
#include "ScanProcesses.h"

#define SPARRAYSIZE 1024
#define SPMAXSTRSIZE 64

ScanProcesses::ScanProcesses()
{
}


ScanProcesses::~ScanProcesses()
{
}


void ScanProcesses::Scan() {
	//Setup so that console properties can be changed
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD whiteFG = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED| FOREGROUND_INTENSITY;

	//goodReturn used throughout to check return values
	bool goodReturn = true;

	//Get current time
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);

	//Get a list of process ID's
	DWORD processList[SPARRAYSIZE];
	DWORD * numBytesReturned = new DWORD;
	goodReturn = EnumProcesses(processList, sizeof(processList), numBytesReturned);
	DWORD numProcesses = *numBytesReturned / sizeof(DWORD);
	printf("Number of Processes to be Scanned: %d\n", numProcesses);

	//Create the black list of DLL's which could be related to camera/audio input
	const int blackListLength = 9;
	PCWSTR blackList[blackListLength] = { L"vidcap.ax" ,L"Windows.Media.Devices.dll", L"mfAACEnc.dll",
		L"dsound.dll" , L"lvcod64.dll", L"mfsvr.dll", L"sti.dll", L"vfwwdm32.dll", L"WindowsCodecsRaw.dll"};

	//Sort processes into ascending PID
	std::sort(processList, processList + numProcesses);
	
	//An array of bad processes and a counter for how many are in it
	Process badProcesses[SPARRAYSIZE];
	int badProcessCount = 0;

	//Used to keep track of how far through program we are
	int tenPercent = (int) floor(numProcesses / 10);
	int percentDone = 0;

	//PID of process we want to check, set to -1 if we don't want to check any process
	int processInv = -1;

	//Iterate over every process ID
	for (unsigned int processCounter = 0; processCounter < numProcesses;processCounter++) {

		//Open Handle to process
		HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processList[processCounter]);
		
		//problemDetected used to signify whether a program is using a blackList DLL
		bool problemDetected = false;

		//If the process was loaded properly
		if (NULL != processHandle) {

			PWSTR processName = new wchar_t[SPMAXSTRSIZE];
			PWSTR moduleName = new wchar_t[SPMAXSTRSIZE];

			//Initialise list of modules present and dump all the modules into it
			//WOould have liked to do this to the correct size rather than an arbitary size, but that wasn't working
			HMODULE moduleList[SPARRAYSIZE];

			if (EnumProcessModulesEx(processHandle, moduleList, sizeof(moduleList), numBytesReturned, LIST_MODULES_ALL)) {
				//Get number of modules in process
				DWORD numModules = *numBytesReturned / sizeof(HMODULE);

				//Get name of process being looked at from its base module
				GetModuleBaseName(processHandle, moduleList[0], processName, SPMAXSTRSIZE);

				//Iterate over each module in the list checking to see if it is in the red list
				for (DWORD moudleCounter = 0; moudleCounter < numModules;moudleCounter++) {
					GetModuleBaseName(processHandle, moduleList[moudleCounter], moduleName, SPMAXSTRSIZE);

					//Iterate over each member of the redList
					for (int dllCounter = 0;dllCounter < blackListLength;dllCounter++) {

						//Compare moduleName against BlackList and check process being investigated
						if (!wcscmp(moduleName, blackList[dllCounter]) || processList[processCounter] == processInv) {

							//Let the user know there is a problem with a process just once
							if (!problemDetected) {
								SetConsoleTextAttribute(hConsole, whiteFG);
								printf("\n--------------------------\n");
								SetConsoleTextAttribute(hConsole, whiteFG | BACKGROUND_RED | BACKGROUND_INTENSITY);
								printf("Suspect Process: PID= %5d Name= %ls \n", processList[processCounter], processName);

								HANDLE processToken = NULL;
								_TOKEN_USER* userSID = (PTOKEN_USER) malloc(44);
								
								DWORD nameLength = SPMAXSTRSIZE;
								DWORD SidReturn;
								SID_NAME_USE SidType;
								
								LPWSTR accountName = new wchar_t[SPMAXSTRSIZE];
								LPWSTR domainName = new wchar_t[SPMAXSTRSIZE];
								
								if (OpenProcessToken(processHandle, TOKEN_QUERY, &processToken)) {
									if (GetTokenInformation(processToken, TokenUser, (LPVOID)userSID, 44, &SidReturn)) {
										if (LookupAccountSid(NULL, userSID->User.Sid, accountName, &nameLength, domainName, &nameLength, &SidType)) {
											printf("Username: %ls\\%ls\n", domainName,accountName);
										}
									}
								}
								free(userSID);
								if (processToken != NULL) {
									CloseHandle(processToken);
								}

							
									
								//Get timings of the Process
								// 0 = Creation Time		1 = Exit Time
								// 2 = Kernel Time			3 = User mode Time
								FILETIME timeArray[4];
								SYSTEMTIME readableTimeArray[4];
								GetProcessTimes(processHandle, &timeArray[0], &timeArray[1], &timeArray[2], &timeArray[3]);
								for (int i = 0; i < 4;i++) {
									FileTimeToLocalFileTime(&timeArray[i], &timeArray[i]);
									FileTimeToSystemTime(&timeArray[i], &readableTimeArray[i]);
								}
								//Print time
								printf("Process was created at %02d:%02d:%02d",
									readableTimeArray[0].wHour, readableTimeArray[0].wMinute, readableTimeArray[0].wSecond);

								//Print day
								if (readableTimeArray[0].wYear == currentTime.wYear
									&& readableTimeArray[0].wMonth == currentTime.wMonth
									&& readableTimeArray[0].wDay == currentTime.wDay) {
									printf(" - Today\n");
								}
								else {
									printf(" - on %02d/%02d/%04d\n",
										readableTimeArray[0].wDay, readableTimeArray[0].wMonth, readableTimeArray[0].wYear);
								}

							}
							SetConsoleTextAttribute(hConsole, whiteFG);
							printf("\tSuspect DLL in use: %ls \n", moduleName);
							problemDetected = true;

						}
					}
				}
				if (problemDetected) {
					SetConsoleTextAttribute(hConsole, whiteFG);
					printf("--------------------------\n");

					if (badProcessCount < SPARRAYSIZE) {
						badProcesses[badProcessCount].name = processName;
						badProcesses[badProcessCount].PID = processList[processCounter];
						
						badProcesses[badProcessCount].stillAlive = true;
						badProcessCount++;
					}

				}
			}
			else {
				//Had this problem with 32 bit vs 64 bit running where it can't enumerate modules
				printf("Can't Enumerate Modules: %d\tError = %d\n", processList[processCounter], GetLastError());
			}

		}
		else {
			//System level processes end up here, we don't really need to worry about them
			//printf("Can't open PID: %d\n", processList[i]);
		}

		//Close Handle to process
		CloseHandle(processHandle);
		

		//Little percent counter
		if (processCounter%tenPercent == 0) {
			SetConsoleTextAttribute(hConsole, whiteFG | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
			printf("%d%% ", percentDone);
			percentDone += 10;
			SetConsoleTextAttribute(hConsole, whiteFG);
			printf(" \n");
		}
	}

	printf("Number of possible bad processes found: %d\n", badProcessCount);

	if (badProcessCount > 0) {
		while (true) {
			DWORD PidToKill;
			printf("\n--------------------------\n");
			printf("Enter the PID of a process you want to kill\nUse 0 to exit\nPID: ");
			scanf_s("%d", &PidToKill);
			if (PidToKill == 0) {
				break;
			}
			boolean processFound = false;
			for (int i = 0; i < badProcessCount;i++) {
				if (badProcesses[i].PID == PidToKill && badProcesses[i].stillAlive) {
					printf("Are you sure you want to terminate %ls? [y/n]\n", badProcesses[i].name);
					char ans;
					scanf_s(" %c",&ans);
					if (ans == 'y' || ans == 'Y') {
						//Open Handle to process
						HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, false, PidToKill);
						if (processHandle != NULL) {
							printf("KILLING Process: %05d\n", badProcesses[i].PID);
							TerminateProcess(processHandle, 0);
							badProcesses[i].stillAlive = false;
						}
						else {
							printf("Error killing process, this program does not have the required permissions");
						}
					}
					processFound = true;
				}
			}
			if (!processFound) {
				printf("Error in finding that process\n");
			}
		}
	}
	else {
		printf("It seems you have a clear process list");
	}



}
