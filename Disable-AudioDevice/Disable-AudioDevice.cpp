// Disable-AudioDevice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string> 
#include "..\Get-AudioDevice\AudioDeviceManger.h"

int wmain(int argc, wchar_t* argv[])
{
	// If no command line arguments are provided, display a help message
	if (argc < 2)
	{
		std::cout << "Usage: Disable-AudioDevice <device_name>" << std::endl;
		std::cout << "Example: Disable-AudioDevice \"Speakers (Realtek High Definition Audio)\"" << std::endl;
		std::cout << "Substring matches are supported, e.g. Disable-AudioDevice \"polycom\" will disable the first device containing polycom in the name.";
		return 1;
	}

	// Get the name of the audio device from the command line arguments
	std::wstring targetDeviceName = argv[1];

	// Get the Device ID based on the device name
	std::wstring newTargetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Enabled, DeviceTypes::Output);
	if (newTargetDeviceName.empty())
	{
		// Is the device already disabled?
		newTargetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Disabled, DeviceTypes::Output);
		if (newTargetDeviceName.empty())
		{
			std::wcerr << L"Failed to find a matching device." << std::endl;
			return -1;
		}
		else {
			std::wcerr << L"Device is already Disabled: " << newTargetDeviceName << std::endl;
			return 0;
		}
	}

	LPCWSTR deviceId = getDeviceIdFromName(newTargetDeviceName, DeviceStatuses::Enabled, DeviceTypes::Output);
	if (deviceId == NULL)
	{
		std::wcerr << L"Failed to find a matching device: " << newTargetDeviceName << std::endl;
		return -1;
	}

	HRESULT hr = SetAudioDeviceStatus(deviceId, DeviceStatuses::Disabled);
	if (hr != 0) {
		std::wcerr << L"Failed to Disable audio output device: " << newTargetDeviceName << std::hex << hr << std::endl;
		return -1;
	}
	std::wcout << L"Audio output device Disabled: " << newTargetDeviceName << std::endl;
	return 0;
}
