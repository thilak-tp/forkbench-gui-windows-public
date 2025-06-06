#pragma once
#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>
#pragma comment(lib, "wbemuuid.lib")

std::wstring GetWMIInfo(const std::wstring& wmiClass, const std::wstring& property);
void collectWMIInfo(HWND hwnd);
std::wstring decodeMemoryType(const std::wstring& typeCode);


class Computer {

};
class powerInfo : public Computer {
	std::wstring powerBatteryStatus;
	std::wstring powerBatteryChargeRemaining;
	std::wstring powerEstimatedRunTime;
	std::wstring powerBatteryChemistry;
	std::wstring powerElementName;
	std::wstring powerIsActive;
};
class outputDeviceInfo : public Computer {
	std::wstring soundDeviceName;
	std::wstring soundDeviceManufacturer;
	std::wstring soundDeviceStatus;
};
class inputDeviceInfo  : public Computer {
	std::wstring inputDeviceName;
	std::wstring inputDeviceID;
	std::wstring inputDeviceNumberofFunctionKeys;
	std::wstring inputPointingDeviceName;
	std::wstring inputPointingDeviceID;
	std::wstring inputPointingDeviceNumberofButtons;
};

class NetworkInfo : public Computer {
	std::wstring networkAdapterName;
	std::wstring networkMACAddress;
	std::wstring networkConnectionID;
	std::wstring networkSpeed;
	std::wstring networkAdapterType;
	std::wstring networkAdapterDescription;
	std::wstring networkAdapterIPAddress;
	std::wstring networkDefaultGateway;
	std::wstring networkDNSHostName;
	std::wstring networkDHCPEnabled;
};
class OSInfo : public Computer {
	std::wstring osName;
	std::wstring osVersion;
	std::wstring osBuildNumber;
	std::wstring osUserName;
	std::wstring osLastBootUpTime;
	std::wstring osInstallDate;
	std::wstring osRegisteredUser;
	std::wstring OSArchitecture;
	std::wstring osLocale;
	std::wstring osSerialNumber;
	std::wstring osSystemDrive;
};
class StorageInfo : public Computer {
	std::wstring diskSize;
	std::wstring diskType;
	std::wstring diskModel;
	std::wstring diskSerialNumber;
	std::wstring diskInterfaceType;
	std::wstring diskFirmwareVersion;
	std::wstring diskPartition;
	std::wstring diskDeviceID;
	std::wstring diskFileSystem;
	std::wstring diskVolumeName;
	std::wstring diskLabel;
	std::wstring diskDriveLetter;
	std::wstring diskCapacity;
	std::wstring diskFreeSpace;
	std::wstring diskFileSystemType;

};
class MotherboardInfo : public Computer {
	std::wstring motherboardManufacturer;
	std::wstring motherboardProductName;
	std::wstring motherboardSerialNumber;
	std::wstring motherboardBIOSVersion;
	std::wstring motherboardReleaseDate;
	std::wstring motherboardManufacturer;

};
class SystemMemoryInfo: public Computer {
	std::wstring totalPhysicalMemory;
	std::wstring RAMType;
	std::wstring RAMSpeed;
	std::wstring RAMFormFactor;
	std::wstring RAMManufacturer;
	std::wstring RAMPartNumber;
	std::wstring RAMSerialNumber;
	std::wstring RAMCapacity;
};
class GPUandDisplayInfo : public Computer {
	std::wstring gpuName;
	std::wstring gpuDriverVer;
	std::wstring gpuTotalVRAM;
	std::wstring gpuVideoModeDescription;
	std::wstring gpuCurrentRefreshRate;
	std::wstring gpuCurrentHResolution;
	std::wstring gpuCurrentVResolution;
	std::wstring gpuMonitorType;
	std::wstring gpuScreenHeight;
	std::wstring gpuScreenWidth;

};
class CPUInfo : public Computer {
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

