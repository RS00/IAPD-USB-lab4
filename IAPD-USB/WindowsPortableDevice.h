#pragma once
#include <Windows.h>
#include <PortableDevice.h>
#include <PortableDeviceTypes.h>
#include <PortableDeviceApi.h>
#include <wrl\client.h>
#include <string>
#include <vector>
#define PATH_SIZE 128
using namespace Microsoft::WRL;
using namespace std;

#pragma comment(lib, "PortableDeviceGuids.lib")

struct PORTABLE_DEVICES
{
	wstring deviceName;
	wstring path;
	vector<wstring> storName;
	vector<long long> total;
	vector<long long> free;
	int count;
};

class FileEnumerator
{
	friend class WindowsPortableDevice;
	IEnumPortableDeviceObjectIDs *enumObjectIDs;
	void setEnumerator(IEnumPortableDeviceObjectIDs *enumerator);
	int getNextFile(wstring *paths, int pathsNum);
	FileEnumerator();
	~FileEnumerator();
};

class WindowsPortableDevice
{
private:
	IPortableDeviceManager *deviceManager = nullptr;
	IPortableDeviceContent *content = nullptr;
	IPortableDevice *device = nullptr;
	IPortableDeviceProperties *properties = nullptr;
	IPortableDeviceKeyCollection *keys = nullptr;
	DWORD deviceCount;
	void createKeyCollection();
	void createProperties();
	bool getFileName(const wstring &path, wstring *str);
	bool getFileCharParam(const wstring &path, wstring *str, const PROPERTYKEY &key);
	bool getFileInt8Param(const wstring &path, ULONGLONG *num, const PROPERTYKEY &key);
	bool getObjectType(const wstring &path, GUID *guid);
	bool getFileGUIDParam(const wstring &path, GUID *guid, const PROPERTYKEY &key);
	bool getFunctionalObjectCategoty(const wstring &path, GUID *guid);
	bool getSpaceFree(const wstring &path, unsigned long long *size);
	bool getSpace(const wstring &path, unsigned long long *size);
	bool getDevicePath(int deviceIndex, wstring *path);
	bool getFriendlyName(wstring *path, wstring *str);
	void refreshDeviceCount();
	void getFileEnumerator(const wstring &path, FileEnumerator &enumerator);
		
public:
	WindowsPortableDevice();
	PORTABLE_DEVICES *getDevices();
	DWORD getDeviceCount();
	~WindowsPortableDevice();
};

