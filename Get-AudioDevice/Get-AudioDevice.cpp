// Get-AudioDevice 
// This application returns a list of audio devices on the system.

// Command line options
// -h: Display Help Message
// -input or -output displays input or output devices respectively. 
// -disabled or -enabled displays disabled or enabled devices respectively.
// Running the executable with no options will display enabled output devices.

#include <iostream>
#include <string>
#include <vector>
#include "AudioDeviceManger.h"


void PrintHelp() {
	std::wcout << L"Get-AudioDevice.exe" << std::endl;
	std::wcout << L"  -h: Display Help Message" << std::endl;
	std::wcout << L"  -input or -output displays input or output devices respectively. " << std::endl;
	std::wcout << L"  -disabled or -enabled displays disabled or enabled devices respectively." << std::endl;
	std::wcout << L"  Running the executable with no options will display enabled output devices." << std::endl;
	return;
};

DeviceStatuses DeviceStatusFromCommandLine(int argc, wchar_t* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (_wcsicmp(argv[i], L"-disabled") == 0) {
				return DeviceStatuses::Disabled;
			}
		}
	}
	return DeviceStatuses::Enabled;
};


int wmain(int argc, wchar_t* argv[])
{
	
	DeviceStatuses devicestatus = DeviceStatusFromCommandLine(argc, argv);
	Actions action = ActionFromCommandLine(argc, argv);
	DeviceTypes deviceType = DeviceTypeFromCommandLine(argc, argv);
	
	if (action == Actions::SHOWHELP) {
		PrintHelp();
		return 0;
	}
	
	std::vector <std::wstring> deviceNames;

	HRESULT hr = getDeviceNames(&deviceNames,devicestatus,deviceType);

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
