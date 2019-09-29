#pragma once
using namespace std;
class ScanProcesses
{

	typedef struct _Process {
		PWSTR name;
		DWORD PID;
		boolean stillAlive;
	} Process;

public:
	ScanProcesses();
	~ScanProcesses();
	static void Scan();

};

