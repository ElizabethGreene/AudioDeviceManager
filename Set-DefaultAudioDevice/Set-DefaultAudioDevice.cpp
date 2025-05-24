// Set-DefaultAudioDevice.cpp
// This application takes a single argument, the name of the audio device to set as default.
// If no audio device is specified, it displays a help message.

#include <iostream>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>
#include <algorithm>
#include "..\Get-AudioDevice\AudioDeviceManger.h"

void PrintHelp() {
	std::wcout <<
		L" Set-DefaultAudioDevice [-h] [-input|-output] [-sound|-communications] <device_name>" << std::endl <<
		L"  Sets the Default sound or communications device." << std::endl <<
		std::endl <<
		L" Syntax:" << std::endl <<
		L"  -h (or with no options) : Displays this help" << std::endl <<
		L"  -sounds | -communications : Select the \"role\" to set" << std::endl <<
		L"    -sounds : Games, system notifications, Music, movies, narration, and live music recording." << std::endl <<
		L"    -communications : Voice communications (talking to another person)." << std::endl <<
		L"  -Input | -Output" << std::endl <<
		L"    Sets the Input or Output device respectively." << std::endl <<
		L"  <device name> : The name of the audio device to set as default." << std::endl <<
		L"    Substring matches are supported, e.g. \"polycom\" will match to the first device with polycom in the name." << std::endl <<
		std::endl <<
		L"  Running the executable with just a device name sets the default sound output device." << std::endl <<
		L"    Example: Set-DefaultAudioDevice \"Realtek\"" << std::endl <<
		std::endl << std::endl;
	return;
}


int wmain(int argc, wchar_t* argv[]) // Add parameters to main function
{
	// TODO: I don't love how I'm handling command line options across this entire suite. 

	// Define the valid command line options
	std::vector <std::wstring> validOptions = { L"-h", L"-input", L"-output", L"-sounds", L"-communications" };

	// Get the name of the audio device from the command line arguments
	std::wstring targetDeviceName = DeviceNameFromCommandLineOptions(argc, argv, validOptions);

	// Get the device type from the command line arguments
	DeviceTypes deviceType = DeviceTypeFromCommandLine(argc, argv);

	// Get the role from the command line arguments
	ERole eRole = ERoleFromCommandLine(argc, argv);

	// Display help message if -h is specified 
	if (ActionFromCommandLine(argc, argv) == Actions::SHOWHELP)
	{
		PrintHelp();
		return 0;
	}

	// Display help message if no device name is found.
	if (targetDeviceName.empty())
	{
		std::wcout << L"No device name found" << std::endl;
		PrintHelp();
		return 0;
	}

	// There is a non-intuitive behavior here.  The setDefault function doesn't take input or output as an argument. 
	// Instead, the input or output comes from the device id discovered by the getDeviceName/ID functions.

	// Get the full Device name based on the name from the command line
	targetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Enabled, deviceType);
	if (targetDeviceName.empty())
	{
		std::wcout << L"Failed to find a matching device." << std::endl;
		return -1;
	}

	// Get the device ID based on the device name
	LPCWSTR deviceId = getDeviceIdFromName(targetDeviceName, DeviceStatuses::Enabled, deviceType);
	if (deviceId == NULL)
	{
		std::wcout << L"Failed to find a matching device: " << targetDeviceName << std::endl;
		return -1;
	}

	// Set the default audio device using the device ID
	HRESULT hr = SetDefaultAudioDevice(deviceId, eRole);
	if (FAILED(hr))
	{
		std::cerr << "Failed to set default device: " << std::hex << hr << std::endl;
		return -1;
	}
	std::wcout << L"Default audio device set to: " << targetDeviceName << std::endl;
}

