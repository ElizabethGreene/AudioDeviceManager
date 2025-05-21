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



size_t findCaseInsensitive(std::wstring str, std::wstring substr) {
	// I've written this function 5,000 times. For god's green gravy goats, why isn't it in the standard library?
	auto it = std::search(
		str.begin(), str.end(),
		substr.begin(), substr.end(),
		[](char c1, char c2) {
			return std::tolower(c1) == std::tolower(c2);
		}
	);
	return (it == str.end()) ? std::wstring::npos : (it - str.begin());
}

int wmain(int argc, wchar_t* argv[]) // Add parameters to main function
{
   // If no audio device is specified, display a help message
   if (argc < 2)
   {
       std::cout << "Usage: Set-DefaultAudioDevice <device_name>" << std::endl;
       std::cout << "Example: Set-DefaultAudioDevice \"Speakers (Realtek High Definition Audio)\"" << std::endl;
       std::cout << "Substring matches are supported, e.g. Set-DefaultAudioDevice \"polycom\" will set the default to the first device containing polycom in the name.";
       return 1;
   }

   // Get the name of the audio device from the command line arguments
   std::wstring targetDeviceName = argv[1];

   std::vector <std::wstring> deviceNames;

   HRESULT hr = getDeviceNames(&deviceNames);

   if (FAILED(hr))
   {
	   std::cerr << "Failed to get device names: " << std::hex << hr << std::endl;
	   return -1;
   }

   // Loop through the device names to match the device name.

   for (const auto& deviceName : deviceNames)
   {
	   if (findCaseInsensitive(deviceName, targetDeviceName) != std::wstring::npos) {

		   HRESULT hr = SetDefaultAudioDeviceByName(deviceName);
		   if (FAILED(hr))
		   {
			   std::cerr << "Failed to set default audio device: " << std::hex << hr << std::endl;
			   return 1;
		   }
		   std::wcout << L"Default audio device set to: " << deviceName << std::endl;
		   return 0;
	   }
   }

   std::cerr << "No match for device name" << std::endl;
   return -1;
}


