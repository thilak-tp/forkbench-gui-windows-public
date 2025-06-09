// Implicit headerfiles
#include <windows.h>
#include <tchar.h>
#include <string>

// Explicit headerfiles
#include "resource.h"
#include "fb-main.h"
#include "fetch-details.h"
#include "cpu-benchmark.h"

HBRUSH hbrTransparent = (HBRUSH)GetStockObject(HOLLOW_BRUSH);  // Global or static
HINSTANCE hInst;
HWND hCPUStatic, hCoreStatic, hRAMStatic, hThreadStatic, hCPUManufacturerStatic, hCPUMaxClockSpeed, hCPUCurrentClockSpeed, hCPUL1Cache, hCPUL2Cache, hCPUL3Cache, hArchitecture, hDisplayCore, hDisplayCPUInformation, hDisplaySystemMemInfo;
// For System Memory specifications
HWND hTotalPhysicalMem,hUsablePhysicalMem, hRAMSpeed, hRAMManufacturer ;
// For GPU Specifications
HWND hDisplayGPUInfo,hGPUName, hGPUAdapterCompatibility, hGPUDriverVer, hGPUDriverDate, hGPUVRAM;

//For OS Specifications
HWND hDisplayOSInfo,hOSName,hOSArch, hOSbuild, hOSVersion, hOSSysDrive;


// For Forkbench
HWND hResult;
// Window Button Handlers
HWND hCloseBtn, hMinBtn, hBenchmarkBtn;
HBITMAP hBitmapCPU, hBitmapRAM, hBitmapGPU, hBitmapOS;
HFONT hFontDetails, hFontDisplayCore, hFontTitles, hFontBench;

CPUInfo cpuInfo;
//cpuInfo.setCPUL1Cache();
SystemMemoryInfo memInfo;
//GPU
GPUandDisplayInfo gpuInfo;
OSInfo osInfo;

// This is the entry point of the application. It creates a window and registers a class for it.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"Forkbench";
	populateClassObjects();
	initFonts();
	initBitmaps();

	// Populate the WNDCLASSEX structure
	WNDCLASSEX wc = {};
	wc.hbrBackground = CreateSolidBrush(RGB(64, 64, 64));
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON3));
	wc.lpszClassName = className;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	
	// The above class is resigtered
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("Window Class Registration Failed"), _T("Error"), NULL);
		return FAILED;
	}
	// This creates the actual visible window.
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, className, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 450, 600, NULL, NULL, hInstance, NULL);
	if (!hwnd) {
		MessageBox(NULL, _T("WIndows Creation Failed"), _T("Error"), NULL);
		return FAILED;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Message loop to handle messages that windows sends
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0; // Exit the application

}

// A window procedure function catches messages from the windows so that we can handle these messages appropriately
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	

	switch (msg)
	{
		

	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wparam;
		HWND hStatic = (HWND)lparam;

		if (hStatic == hDisplayCore) {
			SetBkMode(hdcStatic, TRANSPARENT);                   // Transparent background
			SetTextColor(hdcStatic, RGB(0, 0, 0));          // Optional: white text
			return (INT_PTR)hbrTransparent;                       // Use null brush
		}
	}
	break;

		// To handle the dragging of the widnow without the title bar
	case WM_LBUTTONDOWN:
		ReleaseCapture();
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return 0;
	
	

	case WM_CLOSE:
		// When user tries to close the window
		if (MessageBox(hwnd, L"Are you sure you want to exit?", L"Exit", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			DestroyWindow(hwnd);  // Proceed with closing
		}
		return 0;
	case WM_CREATE: {

		displayWindows(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
		updateFonts();
		windowButtons(hwnd);
		

		break;
	}
	case WM_COMMAND:
		if (LOWORD(wparam) == 103) {
			CreateThread(NULL, 0, BenchmarkCPUThread, hwnd, 0, NULL);
			forkScore =forkBenchScore(resultLatency);
			std::wstring forkS = std::to_wstring(forkScore);
			if (!forkS.empty()) SetWindowTextW(hResult, (L"ForkScore: " + forkS).c_str());
			
			break;
		}
		break;

	
	case WM_PAINT: {

		updateCPUInfo(cpuInfo);
		updateSystemMemoryInfo(memInfo);
		updateGPUInfo(gpuInfo);
		updateOSInfo(osInfo);
		paintBitmaps(hwnd);
		
	}
		return 0;

		
	case WM_DESTROY:
		// Called when the window is being destroyed
		PostQuitMessage(0);  // Exit message loop
		return 0;

	case WM_SIZE: {
		/*RECT rc;
		GetClientRect(hwnd, &rc);
		HRGN hRgn = CreateRoundRectRgn(0, 0, rc.right, rc.bottom, 20, 20);
		SetWindowRgn(hwnd, hRgn, TRUE);*/
		// Called when the window is resized (including maximize or minimize)
		switch (wparam) {
		case SIZE_MINIMIZED:
			OutputDebugString(L"Window Minimized\n");
			break;
		case SIZE_MAXIMIZED:
			OutputDebugString(L"Window Maximized\n");
			break;
		case SIZE_RESTORED:
			OutputDebugString(L"Window Restored\n");
			break;
		}
		return 0;
	}

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
 }
void updateOSInfo(OSInfo& osInfo) {

	std::wstring osName = osInfo.getosName();
	std::wstring osArch = osInfo.getosArchitecture();
	std::wstring osBuild = osInfo.getosBuildNumber();
	std::wstring osVersion = osInfo.getosVersion();
	std::wstring osSysDrive = osInfo.getosSystemDrive();
	//hOSName,hOSArch, hOSbuild, hOSVersion, hOSSysDrive;
	if (!osName.empty()) SetWindowTextW(hOSName, (L"OS: " + osName).c_str());
	if (!osArch.empty()) SetWindowTextW(hOSArch, (L"Architecture: " + osArch).c_str());
	if (!osBuild.empty()) SetWindowTextW(hOSbuild, (L"Build: " + osBuild).c_str());
	if (!osVersion.empty()) SetWindowTextW(hOSVersion, (L"Version " + osVersion).c_str());
	if (!osSysDrive.empty()) SetWindowTextW(hOSSysDrive, (L"OS Drive: " + osSysDrive).c_str());

}
void updateGPUInfo(GPUandDisplayInfo& gpuInfo) {
	std::wstring gpuName = gpuInfo.getgpuName();
	std::wstring gpuManufacturer = gpuInfo.getgpuAdapterCompatibility();
	std::wstring gpuDriverVer = gpuInfo.getgpuDriverVer();
	std::wstring gpuDriverDate = gpuInfo.getgpuDriverDate();
	std::wstring gpuVRAM = gpuInfo.getgpuTotalVRAM();


	if (!gpuName.empty()) SetWindowTextW(hGPUName, (L"GPU: " + gpuName).c_str());
	if (!gpuManufacturer.empty()) SetWindowTextW(hGPUAdapterCompatibility, (L"Manufacturer: " + gpuManufacturer).c_str());
	if (!gpuDriverVer.empty()) SetWindowTextW(hGPUDriverVer, (L"Driver Version: " + gpuDriverVer).c_str());
	if (!gpuDriverDate.empty()) SetWindowTextW(hGPUDriverDate, (L"Driver Date " + gpuDriverDate).c_str());
	if (!gpuVRAM.empty()) SetWindowTextW(hGPUVRAM, (L"Usable VRAM: " + gpuVRAM).c_str());
}
void updateSystemMemoryInfo(SystemMemoryInfo& info) {
	std::wstring totalRam = info.getRAMCapacity();
	std::wstring usableRam = info.gettotalPhysicalMemory();
	float iTotalRAMGB = std::stof(totalRam);
	float iUsableRAMGB = std::stof(usableRam);
	std::wstring RAMSpeed = info.getRAMSpeed() + L" MHz";
	std::wstring RAMManufacturer = info.getRAMManufacturer();

	

	iTotalRAMGB = iTotalRAMGB / 1024 / 1024 / 1024;
	iUsableRAMGB = iUsableRAMGB / 1024 / 1024 / 1024;
	//.std::swprintf(totalRam, totalRam.length(), L"%.2f", iTotalRAMGB);
	//std::wstring totalRamGB = std::to_wstring(iTotalRAMGB) + L" GB";
	//std::wstring usableRamGB = std::to_wstring(iUsableRAMGB) + L" GB";
	wchar_t bufferTotal[64];
	std::swprintf(bufferTotal, sizeof(bufferTotal) / sizeof(wchar_t), L"%.2f", iTotalRAMGB);
	std::wstring totalRam1 = bufferTotal;
	wchar_t bufferUsable[64];
	std::swprintf(bufferUsable, sizeof(bufferUsable) / sizeof(wchar_t), L"%.2f", iUsableRAMGB);
	std::wstring usableRam1 = bufferUsable;
	totalRam1 += L" GB";
	usableRam1 += L" GB";

	if (!totalRam1.empty()) SetWindowTextW(hTotalPhysicalMem, (L"Total RAM: " + totalRam1).c_str());
	if (!usableRam1.empty()) SetWindowTextW(hUsablePhysicalMem, (L"Usable RAM: " + usableRam1).c_str());
	if (!RAMSpeed.empty()) SetWindowTextW(hRAMSpeed, (L"Speed: " + RAMSpeed).c_str());
	if (!RAMManufacturer.empty()) SetWindowTextW(hRAMManufacturer, (L"Manufacturer: " + RAMManufacturer).c_str());
}
void updateCPUInfo(CPUInfo& info) {
	std::wstring cpuName = info.getCPUName();
	std::wstring cpuNumberOfCores = info.getCPUNumberOfCores();
	std::wstring cpuNumberOfThreads = info.getCPUNumberOfThreads();
	std::wstring cpuManufacturer = info.getCPUManufacturer();
	std::wstring cpuMaxClockSpeed = info.getCPUMaxClockSpeed();
	std::wstring cpuCurrentClockSpeed = info.getCPUCurrentClockSpeed();
	//std::wstring cpuL1Cache = info.getCPUL1Cache();
	std::wstring cpuL2Cache = info.getCPUL2Cache();
	std::wstring cpuL3Cache = info.getCPUL3Cache();
	std::wstring cpuArchitecture = decodeProcessorArchitecture(info.getCPUArchitecture());
	
	
	//float cpuL1CacheMB = std::stoi(cpuL1Cache);
	//cpuL1CacheMB = cpuL1CacheMB / 1024;
	int cpuL2CacheMB = std::stoi(cpuL2Cache);
	cpuL2CacheMB = cpuL2CacheMB / 1024;
	int cpuL3CacheMB = std::stoi(cpuL3Cache);
	cpuL3CacheMB = cpuL3CacheMB / 1024;
	float cpuMaxGHZ = std::stof(cpuMaxClockSpeed.c_str());
	cpuMaxGHZ /= 1000.0f; // Convert to GHz
	cpuMaxGHZ = static_cast<float>(static_cast<int>(cpuMaxGHZ * 100)) / 100; // Round to 2 decimal places
	float cpuCurrentGHZ = std::stoi(cpuCurrentClockSpeed.c_str());
	cpuCurrentGHZ /= 1000.0f; // Convert to GHz
	cpuCurrentGHZ = static_cast<float>(static_cast<int>(cpuCurrentGHZ * 100)) / 100; // Round to 2 decimal places
	//std::wstring cpuMaxClockSpeedGHZ = std::to_wstring(cpuMaxGHZ) + L" GHz";
	std::wstring cpuCurrentClockSpeedGHZ = std::to_wstring(cpuCurrentGHZ) + L" GHz";
	wchar_t bufferMaxClockSpeed[64];
	std::swprintf(bufferMaxClockSpeed, sizeof(bufferMaxClockSpeed) / sizeof(wchar_t), L"%.2f", cpuCurrentGHZ);
	std::wstring maxClockSpeedGHZ = bufferMaxClockSpeed;


	//std::wstring L1CacheMB = std::to_wstring(cpuL1CacheMB) + L" MB";
	std::wstring L2CacheMB = std::to_wstring(cpuL2CacheMB) + L" MB";
	std::wstring L3CacheMB = std::to_wstring(cpuL3CacheMB) + L" MB";
	if (!cpuName.empty()) SetWindowTextW(hCPUStatic, (L"CPU: " + cpuName).c_str());
	if (!cpuNumberOfCores.empty()) SetWindowTextW(hCoreStatic, (L"Cores: " + cpuNumberOfCores).c_str());
	if (!cpuNumberOfThreads.empty()) SetWindowTextW(hThreadStatic, (L"Threads: " + cpuNumberOfThreads).c_str());
	if (!cpuManufacturer.empty()) SetWindowTextW(hCPUManufacturerStatic, (L"Manufacturer: " + cpuManufacturer).c_str());
	if (!maxClockSpeedGHZ.empty()) SetWindowTextW(hCPUMaxClockSpeed, (L"Max Clock Speed: "  + maxClockSpeedGHZ + L" GHz").c_str());
	//if (!cpuCurrentClockSpeedGHZ.empty()) SetWindowTextW(hCPUCurrentClockSpeed, (L"Curent Clock Speed: " + cpuCurrentClockSpeed + L" ( " + cpuCurrentClockSpeedGHZ + L" )").c_str());
	//if (!L2CacheMB.empty()) SetWindowTextW(hCPUL2Cache, (L"L2 Cache: " + L2CacheMB).c_str());
	if (!L2CacheMB.empty()) SetWindowTextW(hCPUL2Cache, (L"L2 Cache: " + L2CacheMB).c_str());
	if (!L3CacheMB.empty()) SetWindowTextW(hCPUL3Cache, (L"L3 Cache: " + L3CacheMB).c_str());
	if (!cpuArchitecture.empty()) SetWindowTextW(hArchitecture, (L"Architecture: " + cpuArchitecture).c_str());
	if (!cpuNumberOfCores.empty()) SetWindowTextW(hDisplayCore, cpuNumberOfCores.c_str());

	
}

void populateClassObjects() {
	cpuInfo.setCPUName();
	cpuInfo.setCPUManufacturer();
	cpuInfo.setCPUNumberOfCores();
	cpuInfo.setCPUNumberOfThreads();
	cpuInfo.setCPUMaxClockSpeed();
	cpuInfo.setCPUCurrentClockSpeed();
	cpuInfo.setCPUL2Cache();
	cpuInfo.setCPUL3Cache();
	cpuInfo.setCPUArchitecture();
	memInfo.settotalPhysicalMemory();
	memInfo.setRAMCapacity();
	memInfo.setRAMSpeed();
	memInfo.setManufacturer();
	gpuInfo.setgpuName();
	gpuInfo.setgpuAdapterCompatibility();
	gpuInfo.setgpuDriverDate();
	gpuInfo.setgpuDriverVer();
	gpuInfo.setgpuTotalVRAM();
	osInfo.setosName();
	osInfo.setOSArchitecture();
	osInfo.setosVersion();
	osInfo.setosBuildNumber();
	osInfo.setosSystemDrive();

}

void initFonts() {

	hFontDetails = CreateFontW(16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
	
	hFontDisplayCore = CreateFontW(
		30, 0, 0, 0,
		FW_NORMAL,                   
		FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH,
		L"Segoe UI Semibold"
	);
	hFontTitles = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    hFontBench = CreateFontW(
		18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		VARIABLE_PITCH, L"Segoe UI");

}

void initBitmaps() {
	std::wstring vendor = gpuInfo.getgpuAdapterCompatibility();
	if (vendor.find(L"NVIDIA") != std::wstring::npos) {
		hBitmapGPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP8),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}
	else if (vendor.find(L"AMD") != std::wstring::npos) {
		hBitmapGPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP10),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}
	else if (vendor.find(L"Intel") != std::wstring::npos) {
		hBitmapGPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP9),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}
	else {
		hBitmapGPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP7),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);

	}
	if (cpuInfo.getCPUManufacturer() == L"Intel") {
		hBitmapCPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP4),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}
	else if (cpuInfo.getCPUManufacturer() == L"AMD") {
		hBitmapCPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP3),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}
	else {
		hBitmapCPU = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP5),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);
	}

	hBitmapRAM = (HBITMAP)LoadImageW(
		GetModuleHandleW(NULL),
		MAKEINTRESOURCE(IDB_BITMAP6),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTCOLOR);

	hBitmapOS = (HBITMAP)LoadImageW(
		GetModuleHandleW(NULL),
		MAKEINTRESOURCE(IDB_BITMAP11),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTCOLOR);
}

void displayWindows(HWND hwnd) {
	hDisplayCPUInformation = CreateWindowW(L"STATIC", L"CPU Specifications", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10, 410, 15, hwnd, NULL, hInst, NULL);
	hCPUStatic = CreateWindowW(L"STATIC", L"CPU: Loading...", WS_CHILD | WS_VISIBLE, 120, 30, 300, 15, hwnd, NULL, hInst, NULL);
	hCoreStatic = CreateWindowW(L"STATIC", L"Cores: Loading...", WS_CHILD | WS_VISIBLE, 120, 50, 140, 15, hwnd, NULL, hInst, NULL);
	//hRAMStatic = CreateWindowW(L"STATIC", L"RAM: Loading...",WS_CHILD | WS_VISIBLE, 20, 90, 600, 25, hwnd, NULL, hInst, NULL);
	hThreadStatic = CreateWindowW(L"STATIC", L"CPU: Loading...", WS_CHILD | WS_VISIBLE, 264, 50, 155, 15, hwnd, NULL, hInst, NULL);
	hCPUManufacturerStatic = CreateWindowW(L"STATIC", L"Manufacturer: Loading...", WS_CHILD | WS_VISIBLE, 120, 70, 140, 15, hwnd, NULL, hInst, NULL);
	hCPUMaxClockSpeed = CreateWindowW(L"STATIC", L"Max Clock Speed: Loading...", WS_CHILD | WS_VISIBLE, 120, 90, 300, 15, hwnd, NULL, hInst, NULL);
	//hCPUCurrentClockSpeed = CreateWindowW(L"STATIC", L"Current Clock Speed: Loading...", WS_CHILD | WS_VISIBLE, 100, 120, 300, 15, hwnd, NULL, hInst, NULL);
	//hCPUL1Cache = CreateWindowW(L"STATIC", L"L1 Cache: Loading...", WS_CHILD | WS_VISIBLE, 100, 100, 140, 15, hwnd, NULL, hInst, NULL);
	hCPUL2Cache = CreateWindowW(L"STATIC", L"L2 Cache: Loading...", WS_CHILD | WS_VISIBLE, 120, 110, 140, 15, hwnd, NULL, hInst, NULL);
	hCPUL3Cache = CreateWindowW(L"STATIC", L"L3 Cache: Loading...", WS_CHILD | WS_VISIBLE, 264, 110, 155, 15, hwnd, NULL, hInst, NULL);
	hArchitecture = CreateWindowW(L"STATIC", L"Architecture: Loading...", WS_CHILD | WS_VISIBLE, 264, 70, 155, 15, hwnd, NULL, hInst, NULL);
	hDisplayCore = CreateWindowW(L"STATIC", L"!", WS_CHILD | WS_VISIBLE, 75, 90, 30, 30, hwnd, NULL, hInst, NULL);


	//RAM Specs
	hDisplaySystemMemInfo = CreateWindowW(L"STATIC", L"System Memory Specifications", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 135, 410, 15, hwnd, NULL, hInst, NULL);
	hTotalPhysicalMem = CreateWindowW(L"STATIC", L"Total RAM: Loading...", WS_CHILD | WS_VISIBLE, 120, 155, 300, 15, hwnd, NULL, hInst, NULL);
	hUsablePhysicalMem = CreateWindowW(L"STATIC", L"Usable RAM: Loading...", WS_CHILD | WS_VISIBLE, 120, 175, 300, 15, hwnd, NULL, hInst, NULL);
	hRAMSpeed = CreateWindowW(L"STATIC", L"Speed: Loading...", WS_CHILD | WS_VISIBLE, 120, 195, 140, 15, hwnd, NULL, hInst, NULL);
	hRAMManufacturer = CreateWindowW(L"STATIC", L"Manufacturer: Loading...", WS_CHILD | WS_VISIBLE, 264, 195, 155, 15, hwnd, NULL, hInst, NULL);

	//GPU Specs
	hDisplayGPUInfo = CreateWindowW(L"STATIC", L"GPU Specifications", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 225, 410, 15, hwnd, NULL, hInst, NULL);
	hGPUName = CreateWindowW(L"STATIC", L"GPU: Loading...", WS_CHILD | WS_VISIBLE, 120, 245, 300, 15, hwnd, NULL, hInst, NULL);
	hGPUAdapterCompatibility = CreateWindowW(L"STATIC", L"Manufacturer: Loading...", WS_CHILD | WS_VISIBLE, 120, 265, 300, 15, hwnd, NULL, hInst, NULL);
	hGPUDriverVer = CreateWindowW(L"STATIC", L"Driver Version: Loading...", WS_CHILD | WS_VISIBLE, 120, 285, 300, 15, hwnd, NULL, hInst, NULL);
	hGPUDriverDate = CreateWindowW(L"STATIC", L"Driver Date: Loading...", WS_CHILD | WS_VISIBLE, 120, 305, 300, 15, hwnd, NULL, hInst, NULL);
	hGPUVRAM = CreateWindowW(L"STATIC", L"VRAM: Loading...", WS_CHILD | WS_VISIBLE, 120, 325, 300, 15, hwnd, NULL, hInst, NULL);

	//OS Specs 
	//hDisplayOSInfo,hOSName,hOSArch, hOSbuild, hOSVersion, hOSSysDrive;
	hDisplayOSInfo = CreateWindowW(L"STATIC", L"OS Specifications", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 350, 410, 15, hwnd, NULL, hInst, NULL);
	hOSName = CreateWindowW(L"STATIC", L"OS: Loading...", WS_CHILD | WS_VISIBLE, 120, 370, 300, 15, hwnd, NULL, hInst, NULL);
	hOSArch = CreateWindowW(L"STATIC", L"Architecture: Loading...", WS_CHILD | WS_VISIBLE, 120, 390, 300, 15, hwnd, NULL, hInst, NULL);
	hOSbuild = CreateWindowW(L"STATIC", L"Build: Loading...", WS_CHILD | WS_VISIBLE, 120, 410, 300, 15, hwnd, NULL, hInst, NULL);
	hOSVersion = CreateWindowW(L"STATIC", L"Version: Loading...", WS_CHILD | WS_VISIBLE, 120, 430, 300, 15, hwnd, NULL, hInst, NULL);
	hOSSysDrive = CreateWindowW(L"STATIC", L"OS Drive: Loading...", WS_CHILD | WS_VISIBLE, 120, 450, 300, 15, hwnd, NULL, hInst, NULL);

	// For Forkbench
	hResult = CreateWindowW(L"STATIC", L"Forkbench Score: <Click Forkbench to launch Benchmark>", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 490, 395, 22, hwnd, NULL, hInst, NULL);
}
void updateFonts() {

	// Apply font
	HWND controls[] = { hResult,hBenchmarkBtn, hOSSysDrive, hOSVersion,hOSbuild,hOSArch,hOSName,hGPUDriverDate,hGPUVRAM,hBenchmarkBtn,hGPUDriverVer,hCPUStatic,hGPUName, hGPUAdapterCompatibility,hCoreStatic, hThreadStatic, hCPUManufacturerStatic, hCPUMaxClockSpeed,
						hCPUCurrentClockSpeed, hCPUL2Cache, hCPUL3Cache, hArchitecture,hTotalPhysicalMem,hUsablePhysicalMem,hRAMSpeed, hRAMManufacturer };
	for (auto hwndCtrl : controls) {
		SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)hFontDetails, TRUE);
	}
	SendMessageW(hDisplayCore, WM_SETFONT, (WPARAM)hFontDisplayCore, TRUE);
	HWND titles[] = { hDisplayCPUInformation,hDisplayOSInfo,hDisplaySystemMemInfo,hDisplayGPUInfo };
	for (auto hwndCtrl : titles) {
		SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)hFontTitles, TRUE);
	}


	// Apply to a control
	SendMessageW(hResult, WM_SETFONT, (WPARAM)hFontBench, TRUE);
}


void paintBitmaps(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	if (hBitmapCPU) {
		BITMAP bitmap;
		GetObject(hBitmapCPU, sizeof(BITMAP), &bitmap);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapCPU);

		SetStretchBltMode(hdc, HALFTONE);

		int scaledWidth = bitmap.bmWidth / 6;
		int scaledHeight = bitmap.bmHeight / 6;

		StretchBlt(hdc, 20, 40, scaledWidth, scaledHeight, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);

		DeleteDC(hMemDC);
	}
	if (hBitmapRAM) {
		BITMAP bitmapRAM;
		GetObject(hBitmapRAM, sizeof(BITMAP), &bitmapRAM);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapRAM);

		SetStretchBltMode(hdc, HALFTONE);

		int scaledWidth = bitmapRAM.bmWidth / 6;
		int scaledHeight = bitmapRAM.bmHeight / 6;

		// Adjust position to avoid overlap with CPU image
		StretchBlt(hdc, 20, 165, scaledWidth, scaledHeight, hMemDC, 0, 0, bitmapRAM.bmWidth, bitmapRAM.bmHeight, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	if (hBitmapGPU) {
		BITMAP bitmapGPU;
		GetObject(hBitmapGPU, sizeof(BITMAP), &bitmapGPU);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapGPU);

		SetStretchBltMode(hdc, HALFTONE);

		int scaledWidth = bitmapGPU.bmWidth / 6;
		int scaledHeight = bitmapGPU.bmHeight / 6;

		// Adjust position to avoid overlap with CPU image
		StretchBlt(hdc, 20, 255, scaledWidth, scaledHeight, hMemDC, 0, 0, bitmapGPU.bmWidth, bitmapGPU.bmHeight, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);

	}
	if (hBitmapOS) {
		BITMAP bitmapOS;
		GetObject(hBitmapOS, sizeof(BITMAP), &bitmapOS);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapOS);

		SetStretchBltMode(hdc, HALFTONE);

		int scaledWidth = bitmapOS.bmWidth / 6;
		int scaledHeight = bitmapOS.bmHeight / 6;

		// Adjust position to avoid overlap with CPU image
		StretchBlt(hdc, 20, 375, scaledWidth, scaledHeight, hMemDC, 0, 0, bitmapOS.bmWidth, bitmapOS.bmHeight, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);

	}


	EndPaint(hwnd, &ps);

}

void windowButtons(HWND hwnd) {
	/*hCloseBtn = CreateWindowW(L"BUTTON", L"X", WS_CHILD | WS_VISIBLE,
		20, 300, 20, 20, hwnd, (HMENU)IDC_CLOSE_BUTTON, hInst, NULL);

	hMinBtn = CreateWindowW(L"BUTTON", L"_", WS_CHILD | WS_VISIBLE,
		20, 300, 20, 20, hwnd, (HMENU)IDC_MIN_BUTTON, hInst, NULL);*/

	hBenchmarkBtn = CreateWindowW(L"BUTTON", L"Forkbench",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20, 520, 400, 30, hwnd, (HMENU)IDC_BENCH_BUTTON, hInst, NULL);

	/*if (!hCloseBtn) {
		MessageBox(hwnd, L"Failed to create Close button", L"Error", MB_OK);
	}
	if (!hMinBtn) {
		MessageBox(hwnd, L"Failed to create Close button", L"Error", MB_OK);
	}*/
	if (!hBenchmarkBtn) {
		MessageBox(hwnd, L"Failed to create Close button", L"Error", MB_OK);
	}


}


int getPhysicalCoreCount() {
	DWORD len = 0;
	GetLogicalProcessorInformation(nullptr, &len);
	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
	if (!GetLogicalProcessorInformation(buffer.data(), &len)) {
		return -1; // Error
	}

	int physicalCores = 0;
	for (auto& info : buffer) {
		if (info.Relationship == RelationProcessorCore) {
			physicalCores++;
		}
	}
	return physicalCores;
}
