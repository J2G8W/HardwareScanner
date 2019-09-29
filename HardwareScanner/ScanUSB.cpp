#include "pch.h"
#include <iostream>
#include "scanUSB.h"


ScanUSB::ScanUSB()
{
}


ScanUSB::~ScanUSB()
{
}


void ScanUSB::Scan()
{
	CoInitialize(nullptr);

	//Taken from Microsoft Website to create the Device Manager
	DWORD numDevices = 0;
	Microsoft::WRL::ComPtr<IPortableDeviceManager>  pPortableDeviceManager;
	Microsoft::WRL::ComPtr<IPortableDevice> device;


	IPortableDeviceValues* pClientInformation = NULL;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IPortableDeviceValues,
		(VOID**)&pClientInformation);


	hr = CoCreateInstance(CLSID_PortableDeviceManager,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pPortableDeviceManager));

	if (FAILED(hr)) {
		printf("! Failed to CoCreateInstance CLSID_PortableDeviceManager, hr = 0x%lx\n", hr);
		return;
	}
	//Figure out how many devices there are
	hr = pPortableDeviceManager->GetDevices(NULL, &numDevices);
	printf("\n%d Windows Portable Device(s) found on the system\n\n", numDevices);

	if (numDevices == 0) {
		return;
	}

	//Have an array large enough for the Devices
	PWSTR* deviceIDs = new(std::nothrow) PWSTR[numDevices];
	if (deviceIDs != NULL) {
		//Get devices then go through each one
		hr = pPortableDeviceManager->GetDevices(deviceIDs, &numDevices);
		for (unsigned int i = 0;i < numDevices; i++) {

			
			//Get the size needed to return then return the name of the device
			DWORD nameSize = 0;
			pPortableDeviceManager->GetDeviceFriendlyName(deviceIDs[i], NULL, &nameSize);
			WCHAR * deviceName = (WCHAR*)malloc(nameSize * sizeof(WCHAR));
			pPortableDeviceManager->GetDeviceFriendlyName(deviceIDs[i], deviceName, &nameSize);
			printf("Friendly name: %ls\n", deviceName);

			//Open a device
			hr = CoCreateInstance(CLSID_PortableDeviceFTM,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&device));


			if (SUCCEEDED(hr)) {
				hr = device->Open(deviceIDs[i], pClientInformation);

			}
			free(deviceName);

		}

	}
	


}