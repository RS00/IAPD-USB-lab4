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
	char DriveLetter = symbPath;
	char szRootPath[] = "X:\\";   // "X:\"
	szRootPath[0] = DriveLetter;
	char szVolumeAccessPath[] = "\\\\.\\X:";   // "\\.\X:"
	szVolumeAccessPath[4] = DriveLetter;
	long DiskNumber = -1;

	HANDLE hVolume = CreateFileA(szVolumeAccessPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hVolume == INVALID_HANDLE_VALUE) {
		return false;
	}

	STORAGE_DEVICE_NUMBER sdn;
	DWORD dwBytesReturned = 0;
	long res = DeviceIoControl(hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
	if (res) {
		DiskNumber = sdn.DeviceNumber;
	}
	CloseHandle(hVolume);

	if (DiskNumber == -1) {
		return false;
	}

	UINT DriveType = GetDriveTypeA(szRootPath);

	DEVINST DevInst = getDrivesDevInstByDiskNumber(DiskNumber, DriveType);

	if (DevInst == 0) {
		return 1;
	}

	ULONG Status = 0;
	ULONG ProblemNumber = 0;
	PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;
	WCHAR VetoNameW[MAX_PATH];
	bool bSuccess = false;

	res = CM_Get_Parent(&DevInst, DevInst, 0);  
	res = CM_Get_DevNode_Status(&Status, &ProblemNumber, DevInst, 0);
	bool IsRemovable = ((Status & DN_REMOVABLE) != 0);


	for (long tries = 1; tries <= 3; tries++) { 
		VetoNameW[0] = 0;
		if (IsRemovable) {
			res = CM_Request_Device_EjectW(DevInst, &VetoType, VetoNameW, MAX_PATH, 0);
			
		}
		else {
			res = CM_Query_And_Remove_SubTreeW(DevInst, &VetoType, VetoNameW, MAX_PATH * 2, 0);
		}
		bSuccess = (res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);
		if (bSuccess) {
			break;
		}
		else {
			Sleep(200); 
		}
	}

	return true;
}

DEVINST USBDevice::getDrivesDevInstByDiskNumber(long DiskNumber, UINT DriveType) {

	GUID* guid;

	switch (DriveType) {
	case DRIVE_REMOVABLE:
	case DRIVE_FIXED:
		guid = (GUID*)(void*)&GUID_DEVINTERFACE_DISK;
		break;
	case DRIVE_CDROM:
		guid = (GUID*)(void*)&GUID_DEVINTERFACE_CDROM;
		break;
	default:
		return 0;
	}

	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (hDevInfo == INVALID_HANDLE_VALUE) {
		return 0;
	}


	DWORD dwIndex = 0;
	SP_DEVICE_INTERFACE_DATA devInterfaceData = { 0 };
	devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	BOOL bRet = FALSE;

	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd;
	SP_DEVICE_INTERFACE_DATA         spdid;
	SP_DEVINFO_DATA                  spdd;
	DWORD                            dwSize;

	spdid.cbSize = sizeof(spdid);

	while (true) {
		bRet = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guid, dwIndex, &devInterfaceData);
		if (!bRet) {
			break;
		}

		SetupDiEnumInterfaceDevice(hDevInfo, NULL, guid, dwIndex, &spdid);

		dwSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, NULL, 0, &dwSize, NULL);

		if (dwSize) {
			pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
			if (pspdidd == NULL) {
				continue; 
			}
			pspdidd->cbSize = sizeof(*pspdidd); 
			ZeroMemory((PVOID)&spdd, sizeof(spdd));
			spdd.cbSize = sizeof(spdd);

			long res = SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, pspdidd, dwSize, &dwSize, &spdd);
			if (res) {

				HANDLE hDrive = CreateFile(pspdidd->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
				if (hDrive != INVALID_HANDLE_VALUE) {
					STORAGE_DEVICE_NUMBER sdn;
					DWORD dwBytesReturned = 0;
					res = DeviceIoControl(hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
					if (res) {
						if (DiskNumber == (long)sdn.DeviceNumber) {
							CloseHandle(hDrive);
							HeapFree(GetProcessHeap(), 0, pspdidd);
							SetupDiDestroyDeviceInfoList(hDevInfo);
							return spdd.DevInst;
						}
					}
					CloseHandle(hDrive);
				}
			}
			HeapFree(GetProcessHeap(), 0, pspdidd);
		}
		dwIndex++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return 0;
}
