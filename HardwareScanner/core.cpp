#include "pch.h"
#include "ScanInternal.h"
#include "ScanProcesses.h"




int main() {
	SetConsoleTitle(L"Hardware Scanner - Julian W");

	WORD whiteFG = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, whiteFG);


	//Get System power Status
	SYSTEM_POWER_STATUS * sysPowerStatus = new SYSTEM_POWER_STATUS;
	SetConsoleTextAttribute(hConsole, BACKGROUND_RED | whiteFG);
	if (GetSystemPowerStatus(sysPowerStatus)) {
		if (sysPowerStatus->SystemStatusFlag) {
			printf("Battery Saver On\n");
		}
		else {
			printf("Battery Saver Off\n");
		}
		printf("Current Charge: %d%% \n\n", sysPowerStatus->BatteryLifePercent);
	}

	SetConsoleTextAttribute(hConsole, whiteFG);

	boolean running = true;
	while (running) {
		int option;
		printf("Option 1: Camera and Microphone Details \nOption 2: Brief enumeration of all devices\nOption 3: Scan Processes\nOption 4: Exit Program\n");
		printf("Choice: ");
		scanf_s("%d", &option);

		switch (option) {
		case 1:
		case 2:
			ScanInternal::Scan(option);
			break;
		case 3:
			ScanProcesses::Scan();
			break;
		case 4:
			running = false;
			printf("EXITING PROGRAM\n");
			break;
		default:
			printf("That was not a valid input\n");
			break;
		}
		SetConsoleTextAttribute(hConsole, whiteFG);
		printf("\n--------------------------");
		printf("\n--------------------------\n\n");
	}


	system("pause");
	return 0;
}