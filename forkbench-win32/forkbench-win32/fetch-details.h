#pragma once
#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>


#pragma comment(lib, "wbemuuid.lib")
#include "app_defs.h"
RetVal GetWMIInfo(const std::wstring& wmiClass, const std::wstring& property, std::wstring& result);
//void collectWMIInfo(HWND hwnd);
//std::wstring getL1CacheSize();
std::wstring decodeMemoryType(const std::wstring& typeCode);
std::wstring decodeProcessorArchitecture(std::wstring architectureCodeInput);


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
	RetVal settotalPhysicalMemory();
	RetVal setRAMType();
	RetVal setRAMSpeed();
	RetVal setFormFactor();
	RetVal setManufacturer();
	RetVal setRAMPartNumber();
	RetVal setRAMSerialNumber();
	RetVal setRAMCapacity();

	std::wstring gettotalPhysicalMemory();
	std::wstring getRAMCapacity();
	std::wstring getRAMSpeed();
	std::wstring getRAMManufacturer();
};
class GPUandDisplayInfo : public Computer {
	std::wstring gpuName;
	std::wstring gpuDriverVer;
	std::wstring gpuDriverDate;
	std::wstring gpuAdapterCompatibility; // Manufacturer
	std::wstring gpuTotalVRAM;
	std::wstring gpuVideoMemoryType;
	std::wstring gpuVideoArchitecture;
	std::wstring gpuVideoModeDescription;
	std::wstring gpuCurrentRefreshRate;
	std::wstring gpuCurrentHResolution;
	std::wstring gpuCurrentVResolution;
	std::wstring gpuMonitorType;
	std::wstring gpuScreenHeight;
	std::wstring gpuScreenWidth;
	
public:

	GPUandDisplayInfo();
	RetVal setgpuName();
	RetVal setgpuDriverVer();
	RetVal setgpuDriverDate();
	RetVal setgpuAdapterCompatibility();
	RetVal setgpuTotalVRAM();
	RetVal setgpuVideoMemoryType();
	RetVal setgpuVideoArchitecture();
	RetVal setgpuVideoModeDescription();
	RetVal setgpuCurrentRefreshRate();
	RetVal setgpuCurrentHResolution();
	RetVal setgpuCurrentVResolution();
	RetVal setgpuMonitorType();
	RetVal setgpuScreenHeight();
	RetVal setgpuScreenWidth();

	std::wstring getgpuName();
	std::wstring getgpuDriverVer();
	std::wstring getgpuDriverDate();
	std::wstring getgpuAdapterCompatibility();
	std::wstring getgpuTotalVRAM();
	std::wstring getgpuVideoMemoryType();
	std::wstring getgpuVideoArchitecture();
	std::wstring getgpuVideoModeDescription();
	std::wstring getgpuCurrentRefreshRate();
	std::wstring getgpuCurrentHResolution();
	std::wstring getgpuCurrentVResolution();
	std::wstring getgpuMonitorType();
	std::wstring getgpuScreenHeight();
	std::wstring getgpuScreenWidth();

};

class CPUInfo : public Computer {
	std::wstring cpuManufacturer;        // Processor Manufacturer Name: Intel/ AMD
	std::wstring cpuName;                //Processor Model Info:  Intel(R) [Processor Family] CPU [Model Number] @ [Clock Speed] \ "AMD [Family] [Model] [Details]"
	std::wstring cpuNumberOfCores;
	std::wstring cpuNumberOfThreads;         
	std::wstring cpuMaxClockSpeed;
	std::wstring cpuCurrentClockSpeed;
	std::wstring cpuL1Cache;
	std::wstring cpuL2Cache;
	std::wstring cpuL3Cache;
	std::wstring cpuArchitecture;
	std::wstring cpuCodeName;
	std::wstring cpuLoadPercentage;
	std::wstring cpuVoltageCapacity;

public:
	CPUInfo();
	RetVal getL1CacheSize(std::wstring &result);
	// Setter functions to fetch details
	RetVal setCPUManufacturer();
	RetVal setCPUName();
	RetVal setCPUNumberOfCores();
	RetVal setCPUNumberOfThreads();
	RetVal setCPUL1Cache();
	RetVal setCPUL2Cache();
	RetVal setCPUL3Cache();
	RetVal setCPUCodeName();
	RetVal setCPUMaxClockSpeed();
	RetVal setCPUCurrentClockSpeed();
	RetVal setCPUArchitecture();
	RetVal setCPULoadPercentage();
	RetVal setCPUVoltageCapacity();
			  
	// Getter functions to return asked values
	std::wstring getCPUName();
	std::wstring getCPUManufacturer();
	std::wstring getCPUNumberOfCores();
	std::wstring getCPUNumberOfThreads();
	std::wstring getCPUMaxClockSpeed();
	std::wstring getCPUCurrentClockSpeed();
	std::wstring getCPUL1Cache();
	std::wstring getCPUL2Cache();
	std::wstring getCPUL3Cache();
	std::wstring getCPUArchitecture();
};
