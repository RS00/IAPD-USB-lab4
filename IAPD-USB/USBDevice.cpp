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

bool USBDevice::ejectDrive(CHAR symbPath)
{
	CHAR devicepath[16];
	strcpy_s(devicepath, "\\\\.\\?:");
	devicepath[4] = symbPath;

	DWORD dwRet = 0;
	HANDLE hVol = CreateFileA(devicepath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hVol == INVALID_HANDLE_VALUE)
		return false;

	if (!DeviceIoControl(hVol, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &dwRet, 0))
		return false;

	if (!DeviceIoControl(hVol, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &dwRet, 0))
		return false;

	DWORD dwBytesReturned;
	PREVENT_MEDIA_REMOVAL PMRBuffer;

	PMRBuffer.PreventMediaRemoval = true;
	DeviceIoControl(hVol, IOCTL_STORAGE_MEDIA_REMOVAL, &PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &dwBytesReturned, NULL);

	bool result = DeviceIoControl(hVol, IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &dwRet, 0);

	CloseHandle(hVol);
	return result;
}
