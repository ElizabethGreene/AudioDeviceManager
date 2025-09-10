// Enable-AudioDeviceSysFX.cpp : Use the PKEY_AudioEndpoint_Disable_SysFx property to enable an audio device system effects
//

#include <iostream>
#include <string> 
#include "..\Get-AudioDevice\AudioDeviceManger.h"
#include "..\version.h"

void PrintHelp() {
	std::wcout <<
		"Enable-AudioDeviceSysFX" << std::endl <<
		VER_COPYRIGHT << " v" << VER_PRODUCT_VERSION_STR << std::endl <<
		"  Enables audio device \"audio enhancements\" " << std::endl << std::endl <<
		"Syntax:" << std::endl <<
		"  Enable-AudioDeviceSysFX <device_name> [-input | -output]" << std::endl <<
		"    Calling with no options displays this help message." << std::endl <<
		"    -input | -output : Select to enable an input or output device audio enhancements." << std::endl <<
		"    if -input or -output is not specified, the default is to enable an output device audio enhancements." << std::endl <<
		"    <device_name> : The name of the audio device audio enhancements to enable." << std::endl <<
		"  Example: Enable-AudioDeviceSysFX \"Realtek High Definition Audio)\" will enable the audio enhancements on that output device." << std::endl <<
		"  Substring matches are supported, e.g. Enable-AudioDeviceSysFX \"polycom\" -input will enable the audio enhancements on the first input device containing polycom in the name." <<
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
	std::wstring newTargetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Enabled, deviceType);
	if (newTargetDeviceName.empty())
	{
		std::wcerr << L"Failed to find a matching device." << std::endl;
		return -1;
	}

	// TODO: Add support for disabled devices in the future.

	LPCWSTR deviceId = getDeviceIdFromName(newTargetDeviceName, DeviceStatuses::Enabled, deviceType);
	if (deviceId == NULL)
	{
		std::wcerr << L"Failed to find a matching device: " << newTargetDeviceName << std::endl;
		return -1;
	}

	HRESULT hr = SetAudioDeviceSysFXStatus(deviceId, DeviceSysFXStatuses::Enabled);
	if (hr != 0) {
		std::wcerr << L"Failed to enable audio enhancements for device: " << newTargetDeviceName << std::hex << hr << std::endl;
		return -1;
	}
	std::wcout << L"Enabled audio enhancements for device: " << newTargetDeviceName << std::endl;
	return 0;
}
