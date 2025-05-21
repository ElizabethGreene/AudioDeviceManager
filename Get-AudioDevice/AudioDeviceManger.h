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



HRESULT getDeviceNames(std::vector <std::wstring>* deviceNames)
{
	CoInitialize(NULL);
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	IMMDevice* device = NULL;
	IPropertyStore* propertyStore = NULL;
	PROPVARIANT propVar;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return hr;
	}
	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
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



HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
	IPolicyConfigVista* pPolicyConfig;
	ERole reserved = eConsole;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}
	return hr;
}


HRESULT SetDefaultAudioDeviceByName(const std::wstring& deviceName)
{
	CoInitialize(NULL);
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	IMMDevice* device = NULL;
	IPropertyStore* propertyStore = NULL;
	PROPVARIANT propVar;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return hr;
	}
	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
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
			continue;
		device->OpenPropertyStore(STGM_READ, &propertyStore);
		propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar);
		
		
		if (deviceName == propVar.pwszVal)
		{
			
			// Get the device ID
			LPOLESTR deviceId = NULL;
			hr = device->GetId(&deviceId);
			if (FAILED(hr))
			{
				std::cerr << "Failed to get device ID: " << std::hex << hr << std::endl;
				device->Release();
				return hr;
			}

			SetDefaultAudioPlaybackDevice(deviceId);

			break;
		}
		device->Release();
		propertyStore->Release();
	}
	deviceCollection->Release();
	deviceEnumerator->Release();
	return S_OK;
}
