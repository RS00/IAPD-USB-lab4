#include "USBDevice.h"


USBDevice::USBDevice(wstring name, wstring path, wstring symbPath)
{
	this->name = name;
	this->path = path;
	this->symbPath = symbPath;
}


USBDevice::~USBDevice()
{
}
