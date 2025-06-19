// Enable-AudioDevice.cpp
//
#include <iostream>
#include <string> 
#include "..\Get-AudioDevice\AudioDeviceManger.h"
#include "..\version.h"

void PrintHelp() {
	std::wcout <<
		"Enable-AudioDevice" << std::endl <<
		VER_COPYRIGHT << " v" << VER_PRODUCT_VERSION_STR << std::endl <<
		"  Enables an audio device." << std::endl << std::endl <<
		"Syntax:" << std::endl <<
		"  Enable-AudioDevice <device_name> [-input | -output]" << std::endl <<
		"    Calling with no options displays this help message." << std::endl <<
		"    -input | -output : Select to enable an input or output device." << std::endl <<
		"    if -input or -output is not specified, the default is to enable an output device." << std::endl <<
		"    <device_name> : The name of the audio device to enable." << std::endl <<
		"  Example: Enable-AudioDevice \"Realtek High Definition Audio)\" will enable that output device." << std::endl <<
		"  Substring matches are supported, e.g. Enable-AudioDevice \"polycom\" -input will enable the first input device containing polycom in the name." <<
		std::endl << std::endl;
	return;
}


int wmain(int argc, wchar_t* argv[])
{
	// Define the valid command line options
	std::vector <std::wstring> validOptions = { L"-input", L"-output", L"-h" };

	// Get the name of the audio device from the command line arguments
	std::wstring targetDeviceName = DeviceNameFromCommandLineOptions(argc, argv, validOptions);

	// Get the device type from the command line arguments
	DeviceTypes deviceType = DeviceTypeFromCommandLine(argc, argv);

	// Display help message if -h is specified 
	if (ActionFromCommandLine(argc, argv) == Actions::SHOWHELP)
	{
		PrintHelp();
		return 0;
	}

	// Display help message if no device name is found.
	if (targetDeviceName.empty())
	{
		std::wcerr << L"No device name found" << std::endl;
		PrintHelp();
		return -1;
	}

	// Get the Device ID based on the device name
	std::wstring newTargetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Disabled, deviceType);
	if (newTargetDeviceName.empty())
	{
		// Is the device already enabled?
		newTargetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Enabled, deviceType);
		if (newTargetDeviceName.empty())
		{
			std::wcerr << L"Failed to find a matching device." << std::endl;
			return -1;
		}
		else {
			std::wcout << L"Device is already enabled: " << newTargetDeviceName << std::endl;
			return 0;
		}
	}

	LPCWSTR deviceId = getDeviceIdFromName(newTargetDeviceName, DeviceStatuses::Disabled, deviceType);
	if (deviceId == NULL)
	{
		std::wcerr << L"Failed to find a matching device: " << newTargetDeviceName << std::endl;
		return -1;
	}

	HRESULT hr = SetAudioDeviceStatus(deviceId, DeviceStatuses::Enabled);
	if (hr != 0) {
		std::wcerr << L"Failed to enable audio device: " << newTargetDeviceName << std::hex << hr << std::endl;
		return -1;
	}
	std::wcout << L"Audio device enabled: " << newTargetDeviceName << std::endl;
	return 0;
}
