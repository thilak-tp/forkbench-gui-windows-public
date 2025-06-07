#include "fetch-details.h"

RetVal GetWMIInfo(const std::wstring& wmiClass, const std::wstring& property, std::wstring& result) {
    HRESULT hres;
    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;
    IEnumWbemClassObject* pEnumerator = nullptr;
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    //std::wstring result;

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) return COM_INIT_FAILED;

    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hres)) return SEC_INIT_FAILED;

    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) return WLOCATOR_CREATION_FAILED;

    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0,
        NULL, 0, 0, &pSvc);
    if (FAILED(hres)) return CONNECT_SERVER_FAILED;

    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hres)) return SET_PROXY_BLANKED_FAILED;

    std::wstring query = L"SELECT " + property + L" FROM " + wmiClass;
    hres = pSvc->ExecQuery(_bstr_t("WQL"), _bstr_t(query.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    if (FAILED(hres)) return QUERY_EXEC_FAILED;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (!uReturn) break;

        VARIANT vtProp;
        hr = pclsObj->Get(property.c_str(), 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr)) {
            switch (vtProp.vt) {
            case VT_BSTR:
                result = vtProp.bstrVal;
                break;
            case VT_I4:
            case VT_UI4:
                result = std::to_wstring(vtProp.ulVal);
                break;
            case VT_I8:
            case VT_UI8:
                result = std::to_wstring(vtProp.ullVal);
                break;
            case VT_BOOL:
                result = (vtProp.boolVal == VARIANT_TRUE) ? L"True" : L"False";
                break;
            default:
                return UNSUPPORTED_TYPE;
                break;
            }
        }
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();
    return SUCCESS;
}
std::wstring decodeMemoryType(const std::wstring& typeCode) {
    if (typeCode == L"20") return L"DDR";
    if (typeCode == L"21") return L"DDR2";
    if (typeCode == L"24") return L"DDR3";
    if (typeCode == L"26") return L"DDR4";
    if (typeCode == L"30") return L"DDR5";
    return L"Unknown";
}
/*
void collectWMIInfo(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    HFONT hFont = CreateFontW(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    int lineHeight = 24;
    int columnWidth = 350;  // Adjust based on how wide your strings are
    int padding = 20;
    int x = padding;
    int y = padding;
    int maxY = clientRect.bottom - padding;

    auto drawLine = [&](const std::wstring& label, const std::wstring& value) {
        std::wstring fullLine = label + L": " + value;

        // Check if next line will exceed vertical space
        if (y + lineHeight > maxY) {
            y = padding;
            x += columnWidth;
        }

        TextOutW(hdc, x, y, fullLine.c_str(), (int)fullLine.length());
        y += lineHeight;
        };

    // CPU Info
    drawLine(L"CPU Name", GetWMIInfo(L"Win32_Processor", L"Name"));
    drawLine(L"CPU Codename", GetWMIInfo(L"Win32_Processor", L"Caption"));
    drawLine(L"Cores", GetWMIInfo(L"Win32_Processor", L"NumberOfCores"));
    drawLine(L"Threads", GetWMIInfo(L"Win32_ComputerSystem", L"NumberOfLogicalProcessors"));
    drawLine(L"Threads", GetWMIInfo(L"Win32_Processor", L"ThreadCount"));
    drawLine(L"MaxClockSpeed", GetWMIInfo(L"Win32_Processor", L"MaxClockSpeed"));
    drawLine(L"CurrentClockSpeed", GetWMIInfo(L"Win32_Processor", L"CurrentClockSpeed"));
    drawLine(L"L2CacheSize", GetWMIInfo(L"Win32_Processor", L"L2CacheSize"));
    drawLine(L"L2CacheSize", GetWMIInfo(L"Win32_Processor", L"L3CacheSize"));
    drawLine(L"Architecture", GetWMIInfo(L"Win32_Processor", L"Architecture"));
    drawLine(L"Manufacturer", GetWMIInfo(L"Win32_Processor", L"Manufacturer"));
    drawLine(L"LoadPercentage", GetWMIInfo(L"Win32_Processor", L"LoadPercentage"));
    drawLine(L"Voltage Caps", GetWMIInfo(L"Win32_Processor", L"VoltageCaps"));
    

    // Motherboard & BIOS
    drawLine(L"Motherboard", GetWMIInfo(L"Win32_BaseBoard", L"Product"));
    drawLine(L"Manufacturer", GetWMIInfo(L"Win32_BaseBoard", L"Manufacturer"));
    drawLine(L"SerialNumber", GetWMIInfo(L"Win32_BaseBoard", L"SerialNumber"));
    drawLine(L"BIOS Version", GetWMIInfo(L"Win32_BIOS", L"SMBIOSBIOSVersion"));
    drawLine(L"ReleaseDate", GetWMIInfo(L"Win32_BIOS", L"ReleaseDate"));
    drawLine(L"Manufacturer", GetWMIInfo(L"Win32_BIOS", L"Manufacturer"));


    // RAM
    drawLine(L"Total RAM", GetWMIInfo(L"Win32_ComputerSystem", L"TotalPhysicalMemory"));
    std::wstring ramTypeCode = GetWMIInfo(L"Win32_PhysicalMemory", L"MemoryType");
    drawLine(L"RAM Type", GetWMIInfo(L"Win32_PhysicalMemory", L"MemoryType"));
    drawLine(L"RAM Speed", GetWMIInfo(L"Win32_PhysicalMemory", L"Speed"));
    drawLine(L"Capacity", GetWMIInfo(L"Win32_PhysicalMemory", L"Capacity"));
    drawLine(L"FormFactor", GetWMIInfo(L"Win32_PhysicalMemory", L"FormFactor"));
    drawLine(L"Manufacturer", GetWMIInfo(L"Win32_PhysicalMemory", L"Manufacturer"));
    drawLine(L"PartNumber", GetWMIInfo(L"Win32_PhysicalMemory", L"PartNumber"));
    drawLine(L"SerialNumber", GetWMIInfo(L"Win32_PhysicalMemory", L"SerialNumber"));

    // GPU
    drawLine(L"GPU", GetWMIInfo(L"Win32_VideoController", L"Name"));
    std::wstring vramBytes = GetWMIInfo(L"Win32_VideoController", L"AdapterRAM");
    //drawLine(L"VRAM", formatBytes(vramBytes));

    drawLine(L"GPU Driver", GetWMIInfo(L"Win32_VideoController", L"DriverVersion"));
    drawLine(L"VRAM", GetWMIInfo(L"Win32_VideoController", L"AdapterRAM"));
    drawLine(L"VideoProcessor", GetWMIInfo(L"Win32_VideoController", L"VideoProcessor"));
    drawLine(L"VideoModeDescription", GetWMIInfo(L"Win32_VideoController", L"VideoModeDescription"));
    drawLine(L"CurrentRefreshRate", GetWMIInfo(L"Win32_VideoController", L"CurrentRefreshRate"));
    drawLine(L"CurrentHorizontalResolution", GetWMIInfo(L"Win32_VideoController", L"CurrentHorizontalResolution"));
    drawLine(L"CurrentVerticalResolution", GetWMIInfo(L"Win32_VideoController", L"CurrentVerticalResolution"));
    drawLine(L"MonitorType", GetWMIInfo(L"Win32_DesktopMonitor", L"MonitorType"));
    drawLine(L"ScreenHeight", GetWMIInfo(L"Win32_DesktopMonitor", L"ScreenHeight"));
    drawLine(L"ScreenWidth", GetWMIInfo(L"Win32_DesktopMonitor", L"ScreenWidth"));
    // OS
    drawLine(L"OS", GetWMIInfo(L"Win32_OperatingSystem", L"Caption"));
    drawLine(L"Build", GetWMIInfo(L"Win32_OperatingSystem", L"BuildNumber"));
	drawLine(L"Version", GetWMIInfo(L"Win32_OperatingSystem", L"Version"));
    drawLine(L"UserName", GetWMIInfo(L"Win32_OperatingSystem", L"UserName"));
    drawLine(L"LastBootUpTime", GetWMIInfo(L"Win32_OperatingSystem", L"LastBootUpTime"));
    drawLine(L"InstallDate", GetWMIInfo(L"Win32_OperatingSystem", L"InstallDate"));
    drawLine(L"RegisteredUser", GetWMIInfo(L"Win32_OperatingSystem", L"RegisteredUser"));
    drawLine(L"OSArchitecture", GetWMIInfo(L"Win32_OperatingSystem", L"OSArchitecture"));
    drawLine(L"Locale", GetWMIInfo(L"Win32_OperatingSystem", L"Locale"));
    drawLine(L"SerialNumber", GetWMIInfo(L"Win32_OperatingSystem", L"SerialNumber"));
    drawLine(L"SystemDrive", GetWMIInfo(L"Win32_OperatingSystem", L"SystemDrive"));
    // Storage
    drawLine(L"Disk Size", GetWMIInfo(L"Win32_DiskDrive", L"Size"));
    drawLine(L"Disk Type", GetWMIInfo(L"Win32_DiskDrive", L"MediaType"));
    drawLine(L"Model", GetWMIInfo(L"Win32_DiskDrive", L"Model"));
    drawLine(L"InterfaceType", GetWMIInfo(L"Win32_DiskDrive", L"InterfaceType"));
    drawLine(L"SerialNumber", GetWMIInfo(L"Win32_DiskDrive", L"SerialNumber"));
    drawLine(L"FirmwareRevision", GetWMIInfo(L"Win32_DiskDrive", L"FirmwareRevision"));
    drawLine(L"Partitions", GetWMIInfo(L"Win32_DiskDrive", L"Partitions"));
    drawLine(L"Partitions", GetWMIInfo(L"Win32_LogicalDisk", L"DeviceID"));
    drawLine(L"FileSystem", GetWMIInfo(L"Win32_LogicalDisk", L"FileSystem"));
    drawLine(L"Size", GetWMIInfo(L"Win32_LogicalDisk", L"Size"));
    drawLine(L"VolumeName", GetWMIInfo(L"Win32_LogicalDisk", L"VolumeName"));
    drawLine(L"DriveType", GetWMIInfo(L"Win32_LogicalDisk", L"DriveType"));
    drawLine(L"Label", GetWMIInfo(L"Win32_Volume", L"Label"));
    drawLine(L"DriveLetter", GetWMIInfo(L"Win32_Volume", L"DriveLetter"));
    drawLine(L"Capacity", GetWMIInfo(L"Win32_Volume", L"Capacity"));
    drawLine(L"FreeSpace", GetWMIInfo(L"Win32_Volume", L"FreeSpace"));
    drawLine(L"FileSystem", GetWMIInfo(L"Win32_Volume", L"FileSystem"));

	// Network
    drawLine(L"Name", GetWMIInfo(L"Win32_NetworkAdapter", L"Name"));
    drawLine(L"MACAddress", GetWMIInfo(L"Win32_NetworkAdapter", L"MACAddress"));
    drawLine(L"NetConnectionID", GetWMIInfo(L"Win32_NetworkAdapter", L"NetConnectionID"));
    drawLine(L"Speed", GetWMIInfo(L"Win32_NetworkAdapter", L"Speed"));
    drawLine(L"AdapterType", GetWMIInfo(L"Win32_NetworkAdapter", L"AdapterType"));
    drawLine(L"Description", GetWMIInfo(L"Win32_NetworkAdapterConfiguration", L"Description"));
    drawLine(L"IPAddress", GetWMIInfo(L"Win32_NetworkAdapterConfiguration", L"IPAddress"));
    drawLine(L"DefaultIPGateway", GetWMIInfo(L"Win32_NetworkAdapterConfiguration", L"DefaultIPGateway"));
    drawLine(L"DNSHostName", GetWMIInfo(L"Win32_NetworkAdapterConfiguration", L"DNSHostName"));
    drawLine(L"DHCPEnabled", GetWMIInfo(L"Win32_NetworkAdapterConfiguration", L"DHCPEnabled"));

    // Battery and Power
	drawLine(L"BatteryStatus", GetWMIInfo(L"Win32_Battery", L"BatteryStatus"));
    drawLine(L"EstimatedChargeRemaining", GetWMIInfo(L"Win32_Battery", L"EstimatedChargeRemaining"));
    drawLine(L"EstimatedRunTime", GetWMIInfo(L"Win32_Battery", L"EstimatedRunTime"));
    drawLine(L"Chemistry", GetWMIInfo(L"Win32_Battery", L"Chemistry"));
	drawLine(L"ElementName", GetWMIInfo(L"Win32_PowerPlan", L"ElementName"));
    drawLine(L"IsActive", GetWMIInfo(L"Win32_PowerPlan", L"IsActive"));

	// Input Devices
    drawLine(L"Description", GetWMIInfo(L"Win32_Keyboard", L"Description"));
    drawLine(L"DeviceID", GetWMIInfo(L"Win32_Keyboard", L"DeviceID"));
    drawLine(L"NumberOfFunctionKeys", GetWMIInfo(L"Win32_Keyboard", L"NumberOfFunctionKeys"));
    drawLine(L"Description", GetWMIInfo(L"Win32_PointingDevice", L"Description"));
    drawLine(L"DeviceID", GetWMIInfo(L"Win32_PointingDevice", L"DeviceID"));
    drawLine(L"NumberOfFunctionKeys", GetWMIInfo(L"Win32_PointingDevice", L"NumberOfFunctionKeys"));

	// Sound Devices
    drawLine(L"Name", GetWMIInfo(L"Win32_SoundDevice", L"Name"));
    drawLine(L"Manufacturer", GetWMIInfo(L"Win32_SoundDevice", L"Manufacturer"));
    drawLine(L"Status", GetWMIInfo(L"Win32_SoundDevice", L"Status"));
    // Clean up
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    EndPaint(hwnd, &ps);
    
}
*/

powerInfo::powerInfo() {
    powerBatteryStatus = L"";
    powerBatteryChargeRemaining = L"";
    powerEstimatedRunTime = L"";
    powerBatteryChemistry = L"";
    powerElementName = L"";
    powerIsActive = L"";
}

outputDeviceInfo::outputDeviceInfo() {
    soundDeviceName = L"";
    soundDeviceManufacturer = L"";
    soundDeviceStatus = L"";
}

inputDeviceInfo::inputDeviceInfo() {
    inputDeviceName = L"";
    inputDeviceID = L"";
    inputDeviceNumberofFunctionKeys = L"";
    inputPointingDeviceName = L"";
    inputPointingDeviceID = L"";
    inputPointingDeviceNumberofButtons = L"";
}

NetworkInfo::NetworkInfo() {
    networkAdapterName = L"";
    networkMACAddress = L"";
    networkConnectionID = L"";
    networkSpeed = L"";
    networkAdapterType = L"";
    networkAdapterDescription = L"";
    networkAdapterIPAddress = L"";
    networkDefaultGateway = L"";
    networkDNSHostName = L"";
    networkDHCPEnabled = L"";

}

OSInfo::OSInfo() {
    osName = L"";
    osVersion = L"";
    osBuildNumber = L"";
    osUserName = L"";
    osLastBootUpTime = L"";
    osInstallDate = L"";
    osRegisteredUser = L"";
    OSArchitecture = L"";
    osLocale = L"";
    osSerialNumber = L"";
    osSystemDrive = L"";
}

StorageInfo::StorageInfo() {
    diskSize = L"";
    diskType = L"";
    diskModel = L"";
    diskSerialNumber = L"";
    diskInterfaceType = L"";
    diskFirmwareVersion = L"";
    diskPartition = L"";
    diskDeviceID = L"";
    diskFileSystem = L"";
    diskVolumeName = L"";
    diskLabel = L"";
    diskDriveLetter = L"";
    diskCapacity = L"";
    diskFreeSpace = L"";
    diskFileSystemType = L"";
}

MotherboardInfo::MotherboardInfo() {
    motherboardManufacturer = L"";
    motherboardProductName = L"";
    motherboardSerialNumber = L"";
    motherboardBIOSVersion = L"";
    motherboardReleaseDate = L"";
}

SystemMemoryInfo::SystemMemoryInfo() {
    totalPhysicalMemory = L"";
    RAMType = L"";
    RAMSpeed = L"";
    RAMFormFactor = L"";
    RAMManufacturer = L"";
    RAMPartNumber = L"";
    RAMSerialNumber = L"";
    RAMCapacity = L"";
}

GPUandDisplayInfo::GPUandDisplayInfo() {
    gpuName = L"";
    gpuDriverVer = L"";
    gpuTotalVRAM = L"";
    gpuVideoModeDescription = L"";
    gpuCurrentRefreshRate = L"";
    gpuCurrentHResolution = L"";
    gpuCurrentVResolution = L"";
    gpuMonitorType = L"";
    gpuScreenHeight = L"";
    gpuScreenWidth = L"";
}

CPUInfo::CPUInfo() {
    cpuName = L"";
    cpuCodeName = L"";
    cpuNumberOfCores = L"";
    cpuNumberOfThreads = L"";
    cpuMaxClockSpeed = L"";
    cpuCurrentClockSpeed = L"";
    cpuL2Cache = L"";
    cpuL3Cache = L"";
    cpuArchitecture = L"";
    cpuManufacturer = L"";
    cpuLoadPercentage = L"";
    cpuVoltageCapacity = L"";
}
RetVal CPUInfo::setCPUManufacturer() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"Manufacturer", cpuManufacturer);
    if (cpuManufacturer == L"GenuineIntel")
        cpuManufacturer = L"Intel";
    else if (cpuManufacturer == L"AuthenticAMD")
        cpuManufacturer = L"AMD";
   
    switch (ret) {

    case COM_INIT_FAILED: return FAILED;
    case SEC_INIT_FAILED: return FAILED;
    case WLOCATOR_CREATION_FAILED: return FAILED;
    case CONNECT_SERVER_FAILED: return FAILED;
    case SET_PROXY_BLANKED_FAILED: return FAILED;
    case QUERY_EXEC_FAILED: return FAILED;
    case UNSUPPORTED_TYPE: return FAILED;

    }
    return SUCCESS;

}
RetVal CPUInfo::setCPUName() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"Name", cpuName);
    switch (ret) {
   
    case COM_INIT_FAILED: return FAILED;
    case SEC_INIT_FAILED: return FAILED;
    case WLOCATOR_CREATION_FAILED: return FAILED;
    case CONNECT_SERVER_FAILED: return FAILED;
    case SET_PROXY_BLANKED_FAILED: return FAILED;
    case QUERY_EXEC_FAILED: return FAILED;
    case UNSUPPORTED_TYPE: return FAILED;

    }
    return SUCCESS;
    
}


RetVal CPUInfo::setCPUNumberOfCores() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"NumberOfCores", cpuNumberOfCores);
    switch (ret) {

    case COM_INIT_FAILED: return FAILED;
    case SEC_INIT_FAILED: return FAILED;
    case WLOCATOR_CREATION_FAILED: return FAILED;
    case CONNECT_SERVER_FAILED: return FAILED;
    case SET_PROXY_BLANKED_FAILED: return FAILED;
    case QUERY_EXEC_FAILED: return FAILED;
    case UNSUPPORTED_TYPE: return FAILED;

    }
    return SUCCESS;

}
RetVal CPUInfo::setCPUNumberOfThreads() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"ThreadCount", cpuNumberOfThreads);
    switch (ret) {

    case COM_INIT_FAILED: return FAILED;
    case SEC_INIT_FAILED: return FAILED;
    case WLOCATOR_CREATION_FAILED: return FAILED;
    case CONNECT_SERVER_FAILED: return FAILED;
    case SET_PROXY_BLANKED_FAILED: return FAILED;
    case QUERY_EXEC_FAILED: return FAILED;
    case UNSUPPORTED_TYPE: return FAILED;

    }
    return SUCCESS;

}
RetVal CPUInfo::setCPUCodeName() {


}
RetVal CPUInfo::setCPUMaxClockSpeed() {


}
RetVal CPUInfo::setCPUCurrentClockSpeed() {


}
RetVal CPUInfo::setCPUL2Cache() {


}
RetVal CPUInfo::setCPUL3Cache() {


}
RetVal CPUInfo::setCPUArchitecture() {


}

RetVal CPUInfo::setCPULoadPercentage() {


}
RetVal CPUInfo::setCPUVoltageCapacity() {


}


std::wstring CPUInfo::getCPUManufacturer() {
    return cpuManufacturer;
}

std::wstring CPUInfo::getCPUName() {
    return cpuName;
}

std::wstring CPUInfo::getCPUNumberOfCores() {
    return cpuNumberOfCores;
}

std::wstring CPUInfo::getCPUNumberOfThreads() {
    return cpuNumberOfThreads;
}


