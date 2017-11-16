#include "USBEnumerator.h"


USBEnumerator::USBEnumerator()
{
}


USBEnumerator::~USBEnumerator()
{
}


void USBEnumerator::getVolumeAndLetter(PSP_DEVICE_INTERFACE_DETAIL_DATA_A pDeviceInterfaceDetailData, vector<USBDevice> vectorOfDevices)
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
				USBDevice device(wstring(path.begin(), path.end()), wstring(symbPath.begin(), symbPath.end()));
				vectorOfDevices.push_back(device);
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

		getVolumeAndLetter(pDeviceInterfaceDetailData, vectorOfDevices);
		LocalFree(pDeviceInterfaceDetailData);
		pDeviceInterfaceDetailData = NULL;
	}
	return vectorOfDevices;
}