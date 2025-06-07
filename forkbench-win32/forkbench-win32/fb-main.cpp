// Implicit headerfiles
#include <windows.h>
#include <tchar.h>
#include <string>

// Explicit headerfiles
#include "resource.h"
#include "fb-main.h"
#include "fetch-details.h"

HINSTANCE hInst;
HWND hCPUStatic, hCoreStatic, hRAMStatic, hThreadStatic, hCPUManufacturerStatic;
HBITMAP hBitmap;

// This is the entry point of the application. It creates a window and registers a class for it.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"Forkbench";
	
	// Populate the WNDCLASSEX structure
	WNDCLASSEX wc = {};
	wc.hbrBackground = CreateSolidBrush(RGB(64, 64, 64));
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON2));
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
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, className, WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, 1000, 900, NULL, NULL, hInstance, NULL);
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
	CPUInfo cpuInfo;
	cpuInfo.setCPUName();
	cpuInfo.setCPUManufacturer();
	cpuInfo.setCPUNumberOfCores();
	cpuInfo.setCPUNumberOfThreads();
	switch (msg)
	{
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
		    hBitmap = (HBITMAP)LoadImageW(
			GetModuleHandleW(NULL),
			MAKEINTRESOURCE(IDB_BITMAP2),
			IMAGE_BITMAP,
			0, 0,
			LR_DEFAULTCOLOR);

		hCPUStatic = CreateWindowW(L"STATIC", L"CPU: Loading...",WS_CHILD | WS_VISIBLE, 20, 30, 600, 25, hwnd, NULL, hInst, NULL);
		hCoreStatic = CreateWindowW(L"STATIC", L"Cores: Loading...",WS_CHILD | WS_VISIBLE, 20, 60, 600, 25, hwnd, NULL, hInst, NULL);
		hRAMStatic = CreateWindowW(L"STATIC", L"RAM: Loading...",WS_CHILD | WS_VISIBLE, 20, 90, 600, 25, hwnd, NULL, hInst, NULL);
		hThreadStatic = CreateWindowW(L"STATIC", L"CPU: Loading...", WS_CHILD | WS_VISIBLE, 20, 120, 600, 25, hwnd, NULL, hInst, NULL);
		hCPUManufacturerStatic = CreateWindowW(L"STATIC", L"CPU: Loading...", WS_CHILD | WS_VISIBLE, 20, 150, 600, 25, hwnd, NULL, hInst, NULL);
		break;
	}
	case WM_PAINT: {
		UpdateSystemInfo(cpuInfo);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		
		if (hBitmap) {
			HDC hMemDC = CreateCompatibleDC(hdc);
			SelectObject(hMemDC, hBitmap);

			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			BitBlt(hdc, 650, 20, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			DeleteDC(hMemDC);
		}

		EndPaint(hwnd, &ps);
		}
		return 0;

		
	case WM_DESTROY:
		// Called when the window is being destroyed
		PostQuitMessage(0);  // Exit message loop
		return 0;

	case WM_SIZE:
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


	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
 }
void UpdateSystemInfo(CPUInfo& info) {
	std::wstring cpuName = info.getCPUName();
	std::wstring cores = info.getCPUNumberOfCores();
	std::wstring threads = info.getCPUNumberOfThreads();
	std::wstring cpuManufacturer = info.getCPUManufacturer();

	if (!cpuName.empty()) SetWindowTextW(hCPUStatic, (L"CPU: " + cpuName).c_str());
	if (!cores.empty()) SetWindowTextW(hCoreStatic, (L"Cores: " + cores).c_str());
	if (!threads.empty()) SetWindowTextW(hThreadStatic, (L"Threads: " + threads).c_str());
	if (!cpuManufacturer.empty()) SetWindowTextW(hCPUManufacturerStatic, (L"Manufacturer: " + cpuManufacturer).c_str());

	
}