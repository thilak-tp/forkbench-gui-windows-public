#pragma once
#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>
#pragma comment(lib, "wbemuuid.lib")

std::wstring GetWMIInfo(const std::wstring& wmiClass, const std::wstring& property);
void collectWMIInfo(HWND hwnd);
std::wstring decodeMemoryType(const std::wstring& typeCode);

class outputDeviceInfo {
	
};
class inputDeviceInfo {

};

class NetworkInfo {

};
class OSInfo {

};
class StorageInfo {

};
class MotherboardInfo {

};
class SystemMemoryInfo {

};
class GPUandDisplayInfo {
	std::wstring gpuName;
	std::wstring gpuDriverVer;
	std::wstring gpuTotalVRAM;
	std::wstring ;
	std::wstring;
	std::wstring;
	std::wstring;
	std::wstring;
	std::wstring;

};
class CPUInfo{
	std::wstring cpuName;
	std::wstring cpuCodeName;
	std::wstring cpuNumberOfCores;
	std::wstring cpuNumberOfThreads;
	std::wstring cpuMaxClockSpeed;
	std::wstring cpuCurrentClockSpeed;
	std::wstring cpuL2Cache;
	std::wstring cpuL3Cache;
	std::wstring cpuArchitecture;
	std::wstring cpuManufacturer;
	std::wstring cpuLoadPercentage;
	std::wstring cpuVoltageCapacity;
};

class Computer {
	
};