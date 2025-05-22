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

   // Get the Device ID based on the device name
   targetDeviceName = getDeviceNameFromPartialName(targetDeviceName, DeviceStatuses::Enabled, DeviceTypes::Output);
   if (targetDeviceName.empty())
   {
	   std::wcerr << L"Failed to find a matching device." << std::endl;
	   return -1;
   }
   LPCWSTR deviceId = getDeviceIdFromName(targetDeviceName,DeviceStatuses::Enabled,DeviceTypes::Output);
   if (deviceId == NULL)
   {
	   std::wcerr << L"Failed to find a matching device: " << targetDeviceName << std::endl;
	   return -1;
   }

   // Set the default audio device using the device ID
   HRESULT hr = SetDefaultAudioPlaybackDevice(deviceId);
   if (FAILED(hr))
   {
	   std::cerr << "Failed to set default audio output device: " << std::hex << hr << std::endl;
	   return -1;
   }
   std::wcout << L"Default audio output device set to: " << targetDeviceName << std::endl;
}


