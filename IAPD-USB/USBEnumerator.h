#pragma once
#include "USBDevice.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <vector>
#include "WindowsPortableDevice.h"
#define BUFFER_SIZE 255
using namespace std;

#pragma comment (lib, "setupapi.lib")

class USBEnumerator
{
private:
	void addDevicesToVector(PSP_DEVICE_INTERFACE_DETAIL_DATA_A pDeviceInterfaceDetailData, vector<USBDevice> *vectorOfDevices);
	string getNameOfVolume(string name);
	void getVolumeSize(const char * name, long long int *free, long long int *total, long long int *busy);
public:
	USBEnumerator();
	~USBEnumerator();
	vector<USBDevice> getVectorOfDevices();
};

