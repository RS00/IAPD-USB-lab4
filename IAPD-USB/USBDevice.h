#pragma once
#include <string>
#include <Windows.h>

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

