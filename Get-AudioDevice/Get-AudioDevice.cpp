// Get-AudioDevice 
// This application returns a list of audio devices on the system.

// Command line options
// No Arguments: List connected output devices

#include <iostream>
#include <string>
#include <vector>
#include "AudioDeviceManger.h"

int main()
{
	std::vector <std::wstring> deviceNames;

	HRESULT hr = getDeviceNames(&deviceNames);

	if (FAILED(hr))
	{
		std::cerr << "Failed to get device names: " << std::hex << hr << std::endl;
		return -1;
	}

	for (const auto& deviceName : deviceNames)
	{
		std::wcout << deviceName << std::endl;
	}
	return 0;
}
