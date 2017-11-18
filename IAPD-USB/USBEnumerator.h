#pragma once
#include "USBDevice.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <vector>
#define BUFFER_SIZE 255
using namespace std;

#pragma comment (lib, "setupapi.lib")

class USBEnumerator
{
private:
	void getVolumeAndLetter(PSP_DEVICE_INTERFACE_DETAIL_DATA_A pDeviceInterfaceDetailData, vector<USBDevice> *vectorOfDevices);
	string getNameOfVolume(string name);
public:
	USBEnumerator();
	~USBEnumerator();
	vector<USBDevice> getVectorOfDevices();
};

