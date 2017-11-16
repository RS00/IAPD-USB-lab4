#include "USBDevice.h"


USBDevice::USBDevice(wstring path, wstring symbPath)
{
	this->path = path;
	this->symbPath = symbPath;
}


USBDevice::~USBDevice()
{
}
