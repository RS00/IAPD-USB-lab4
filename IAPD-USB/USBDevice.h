#pragma once
#include <string>
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
	USBDevice(string name, string p, string sp);
	string getName();
	string getSymbPath();
	long long getTotalSpace();
	long long getFreeSpace();
	long long getBusySpace();
	~USBDevice();
};

