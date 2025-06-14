#include "fetch-details.h"
std::wstring decodeProcessorArchitecture(std::wstring architectureCodeInput) {
    int code = std::stoi(architectureCodeInput);
    switch (code) {
        case 0: return L"x86";
        case 1: return L"MIPS";
        case 2: return L"Alpha";
        case 3: return L"PowerPC";
        case 5: return L"ARM32";
        case 6: return L"IA-64";
        case 9: return L"x64";
        case 12: return L"ARM64";
        default: return L"Unknown";
        
    }
}
RetVal CPUInfo::getL1CacheSize(std::wstring& result) {
    HRESULT hres;
    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;
    IEnumWbemClassObject* pEnumerator = nullptr;
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) return COM_INIT_FAILED;

    hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);
    if (FAILED(hres)) {
        CoUninitialize();
        return SEC_INIT_FAILED;
    }

    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) {
        CoUninitialize();
        return WLOCATOR_CREATION_FAILED;
    }

    hres = pLoc->ConnectServer(
        SysAllocString(L"ROOT\\CIMV2"), nullptr, nullptr, 0, NULL, 0, 0, &pSvc);
    if (FAILED(hres)) {
        pLoc->Release();
        CoUninitialize();
        return CONNECT_SERVER_FAILED;
    }

    hres = pSvc->ExecQuery(
        SysAllocString(L"WQL"),
        SysAllocString(L"SELECT InstalledSize, Level FROM Win32_CacheMemory WHERE Level = 3"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator);
    if (FAILED(hres)) {
        if (pSvc) pSvc->Release();
        if (pLoc) pLoc->Release();
        CoUninitialize();
        return QUERY_EXEC_FAILED;
    }

    while (pEnumerator && pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK) {
        if (pclsObj) {
            VARIANT vtSize;
            VariantInit(&vtSize);
            hres = pclsObj->Get(L"InstalledSize", 0, &vtSize, 0, 0);
            if (SUCCEEDED(hres) && vtSize.vt == VT_I4) {
                result += std::to_wstring(vtSize.intVal) ;
            }
            VariantClear(&vtSize);
            pclsObj->Release();
            pclsObj = nullptr;
        }
    }

    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();

    //result = result.empty() ? L"L1 Cache not found.\n" : result;
    return SUCCESS;
}

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
    gpuDriverDate = L"";
    gpuAdapterCompatibility = L""; // Manufacturer
    gpuVideoMemoryType = L"";
    gpuVideoArchitecture = L"";
}

CPUInfo::CPUInfo() {
    cpuName = L"";
    cpuCodeName = L"";
    cpuNumberOfCores = L"";
    cpuNumberOfThreads = L"";
    cpuMaxClockSpeed = L"";
    cpuCurrentClockSpeed = L"";
    cpuL2Cache = L"";
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

RetVal CPUInfo::setCPUMaxClockSpeed() {

    RetVal ret = GetWMIInfo(L"Win32_Processor", L"MaxClockSpeed", cpuMaxClockSpeed);
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
RetVal CPUInfo::setCPUCurrentClockSpeed() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"CurrentClockSpeed", cpuCurrentClockSpeed);
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
RetVal CPUInfo::setCPUL1Cache() {
    RetVal ret = getL1CacheSize(cpuL1Cache);
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
RetVal CPUInfo::setCPUL2Cache() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"L2CacheSize", cpuL2Cache);
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
RetVal CPUInfo::setCPUL3Cache() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"L3CacheSize", cpuL3Cache);
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
RetVal CPUInfo::setCPUArchitecture() {
    RetVal ret = GetWMIInfo(L"Win32_Processor", L"Architecture", cpuArchitecture);
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

std::wstring CPUInfo::getCPUMaxClockSpeed() {
    return cpuMaxClockSpeed;
}

std::wstring CPUInfo::getCPUCurrentClockSpeed() {
    return cpuCurrentClockSpeed;
}
std::wstring CPUInfo::getCPUL1Cache() {
    return cpuL1Cache;
}
std::wstring CPUInfo::getCPUL2Cache() {
    return cpuL2Cache;
}

std::wstring CPUInfo::getCPUL3Cache() {
    return cpuL3Cache;
}

std::wstring CPUInfo::getCPUArchitecture() {
    return cpuArchitecture;
}


// System Memory Info Implementation
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
RetVal SystemMemoryInfo::settotalPhysicalMemory() {
    RetVal ret = GetWMIInfo(L"Win32_ComputerSystem", L"TotalPhysicalMemory", totalPhysicalMemory);
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
RetVal SystemMemoryInfo::setRAMCapacity() {
    RetVal ret = GetWMIInfo(L"Win32_PhysicalMemory", L"Capacity", RAMCapacity);
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

RetVal SystemMemoryInfo::setRAMSpeed() {
    RetVal ret = GetWMIInfo(L"Win32_PhysicalMemory", L"Speed", RAMSpeed);
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

RetVal SystemMemoryInfo::setManufacturer() {
    RetVal ret = GetWMIInfo(L"Win32_PhysicalMemory", L"Manufacturer", RAMManufacturer);
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

RetVal SystemMemoryInfo::setRAMType() {
   
}



RetVal SystemMemoryInfo::setFormFactor() {

}


RetVal SystemMemoryInfo::setRAMPartNumber() {

}

RetVal SystemMemoryInfo::setRAMSerialNumber() {

}



std::wstring SystemMemoryInfo::gettotalPhysicalMemory() {
    return totalPhysicalMemory;
}
std::wstring SystemMemoryInfo::getRAMCapacity() {
    return RAMCapacity;
}
std::wstring SystemMemoryInfo::getRAMSpeed() {
    return RAMSpeed;
}
std::wstring SystemMemoryInfo::getRAMManufacturer() {
    return RAMManufacturer;
}


// GPU Implementations
RetVal GPUandDisplayInfo::setgpuName(){
    RetVal ret = GetWMIInfo(L"Win32_VideoController", L"Name", gpuName);
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
RetVal GPUandDisplayInfo::setgpuDriverVer() {
    RetVal ret = GetWMIInfo(L"Win32_VideoController", L"DriverVersion", gpuDriverVer);
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

RetVal GPUandDisplayInfo::setgpuAdapterCompatibility() {
    RetVal ret = GetWMIInfo(L"Win32_VideoController", L"AdapterCompatibility", gpuAdapterCompatibility);
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
RetVal GPUandDisplayInfo::setgpuDriverDate() {
    RetVal ret = GetWMIInfo(L"Win32_VideoController", L"DriverDate", gpuDriverDate);
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

RetVal GPUandDisplayInfo::setgpuTotalVRAM() {
    GetVRAM_DirectX(gpuTotalVRAM);
    return SUCCESS;
}
RetVal GPUandDisplayInfo::setgpuVideoMemoryType() {

}
RetVal GPUandDisplayInfo::setgpuVideoArchitecture() {

}
RetVal GPUandDisplayInfo::setgpuVideoModeDescription() {

}
RetVal GPUandDisplayInfo::setgpuCurrentRefreshRate() {

}
RetVal GPUandDisplayInfo::setgpuCurrentHResolution() {

}
RetVal GPUandDisplayInfo::setgpuCurrentVResolution() {

}
RetVal GPUandDisplayInfo::setgpuMonitorType() {

}
RetVal GPUandDisplayInfo::setgpuScreenHeight() {

}
RetVal GPUandDisplayInfo::setgpuScreenWidth() {

}

std::wstring GPUandDisplayInfo::getgpuName() {
    return gpuName;
}

std::wstring GPUandDisplayInfo::getgpuAdapterCompatibility() {
    return gpuAdapterCompatibility;
}

std::wstring GPUandDisplayInfo::getgpuDriverVer() {
    return gpuDriverVer;
}

std::wstring GPUandDisplayInfo::getgpuDriverDate() {
    return gpuDriverDate;
}

std::wstring GPUandDisplayInfo::getgpuTotalVRAM() {
    return gpuTotalVRAM;
}

// Operating System 

RetVal OSInfo::setosName() {
    RetVal ret = GetWMIInfo(L"Win32_OperatingSystem", L"Caption", osName);
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

RetVal OSInfo::setosVersion() {
    RetVal ret = GetWMIInfo(L"Win32_OperatingSystem", L"Version", osVersion);
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

RetVal OSInfo::setosBuildNumber() {
    RetVal ret = GetWMIInfo(L"Win32_OperatingSystem", L"BuildNumber", osBuildNumber);
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

RetVal OSInfo::setOSArchitecture() {
    RetVal ret = GetWMIInfo(L"Win32_OperatingSystem", L"OSArchitecture", OSArchitecture);
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

RetVal OSInfo::setosSystemDrive() {
    RetVal ret = GetWMIInfo(L"Win32_OperatingSystem", L"SystemDrive", osSystemDrive);
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


std::wstring OSInfo::getosName() {
    return osName;
}

std::wstring OSInfo::getosVersion() {
    return osVersion;
}

std::wstring OSInfo::getosBuildNumber() {
    return osBuildNumber;
}

std::wstring OSInfo::getosArchitecture() {
    return OSArchitecture;
}
std::wstring OSInfo::getosSystemDrive() {
    return osSystemDrive;
}







//RetVal GetVRAM_DirectX(std::wstring& result) {
//    IDXGIFactory4* pFactory = nullptr;
//    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pFactory)))) {
//        return FAILED;
//    }
//
//    IDXGIAdapter1* pAdapter1 = nullptr;
//    for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
//        IDXGIAdapter3* pAdapter3 = nullptr;
//        if (SUCCEEDED(pAdapter1->QueryInterface(IID_PPV_ARGS(&pAdapter3)))) {
//            DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {};
//            if (SUCCEEDED(pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo))) {
//                double totalVRAMGB = static_cast<double>(memInfo.Budget) / (1024 * 1024 * 1024);
//
//                wchar_t buffer[32];
//                swprintf_s(buffer, L"%.2f GB", totalVRAMGB);
//                result = buffer;
//
//                pAdapter3->Release();
//                pAdapter1->Release();
//                pFactory->Release();
//                return SUCCESS;
//            }
//            pAdapter3->Release();
//        }
//        pAdapter1->Release();
//    }
//
//    pFactory->Release();
//    return FAILED;
//}

RetVal GetVRAM_DirectX(std::wstring& result) {
    IDXGIFactory* pFactory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
        return FAILED;
    }

    IDXGIAdapter* pAdapter = nullptr;
    for (UINT i = 1; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC adapterDesc;
        if (SUCCEEDED(pAdapter->GetDesc(&adapterDesc))) {
            SIZE_T vramBytes = adapterDesc.DedicatedVideoMemory;
            double vramGB = static_cast<double>(vramBytes) / (1024 * 1024 * 1024);

            wchar_t buffer[32];
            swprintf_s(buffer, L"%.2f GB", vramGB);
            result = buffer;

            pAdapter->Release();
            pFactory->Release();
            return SUCCESS;  // Return early after first GPU
        }
        pAdapter->Release();
    }

    pFactory->Release();
    return FAILED;
}
