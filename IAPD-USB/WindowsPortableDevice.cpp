#include "WindowsPortableDevice.h"



WindowsPortableDevice::WindowsPortableDevice()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	deviceCount = 0;

	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&deviceManager));
	hr = deviceManager->GetDevices(nullptr, &deviceCount);
	hr = CoCreateInstance(CLSID_PortableDeviceFTM,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&device));
}


void WindowsPortableDevice::getFileEnumerator(const std::wstring &path, FileEnumerator &enumerator)
{
	IEnumPortableDeviceObjectIDs *enumObjectIDs = nullptr;

	HRESULT hr = content->EnumObjects(0,
		(path == L"") ? WPD_DEVICE_OBJECT_ID : path.c_str(),
		nullptr,
		&enumObjectIDs);
	enumerator.setEnumerator(enumObjectIDs);
}

WindowsPortableDevice::~WindowsPortableDevice()
{
}

DWORD WindowsPortableDevice::getDeviceCount()
{
	refreshDeviceCount();
	return deviceCount;
}

PORTABLE_DEVICES* WindowsPortableDevice::getDevices()
{
	wstring path;
	IPortableDeviceValues *clientInformation = nullptr;
	refreshDeviceCount();
	PORTABLE_DEVICES *pdMass = new PORTABLE_DEVICES[deviceCount];
	for (int i = 0; i < deviceCount; i++)
	{
		pdMass[i].count = 0;
		getDevicePath(i, &path);
		if (wcsstr(path.c_str(), L"usbstor"))
		{
			continue;
		}
		wstring name;
		getFriendlyName(&path, &name);
		HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&clientInformation));
		clientInformation->SetStringValue(WPD_CLIENT_NAME, L"WPD Viewer");
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_MAJOR_VERSION, 1);
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_MINOR_VERSION, 0);
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_REVISION, 1);
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_DESIRED_ACCESS, GENERIC_READ);
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_SHARE_MODE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
		clientInformation->SetUnsignedIntegerValue(WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE, SECURITY_IMPERSONATION);

		hr = device->Open(path.c_str(), clientInformation);
		hr = device->Content(&content);

		FileEnumerator fileEnum;

		// Get root directory
		getFileEnumerator(L"", fileEnum);
		// or some directory
		//path = L"o91900000";
		//device.getFileEnumerator(path, fileEnum);
		wstring files[5];
		int received;
		int fCount = 0;
		while (received = fileEnum.getNextFile(files, 5)) {
			for (int j = 0; j < received; j++) {
				wstring name;
				bool r = getFileName(files[j], &name);
				pdMass[i].storName.push_back(name);
				GUID guid;
				getObjectType(files[j], &guid);
				if (IsEqualGUID(guid, WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT)) {
					getFunctionalObjectCategoty(files[j], &guid);
					if (IsEqualGUID(guid, WPD_FUNCTIONAL_CATEGORY_STORAGE)) {

						ULONGLONG total;
						if (getSpace(files[j], &total)) {
							pdMass[i].total.push_back(total);
						}

						ULONGLONG free;
						if (getSpaceFree(files[j], &free)) {
							pdMass[i].free.push_back(free);
						}
					}
				}
				fCount++;
			}
		}
		pdMass[i].deviceName =  name;
		pdMass[i].path = path;
		pdMass[i].count = fCount;
		if (clientInformation)
			clientInformation->Release();
		hr = device->Close();
	}
	return pdMass;
}

bool WindowsPortableDevice::getSpaceFree(const std::wstring &path, unsigned long long *size) {
	return getFileInt8Param(path, size, WPD_STORAGE_FREE_SPACE_IN_BYTES);
}

bool WindowsPortableDevice::getSpace(const std::wstring &path, unsigned long long *size) {
	return getFileInt8Param(path, size, WPD_STORAGE_CAPACITY);
}

bool WindowsPortableDevice::getObjectType(const std::wstring &path, GUID *guid) {
	return getFileGUIDParam(path, guid, WPD_OBJECT_CONTENT_TYPE);
}

bool WindowsPortableDevice::getFunctionalObjectCategoty(const std::wstring &path, GUID *guid) {
	return getFileGUIDParam(path, guid, WPD_FUNCTIONAL_OBJECT_CATEGORY);
}


bool WindowsPortableDevice::getFileGUIDParam(const std::wstring &path, GUID *guid, const PROPERTYKEY &key) {
	bool result = false;
	IPortableDeviceValues *objectProperties = nullptr;
	createProperties();
	createKeyCollection();
	HRESULT hr = keys->Add(key);

	hr = properties->GetValues(path.c_str(), keys, &objectProperties);
	if (FAILED(hr)) goto error;

	hr = objectProperties->GetGuidValue(key, guid);
	if (FAILED(hr)) goto error;

	result = true;
error:
	if (objectProperties)
		objectProperties->Release();
	return result;
}

bool WindowsPortableDevice::getFileName(const std::wstring &path, std::wstring *str) {
	bool result;
	result = getFileCharParam(path, str, WPD_OBJECT_ORIGINAL_FILE_NAME);
	if (!result)
		result = getFileCharParam(path, str, WPD_OBJECT_NAME);
	return result;
}

bool WindowsPortableDevice::getFileCharParam(const std::wstring &path, std::wstring *str, const PROPERTYKEY &key) {
	bool result = false;
	IPortableDeviceValues *objectProperties = nullptr;
	wchar_t *temp = nullptr;
	createProperties();
	createKeyCollection();
	HRESULT hr = keys->Add(key);
	hr = properties->GetValues(path.c_str(), keys, &objectProperties);
	if (FAILED(hr)) goto error;

	hr = objectProperties->GetStringValue(key, &temp);
	if (FAILED(hr)) goto error;

	*str = temp;
	result = true;
error:
	if (objectProperties)
		objectProperties->Release();
	if (temp)
		CoTaskMemFree(temp);
	return result;
}

void WindowsPortableDevice::createProperties() {
	if (!properties) {
		content->Properties(&properties);
	}
}


void WindowsPortableDevice::createKeyCollection() {
	if (!keys) {
		HRESULT hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&keys));
	}
	keys->Clear();
}

bool WindowsPortableDevice::getFileInt8Param(const std::wstring &path, ULONGLONG *num, const PROPERTYKEY &key) {
	bool result = false;
	IPortableDeviceValues *objectProperties = nullptr;

	createProperties();
	createKeyCollection();
	HRESULT hr = keys->Add(key);
	hr = properties->GetValues(path.c_str(), keys, &objectProperties);
	if (FAILED(hr)) goto error;

	hr = objectProperties->GetUnsignedLargeIntegerValue(key, num);
	if (FAILED(hr)) goto error;

	result = true;
error:
	if (objectProperties)
		objectProperties->Release();
	return result;
}
				

VOID WindowsPortableDevice::refreshDeviceCount()
{
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&deviceManager));
	hr = deviceManager->GetDevices(nullptr, &deviceCount);
	hr = CoCreateInstance(CLSID_PortableDeviceFTM,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&device));
}

bool WindowsPortableDevice::getDevicePath(int deviceIndex, wstring *path) {
	WCHAR **pnpDeviceIDs = nullptr;
	DWORD retrievedDeviceIDCount = 0;

	if (!path || deviceIndex < 0 || deviceCount < 1)
		return FALSE;

	pnpDeviceIDs = new WCHAR*[deviceCount]; //Names
	retrievedDeviceIDCount = deviceCount;
	HRESULT hr = deviceManager->GetDevices(pnpDeviceIDs, &retrievedDeviceIDCount);
	if (FAILED(hr) || retrievedDeviceIDCount <= (DWORD)deviceIndex)
		return FALSE;

	*path = pnpDeviceIDs[deviceIndex];
	delete[] pnpDeviceIDs;
	return TRUE;
}

bool WindowsPortableDevice::getFriendlyName(wstring *path, wstring *str)
{
	DWORD length = 0;
	HRESULT hr = hr = deviceManager->GetDeviceDescription(path->c_str(), nullptr, &length);
	if (FAILED(hr))
		return FALSE;
	WCHAR *DFrName = new WCHAR[length];
	hr = deviceManager->GetDeviceDescription(path->c_str(), DFrName, &length);
	*str = DFrName;
	delete[] DFrName;
	return TRUE;
}




FileEnumerator::FileEnumerator() : enumObjectIDs(nullptr) {}

FileEnumerator::~FileEnumerator() {
	if (enumObjectIDs)
		enumObjectIDs->Release();
}

void FileEnumerator::setEnumerator(IEnumPortableDeviceObjectIDs *enumerator) {
	if (enumObjectIDs)
		enumObjectIDs->Release();
	enumObjectIDs = enumerator;
}

int FileEnumerator::getNextFile(std::wstring *paths, int pathsNum) {
	wchar_t **temp = new wchar_t*[pathsNum];

	ULONG result = 0;
	HRESULT hr = enumObjectIDs->Next(pathsNum, temp, &result);
	for (ULONG index = 0; index < result; index++) {
		paths[index] = temp[index];
		CoTaskMemFree(temp[index]);
	}
	delete[] temp;
	return result;
}
