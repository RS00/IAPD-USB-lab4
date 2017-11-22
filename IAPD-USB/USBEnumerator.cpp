#include "USBEnumerator.h"


USBEnumerator::USBEnumerator()
{
}


USBEnumerator::~USBEnumerator()
{
}


void USBEnumerator::addDevicesToVector(PSP_DEVICE_INTERFACE_DETAIL_DATA_A pDeviceInterfaceDetailData, vector<USBDevice> *vectorOfDevices)
{
	DWORD requiredLength = 0;
	if (strstr(pDeviceInterfaceDetailData->DevicePath, "usbstor"))
	{
		DWORD len = strlen(pDeviceInterfaceDetailData->DevicePath);
		pDeviceInterfaceDetailData->DevicePath[len] = '\\';
		pDeviceInterfaceDetailData->DevicePath[len + 1] = '\0';
		CHAR volume[BUFFER_SIZE];
		CHAR letter[BUFFER_SIZE];
		if (GetVolumeNameForVolumeMountPointA(pDeviceInterfaceDetailData->DevicePath, volume, BUFFER_SIZE))
		{
			if (GetVolumePathNamesForVolumeNameA(volume, letter, 255, &requiredLength))
			{
				string path(pDeviceInterfaceDetailData->DevicePath), symbPath(letter);
				string nameOfDevice = getNameOfVolume(volume);
				long long total, free, busy;
				this->getVolumeSize(path.c_str(), &free, &total, &busy);
				USBDevice device(nameOfDevice, path, symbPath, total, free, busy);
				vectorOfDevices->push_back(device);
			}
		}
	}
}


vector<USBDevice> USBEnumerator::getVectorOfDevices()
{
	vector<USBDevice> vectorOfDevices;
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA deviceInfoData;
	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA_A pDeviceInterfaceDetailData = NULL;
	CHAR *pBuff = NULL;


	//Get device information set
	hDevInfo = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_VOLUME, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
		return vectorOfDevices;

	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	//Enumerate device interfaces that are contained in device inforamtion set
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_VOLUME, i, &deviceInterfaceData); i++)
	{
		ULONG requiredLength = 0;

		//Get detail info
		while (!SetupDiGetDeviceInterfaceDetailA(hDevInfo,
			&deviceInterfaceData, pDeviceInterfaceDetailData, requiredLength, &requiredLength, &deviceInfoData))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				LocalFree(pDeviceInterfaceDetailData);
				pDeviceInterfaceDetailData = NULL;
				pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)LocalAlloc(LMEM_FIXED, requiredLength + 2);
				pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
			}
			else
			{
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return vectorOfDevices;
			}
		}
		addDevicesToVector(pDeviceInterfaceDetailData, &vectorOfDevices);
		LocalFree(pDeviceInterfaceDetailData);
		pDeviceInterfaceDetailData = NULL;
	}
	return vectorOfDevices;
}

string USBEnumerator::getNameOfVolume(string volName)
{
	char nameBuffer[BUFFER_SIZE];
	char sysNameBuffer[BUFFER_SIZE];
	DWORD number;
	DWORD length;
	DWORD fileSF;
	int count = 0;

	while (!GetVolumeInformationA(volName.c_str(), nameBuffer, sizeof(nameBuffer),
		&number, &length, &fileSF, sysNameBuffer, sizeof(sysNameBuffer)))
	{
		count++;
		if (count > 10000)
			break;
	}
	return string(nameBuffer);
}

void USBEnumerator::getVolumeSize(const char * name, long long int *free, long long int *total, long long int *busy)
{
	long long FreeBytesAvailable;
	int count = 0;
	while (!GetDiskFreeSpaceExA(
		name, // directory name
		(PULARGE_INTEGER)&FreeBytesAvailable, // bytes available to caller
		(PULARGE_INTEGER)total, // bytes on disk
		(PULARGE_INTEGER)free // free bytes on disk
	))
	{
		count++;
		if (count > 10000)
		{
			*free = 0;
			*total = 0;
			*busy = 0;
			return;
		}
	}
	
	*busy = *total - *free;
	return;
}