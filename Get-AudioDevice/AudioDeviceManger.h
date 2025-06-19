#pragma once

#include <iostream>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <comdef.h>
#include <string>
#include <vector>
#include <propkey.h> 
#include <Functiondiscoverykeys_devpkey.h>
#include "PolicyConfig.h"
#include <algorithm>
#include <cwctype>

// TODO: Move command line parsing options to their own .h file. 

enum class DeviceTypes {
	Input,
	Output
};

enum class DeviceStatuses {
	Enabled,
	Disabled
};

enum class Actions {
	SHOWDATA,
	SHOWHELP
};

Actions ActionFromCommandLine(int argc, wchar_t* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (_wcsicmp(argv[i], L"-h") == 0) {
				return Actions::SHOWHELP;
			}
		}
	}
	return Actions::SHOWDATA;
};


DeviceTypes DeviceTypeFromCommandLine(int argc, wchar_t* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (_wcsicmp(argv[i], L"-input") == 0) {
				return DeviceTypes::Input;
			}
		}
	}
	return DeviceTypes::Output;
};

// mmdeviceapi.h is included for this data type.  
ERole ERoleFromCommandLine(int argc, wchar_t* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (_wcsicmp(argv[i], L"-communications") == 0) {
				return ERole::eCommunications;
			}
		}
	}
	return ERole::eConsole;
};

size_t findCaseInsensitive(std::wstring str, std::wstring substr) {
	// I've written this function 5,000 times. For god's green gravy goats, why isn't it in the standard library?
	auto it = std::search(
		str.begin(), str.end(),
		substr.begin(), substr.end(),
		[](wchar_t c1, wchar_t c2) {
			return std::tolower(c1) == std::tolower(c2);
		}
	);
	return (it == str.end()) ? std::wstring::npos : (it - str.begin());
}

HRESULT getDeviceNames(std::vector <std::wstring>* deviceNames, DeviceStatuses deviceStatus, DeviceTypes deviceType) {
	DWORD dwStateMask = DEVICE_STATE_ACTIVE; // Defined in mmdeviceapi.h
	if (deviceStatus != DeviceStatuses::Enabled) {
		dwStateMask = DEVICE_STATE_DISABLED;
	}

	EDataFlow dataFlow = eRender; // Output Devices - Defined in mmdeviceapi.h 
	if (deviceType == DeviceTypes::Input) {
		dataFlow = eCapture; // Input Devices - Defined in mmdeviceapi.h
	}

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM subsystem: " << std::hex << hr << std::endl;
		return hr;
	}
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	IMMDevice* device = NULL;
	IPropertyStore* propertyStore = NULL;
//	PROPVARIANT propVar;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return hr;
	}
	hr = deviceEnumerator->EnumAudioEndpoints(dataFlow, dwStateMask, &deviceCollection);
	if (FAILED(hr))
	{
		std::cerr << "Failed to enumerate audio endpoints: " << std::hex << hr << std::endl;
		deviceEnumerator->Release();
		return hr;
	}
	UINT count;
	hr = deviceCollection->GetCount(&count);
	if (FAILED(hr))
	{
		std::cerr << "Failed to get device count: " << std::hex << hr << std::endl;
		deviceCollection->Release();
		deviceEnumerator->Release();
		return hr;
	}

	for (UINT i = 0; i < count; ++i)
	{
		hr = deviceCollection->Item(i, &device);
		if (FAILED(hr))
		{
			deviceNames->push_back(L"Failed to get device: " + std::to_wstring(i));
			continue;
		}

		// Get the device ID
		LPOLESTR deviceId = NULL;
		hr = device->GetId(&deviceId);
		if (FAILED(hr))
		{
			deviceNames->push_back(L"Failed to get device ID: " + std::to_wstring(i));
			device->Release();
			continue;
		}

		IPropertyStore* propertyStore = NULL;
		hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr))
		{
			deviceNames->push_back(deviceId);
			device->Release();
			continue;
		}

		PROPVARIANT propVar;
		PropVariantInit(&propVar);
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar);
		if (FAILED(hr))
		{
			deviceNames->push_back(deviceId);
			PropVariantClear(&propVar);
			propertyStore->Release();
			device->Release();
			continue;
		}
		if (propVar.vt == VT_LPWSTR)
			deviceNames->push_back(propVar.pwszVal);

		PropVariantClear(&propVar);
		propertyStore->Release();
		device->Release();
	}
	deviceCollection->Release();
	deviceEnumerator->Release();
	return S_OK;
}

HRESULT getDefaultAudioDeviceName(std::vector <std::wstring>* deviceNames, DeviceTypes deviceType, ERole eRole) {
	DWORD dwStateMask = DEVICE_STATEMASK_ALL; // Defined in mmdeviceapi.h

	EDataFlow dataFlow = eRender; // Output Devices - Defined in mmdeviceapi.h 
	if (deviceType == DeviceTypes::Input) {
		dataFlow = eCapture; // Input Devices - Defined in mmdeviceapi.h
	}

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM subsystem: " << std::hex << hr << std::endl;
		return hr;
	}
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	
	IMMDevice* device = NULL;
	IPropertyStore* propertyStore = NULL;
	//	PROPVARIANT propVar;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return hr;
	}
	/*hr = deviceEnumerator->EnumAudioEndpoints(dataFlow, dwStateMask, &deviceCollection);
	if (FAILED(hr))
	{
		std::cerr << "Failed to enumerate audio endpoints: " << std::hex << hr << std::endl;
		deviceEnumerator->Release();
		return hr;
	}
	UINT count;*/
	//hr = deviceCollection->GetCount(&count);
	/*if (FAILED(hr))
	{
		std::cerr << "Failed to get device count: " << std::hex << hr << std::endl;
		deviceCollection->Release();
		deviceEnumerator->Release();
		return hr;
	}*/

	// Get the default audio endpoint
		hr = deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, eRole, &device);
		if (FAILED(hr))
		{
			deviceNames->push_back(L"Failed to get device");
			return hr;
		}

		// Get the device ID
		LPOLESTR deviceId = NULL;
		hr = device->GetId(&deviceId);
		if (FAILED(hr))
		{
			deviceNames->push_back(L"Failed to get device ID" );
			device->Release();
			return hr;
		}

		hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr))
		{
			deviceNames->push_back(L"Failed to get device Name");
			device->Release();
			return hr;
		}

		PROPVARIANT propVar;
		PropVariantInit(&propVar);
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar);
		if (FAILED(hr))
		{
			deviceNames->push_back(deviceId);
			PropVariantClear(&propVar);
			propertyStore->Release();
			device->Release();
			return hr;
		}
		if (propVar.vt == VT_LPWSTR)
			deviceNames->push_back(propVar.pwszVal);

		PropVariantClear(&propVar);
		propertyStore->Release();
		device->Release();
	
	deviceEnumerator->Release();
	return S_OK;
}



HRESULT SetAudioDeviceStatus(LPCWSTR deviceId, DeviceStatuses deviceStatus) {
	IPolicyConfig* pPolicyConfig;
	int visibility = deviceStatus == DeviceStatuses::Enabled ? TRUE : FALSE;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC_SERVER, __uuidof(IPolicyConfig), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetEndpointVisibility(deviceId, visibility);
		pPolicyConfig->Release();
	}
	return hr;
}

std::wstring DeviceNameFromCommandLineOptions(int argc, wchar_t* argv[], std::vector<std::wstring> validOptions)
{
	// Loop through each of the command line options and compare them to the "validOptions" vector.
	// If the option is not in the vector, return it as the device name.
	// If there are no matches at the end, return an empty string.
	for (int i = 1; i < argc; ++i) {
		std::wstring str = argv[i];

		// convert str to lower case.
		// The error occurs because std::tolower expects a char, not a wchar_t, and the overload for wchar_t requires a locale.
		// For std::wstring, use a lambda to call std::towlower for each wchar_t, like this:

		std::transform(str.begin(), str.end(), str.begin(),
			[](wchar_t c) { return std::towlower(c); });
		// Check if the option is in the validOptions vector
		if (std::find(validOptions.begin(), validOptions.end(), str) == validOptions.end()) {
			// If the option is not in the vector, return it
			return argv[i];

		}
	}
	// If no matches, return empty string
	return L"";
}


// Get the Device ID based on the device name
std::wstring getDeviceNameFromPartialName (std::wstring targetDeviceName, DeviceStatuses deviceStatus, DeviceTypes deviceType) {
	
	std::vector <std::wstring>deviceNames;
	HRESULT hr = getDeviceNames(&deviceNames, deviceStatus, deviceType);

	// Loop through the device names to match the device name.

	for (const auto& deviceName : deviceNames)
	{
		if (findCaseInsensitive(deviceName, targetDeviceName) != std::wstring::npos) {
			return deviceName;			
		}
	}

	// If no matches, return empty string
	return L"";
}

// Get the Device ID based on the device name
LPCWSTR getDeviceIdFromName(std::wstring targetDeviceName, DeviceStatuses deviceStatus, DeviceTypes deviceType) {

	DWORD dwStateMask = DEVICE_STATE_ACTIVE; // Defined in mmdeviceapi.h
	if (deviceStatus != DeviceStatuses::Enabled) {
		dwStateMask = DEVICE_STATE_DISABLED;
	}

	EDataFlow dataFlow = eRender; // Output Devices - Defined in mmdeviceapi.h 
	if (deviceType == DeviceTypes::Input) {
		dataFlow = eCapture; // Input Devices - Defined in mmdeviceapi.h
	}

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize COM subsystem: " << std::hex << hr << std::endl;
		return NULL;
	}
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	IMMDevice* device = NULL;
	IPropertyStore* propertyStore = NULL;
//	PROPVARIANT propVar;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return NULL;
	}
	hr = deviceEnumerator->EnumAudioEndpoints(dataFlow, dwStateMask, &deviceCollection);
	if (FAILED(hr))
	{
		std::cerr << "Failed to enumerate audio endpoints: " << std::hex << hr << std::endl;
		deviceEnumerator->Release();
		return NULL;
	}
	UINT count;
	hr = deviceCollection->GetCount(&count);
	if (FAILED(hr))
	{
		std::cerr << "Failed to get device count: " << std::hex << hr << std::endl;
		deviceCollection->Release();
		deviceEnumerator->Release();
		return NULL;
	}

	for (UINT i = 0; i < count; ++i)
	{
		hr = deviceCollection->Item(i, &device);
		if (FAILED(hr))
		{
			//deviceNames->push_back(L"Failed to get device: " + std::to_wstring(i));
			continue;
		}

		// Get the device ID
		LPOLESTR deviceId = NULL;
		hr = device->GetId(&deviceId);
		if (FAILED(hr))
		{
			//deviceNames->push_back(L"Failed to get device ID: " + std::to_wstring(i));
			device->Release();
			continue;
		}

		IPropertyStore* propertyStore = NULL;
		hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr))
		{
			//deviceNames->push_back(deviceId);
			device->Release();
			continue;
		}

		PROPVARIANT propVar;
		PropVariantInit(&propVar);
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar);
		if (FAILED(hr))
		{
			//deviceNames->push_back(deviceId);
			PropVariantClear(&propVar);
			propertyStore->Release();
			device->Release();
			continue;
		}
		if (propVar.vt == VT_LPWSTR) {
			if (std::wstring(propVar.pwszVal) == targetDeviceName) {
				PropVariantClear(&propVar);
				propertyStore->Release();
				device->Release(); 
				return deviceId;
				deviceCollection->Release();
				deviceEnumerator->Release();
			}
		}

		PropVariantClear(&propVar);
		propertyStore->Release();
		device->Release();
	}
	deviceCollection->Release();
	deviceEnumerator->Release();
	
	// If no matches, return NULL
	return NULL;
}

HRESULT SetDefaultAudioDevice(LPCWSTR devID,ERole eRole)
{
	IPolicyConfigVista* pPolicyConfig;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, eRole);
		pPolicyConfig->Release();
	}
	return hr;
}


//HRESULT SetDefaultAudioDeviceByName(const std::wstring& deviceName)
//{
//	CoInitialize(NULL);
//	IMMDeviceEnumerator* deviceEnumerator = NULL;
//	IMMDeviceCollection* deviceCollection = NULL;
//	IMMDevice* device = NULL;
//	IPropertyStore* propertyStore = NULL;
//	PROPVARIANT propVar;
//	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
//	if (FAILED(hr))
//	{
//		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
//		return hr;
//	}
//	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
//	if (FAILED(hr))
//	{
//		std::cerr << "Failed to enumerate audio endpoints: " << std::hex << hr << std::endl;
//		deviceEnumerator->Release();
//		return hr;
//	}
//	UINT count;
//	hr = deviceCollection->GetCount(&count);
//	if (FAILED(hr))
//	{
//		std::cerr << "Failed to get device count: " << std::hex << hr << std::endl;
//		deviceCollection->Release();
//		deviceEnumerator->Release();
//		return hr;
//	}
//	for (UINT i = 0; i < count; ++i)
//	{
//		hr = deviceCollection->Item(i, &device);
//		if (FAILED(hr))
//			continue;
//		device->OpenPropertyStore(STGM_READ, &propertyStore);
//		propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar);
//		
//		
//		if (deviceName == propVar.pwszVal)
//		{
//			
//			// Get the device ID
//			LPOLESTR deviceId = NULL;
//			hr = device->GetId(&deviceId);
//			if (FAILED(hr))
//			{
//				std::cerr << "Failed to get device ID: " << std::hex << hr << std::endl;
//				device->Release();
//				return hr;
//			}
//
//			SetDefaultAudioPlaybackDevice(deviceId);
//
//			break;
//		}
//		device->Release();
//		propertyStore->Release();
//	}
//	deviceCollection->Release();
//	deviceEnumerator->Release();
//	return S_OK;
//}
