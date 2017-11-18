#include "USBDevice.h"


USBDevice::USBDevice(string name, string path, string symbPath)
{
	this->name = name;
	this->path = path;
	this->symbPath = symbPath;
}


USBDevice::~USBDevice()
{
}

string USBDevice::getName()
{
	return this->name;
}

string USBDevice::getSymbPath()
{
	return this->symbPath;
}

long long USBDevice::getTotalSpace()
{
	return this->totalSpace;
}

long long USBDevice::getFreeSpace()
{
	return this->freeSpace;
}

long long USBDevice::getBusySpace()
{
	return this->busySpace;
}