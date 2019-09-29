#include "pch.h"
#include "ScanMic.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

ScanMic::ScanMic()
{
}


ScanMic::~ScanMic()
{
}

void ScanMic::Scan(void) 
{
	CoInitialize(nullptr);
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *enumerator = NULL;
	IMMDeviceCollection *collection = NULL;
	IMMDevice *endpoint = NULL;
	IPropertyStore *properties = NULL;

	UINT deviceCount;
	LPWSTR pwszID = NULL;

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&enumerator);

	hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);
	hr = collection->GetCount(&deviceCount);

	hr = collection->Item(0, &endpoint);

	//hr = endpoint->OpenPropertyStore(STGM_READ, &properties);
	endpoint->GetId(&pwszID);
	
}



