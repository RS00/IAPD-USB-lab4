#include "USBDevice.h"


USBDevice::USBDevice(string name, string path, string symbPath, long long total, long long free, long long busy)
{
	this->name = name;
	this->path = path;
	this->symbPath = symbPath;
	this->totalSpace = total;
	this->freeSpace = free;
	this->busySpace = busy;
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

double USBDevice::getTotalSpace()
{
	return (double)this->totalSpace / 1000000;
}

double USBDevice::getFreeSpace()
{
	return (double)this->freeSpace / 1000000;
}

double USBDevice::getBusySpace()
{
	return (double)this->busySpace / 1000000;
}