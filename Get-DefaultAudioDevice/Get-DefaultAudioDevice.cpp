// Get-DefaultAudioDevice 
// Displays the Default audio output device.
//  -h : Displays this help
//
//  -sounds | -communications : Select the "role" to query
//    sounds : Games, system notifications, Music, movies, narration, and live music recording.
//    communications : Voice communications (talking to another person).
//
//  -Input | -Output
//    Displays the Input or Output device respectively.
// 
// Running the executable with no options displays the default sound output device.


#include <iostream>
#include <string>
#include <vector>
#include "..\Get-AudioDevice\AudioDeviceManger.h"
#include "mmdeviceapi.h"
#include "..\version.h"

void PrintHelp() {
	std::wcout <<
		L" Get-DefaultAudioDevice" << std::endl <<
		VER_COPYRIGHT << " v" << VER_PRODUCT_VERSION_STR << std::endl << 
		L"  Displays the Default audio output device." << std::endl <<
		L"   -h : Displays this help" << std::endl <<
		L"   -sounds | -communications : Select the \"role\" to query" << std::endl <<
		L"     sounds : Games, system notifications, Music, movies, narration, and live music recording." << std::endl <<
		L"     communications : Voice communications (talking to another person)." << std::endl <<
		L"   -Input | -Output" << std::endl <<
		L"     Displays the Input or Output device respectively." << std::endl <<
		L"  Running the executable with no options displays the default Multimedia output device." << std::endl << std::endl;
	return;
};

int wmain(int argc, wchar_t* argv[])
{

	ERole erole = ERoleFromCommandLine(argc, argv);
	Actions action = ActionFromCommandLine(argc, argv);
	DeviceTypes deviceType = DeviceTypeFromCommandLine(argc, argv);

	if (action == Actions::SHOWHELP) {
		PrintHelp();
		return 0;
	}

	std::vector <std::wstring> deviceNames;

	HRESULT hr = getDefaultAudioDeviceName(&deviceNames, deviceType, erole);

	if (FAILED(hr))
	{
		std::wcerr << L"Failed to get default audio device information: " << std::hex << hr << std::endl;
		return -1;
	}

	for (const auto& deviceName : deviceNames)
	{
		std::wcout << deviceName << std::endl;
	}
	return 0;
}
