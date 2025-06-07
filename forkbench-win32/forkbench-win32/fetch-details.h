#pragma once
#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>


#pragma comment(lib, "wbemuuid.lib")
#include "app_defs.h"
RetVal GetWMIInfo(const std::wstring& wmiClass, const std::wstring& property, std::wstring& result);
//void collectWMIInfo(HWND hwnd);
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
public:
	powerInfo();
};
class outputDeviceInfo : public Computer {
	std::wstring soundDeviceName;
	std::wstring soundDeviceManufacturer;
	std::wstring soundDeviceStatus;
public:
	outputDeviceInfo();
};
class inputDeviceInfo  : public Computer {
	std::wstring inputDeviceName;
	std::wstring inputDeviceID;
	std::wstring inputDeviceNumberofFunctionKeys;
	std::wstring inputPointingDeviceName;
	std::wstring inputPointingDeviceID;
	std::wstring inputPointingDeviceNumberofButtons;
public:
	inputDeviceInfo();
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
public:
	NetworkInfo();
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
public:
	OSInfo(); 
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
public:
	StorageInfo();

};
class MotherboardInfo : public Computer {
	std::wstring motherboardManufacturer;
	std::wstring motherboardProductName;
	std::wstring motherboardSerialNumber;
	std::wstring motherboardBIOSVersion;
	std::wstring motherboardReleaseDate;
public:
	MotherboardInfo();

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
public:
	SystemMemoryInfo();
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
	
public:

	GPUandDisplayInfo();
};
class CPUInfo : public Computer {
	std::wstring cpuManufacturer;        // Processor Manufacturer Name: Intel/ AMD
	std::wstring cpuName;                //Processor Model Info:  Intel(R) [Processor Family] CPU [Model Number] @ [Clock Speed] \ "AMD [Family] [Model] [Details]"
	std::wstring cpuNumberOfCores;
	std::wstring cpuNumberOfThreads;
	std::wstring cpuCodeName;            
	std::wstring cpuMaxClockSpeed;
	std::wstring cpuCurrentClockSpeed;
	std::wstring cpuL2Cache;
	std::wstring cpuL3Cache;
	std::wstring cpuArchitecture;
	std::wstring cpuLoadPercentage;
	std::wstring cpuVoltageCapacity;
public:
	CPUInfo();
	
	// Setter functions to fetch details
	RetVal setCPUManufacturer();
	RetVal setCPUName();
	RetVal setCPUNumberOfCores();
	RetVal setCPUNumberOfThreads();
	RetVal setCPUCodeName();
	RetVal setCPUMaxClockSpeed();
	RetVal setCPUCurrentClockSpeed();
	RetVal setCPUL2Cache();
	RetVal setCPUL3Cache();
	RetVal setCPUArchitecture();
	RetVal setCPULoadPercentage();
	RetVal setCPUVoltageCapacity();
			  
	// Getter functions to return asked values
	std::wstring getCPUName();
	std::wstring getCPUManufacturer();
	std::wstring getCPUNumberOfCores();
	std::wstring getCPUNumberOfThreads();
};
