#pragma once
#include <string>
using namespace std;
class USBDevice
{
private:
	wstring name;
	wstring path;
	wstring symbPath;
	long long totalSpace;
	long long freeSpace;
public:
	USBDevice(wstring p, wstring sp);
	~USBDevice();
};

