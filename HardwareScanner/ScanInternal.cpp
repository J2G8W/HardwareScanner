#include "pch.h"
#include "ScanInternal.h"
#include "ScanProcesses.h"

#define SIARRAYSIZE 64
#define SIMAXSTRSIZE 128

ScanInternal::ScanInternal()
{
}


ScanInternal::~ScanInternal()
{
}


void ScanInternal::Scan(int option) {
	//Get the handle to the console so we can change the colours in the future
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD whiteFG = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |FOREGROUND_INTENSITY;
	
	
	//Gets the most general amount of info of all devices possible! 
	HDEVINFO hDevInfo;
	hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);

	//Gets GUIDS of devices we want to look at
	const int numDevGUIDs = 3;
	_GUID devGUIDs[numDevGUIDs];

	//Hardcoded - Internal Cameras
	CoCreateGuid(&devGUIDs[0]);
	LPCOLESTR strGUID = OLESTR("{ca3e7ab9-b4c3-4ae6-8251-579ef933890f}");
	CLSIDFromString(strGUID, &devGUIDs[0]);

	//Hardcoded - Scanners + Common External Devices
	CoCreateGuid(&devGUIDs[1]);
	strGUID = OLESTR("{6bdd1fc6-810f-11d0-bec7-08002be2092f}");
	CLSIDFromString(strGUID, &devGUIDs[1]);


	//Hardcoded - AudioEndpoint includes Speakers and Headphones
	CoCreateGuid(&devGUIDs[2]);
	strGUID = OLESTR("{c166523c-fe0c-4a94-a586-f1a80cfbbf3e}");
	CLSIDFromString(strGUID, &devGUIDs[2]);

	
	//Object to recieve information about individual device
	_SP_DEVINFO_DATA * deviceInfo = new _SP_DEVINFO_DATA;
	deviceInfo->cbSize = sizeof(_SP_DEVINFO_DATA);

	//Object to recieve info about individual drivers
	SP_DRVINFO_DATA * driverInfo = new SP_DRVINFO_DATA;
	driverInfo->cbSize = sizeof(SP_DRVINFO_DATA);

	//Loop variables
	BOOL success = true;	//Used to decide whether we are at the end of the Device List
	DWORD count = 0;		//Counts how many devices we have looked at

	
	while (success) {
		
		//Various strings to recieve information
		PWSTR deviceClassName = new wchar_t[SIMAXSTRSIZE];
		PWSTR devId = new wchar_t[SIMAXSTRSIZE];
		PWSTR deviceName = new wchar_t[SIMAXSTRSIZE];
		PWSTR physicalName = new wchar_t[SIMAXSTRSIZE];
		PWSTR serviceName = new wchar_t[SIMAXSTRSIZE];


		//Enumerate the information from hDevInfo
		success = SetupDiEnumDeviceInfo(hDevInfo, count++, deviceInfo);
		
		//If we just want information about the devices in devGUIDs
		if (option == 1) {
			//Figure out if device being looked at has a GUID we want to look at
			bool correctDev = false;
			for (int i = 0;i < numDevGUIDs;i++) {
				if (IsEqualCLSID(deviceInfo->ClassGuid, devGUIDs[i])) {
					correctDev = true;
					break;
				}
			}


			if (correctDev) {

				//Used at various points to see if a return is Successful
				bool goodReturn = true;

				//Get information about Power
				CM_POWER_DATA * powerData = new CM_POWER_DATA;
				powerData->PD_Size = sizeof(CM_POWER_DATA);
				goodReturn = SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_DEVICE_POWER_DATA, NULL, (PBYTE)powerData, sizeof(CM_POWER_DATA), NULL);
				//Only output anything if there is power data, this implies the devie is connected

				if (goodReturn) {
					//Print line
					printf("\n--------------------------\n");
					//Print class name
					goodReturn = SetupDiClassNameFromGuid(&deviceInfo->ClassGuid, deviceClassName, SIMAXSTRSIZE, NULL);
					if (goodReturn) printf("Device Type: %ls\n", deviceClassName);

					//Print Friendly name
					goodReturn = SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_FRIENDLYNAME, NULL, (PBYTE)deviceName, sizeof(wchar_t) * SIMAXSTRSIZE, NULL);
					if (goodReturn) printf("Friendly Device Name: %ls\n", deviceName);

					//Print Physical Device Object Name
					goodReturn = SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (PBYTE)physicalName, sizeof(wchar_t) * SIMAXSTRSIZE, NULL);
					if (goodReturn) printf("Physical Device Name: %ls\n", physicalName);

					//Print Service Name of object
					goodReturn = SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_SERVICE, NULL, (PBYTE)serviceName, sizeof(wchar_t) * SIMAXSTRSIZE, NULL);
					if (goodReturn) printf("Device Service Name: %ls\n", serviceName);

					//Print Power Data
					if (powerData->PD_MostRecentPowerState == PowerDeviceD0) {
						SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_INTENSITY);
						printf(" Device is On  ");
						
					}
					else {
						SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY);
						printf(" Device is Off  ");
						
					}

					SetConsoleTextAttribute(hConsole, whiteFG);
					printf("\n\n");

					//Load the driver list
					SetupDiBuildDriverInfoList(hDevInfo, deviceInfo, SPDIT_COMPATDRIVER);
					printf("Drivers present:\n");
					//Iterate over all the available drivers and output their description
					int driverCount = 0;
					while (SetupDiEnumDriverInfo(hDevInfo, deviceInfo, SPDIT_COMPATDRIVER, driverCount, driverInfo)) {
						printf("\tDriver Description: %ls\n", driverInfo->Description);
						printf("\t\tProvider Name: %ls\n", driverInfo->ProviderName);
						driverCount++;
					}
					if (driverCount == 0) printf("No Drivers present");
				}


				//Get Device Status- not used in this application but may be handy
				/*
				ULONG devStatus = 0;
				ULONG devProblemCode = 0;

				CM_Get_Device_ID(deviceInfo->DevInst, devId, 256, 0);
				printf("Device ID: %ls\n", devId);
				CM_Get_DevInst_Status(&devStatus, &devProblemCode, deviceInfo->DevInst, 0);
				printf("Device Status: %d\n", devStatus);
				*/
			

			}
		}

		//Only enumerate and give power info of devices
		else if (option == 2) {
			bool goodReturn = true;

			//Get information about Power
			CM_POWER_DATA * powerData = new CM_POWER_DATA;
			powerData->PD_Size = sizeof(CM_POWER_DATA);
			goodReturn = SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_DEVICE_POWER_DATA, NULL, (PBYTE)powerData, sizeof(CM_POWER_DATA), NULL);

			//If we can access power data then do we know it is a PHYSICAL device that is CONNECTED
			if (goodReturn) {
				if (powerData->PD_MostRecentPowerState == PowerDeviceD0) {
					//Device On
					SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN );
				}
				else {
					//Device Off
					SetConsoleTextAttribute(hConsole, BACKGROUND_RED );
				}

				//Highlight the types of devices we are especially interested in
				bool correctDev = false;
				for (int i = 0;i < numDevGUIDs;i++) {
					if (IsEqualCLSID(deviceInfo->ClassGuid, devGUIDs[i])) {
						correctDev = true;
						break;
					}
				}
				if (correctDev) {
					CONSOLE_SCREEN_BUFFER_INFO* currConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO;
					GetConsoleScreenBufferInfo(hConsole, currConsoleInfo);
					SetConsoleTextAttribute(hConsole, whiteFG | currConsoleInfo->wAttributes);
				}


				//Get class and device name for output
				goodReturn = SetupDiClassNameFromGuid(&deviceInfo->ClassGuid, deviceClassName, SIMAXSTRSIZE, NULL);
				goodReturn = goodReturn && SetupDiGetDeviceRegistryProperty(hDevInfo, deviceInfo, SPDRP_FRIENDLYNAME, NULL, (PBYTE)deviceName, sizeof(wchar_t) * 30, NULL);
				
				//If both calls came back with an answer
				if (goodReturn) printf("%ls - %ls ", deviceClassName, deviceName);
			}

			//Cleanup time
			SetConsoleTextAttribute(hConsole, whiteFG);

			//Sort out some problems with colours
			if (goodReturn)	printf("\n");
			
		}


	}
	//Cleanup the list
	SetupDiDestroyDeviceInfoList(hDevInfo);


}
