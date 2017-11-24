#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <Windows.h>
#include <Setupapi.h>
#include <winioctl.h>
#include <winioctl.h>
#include <cfgmgr32.h>

using namespace std;
class USBDevice
{
private:
	string name;
	string path;
	string symbPath;
	long long totalSpace;
	long long freeSpace;
	long long busySpace;
	static DEVINST getDrivesDevInstByDiskNumber(long DiskNumber, UINT DriveType);
public:
	USBDevice(string name, string p, string sp, long long total, long long free, long long busy);
	string getName();
	string getSymbPath();
	double getTotalSpace();
	double getFreeSpace();
	double getBusySpace();
	static bool ejectDrive(CHAR symbPath);
	~USBDevice();
};

