#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "fb-main.h"
#include <string>
#include "fetch-details.h"
#include "minwindef.h"
// This is the entry point of the application. It creates a window and registers a class for it.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"Forkbench";
	
	// Populate the WNDCLASSEX structure
	WNDCLASSEX wc = {};
	wc.hbrBackground = CreateSolidBrush(RGB(64, 64, 64));
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.lpszClassName = className;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	
	// The above class is resigtered
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("Window Class Registration Failed"), _T("Error"), NULL);
		return FAILURE;
	}
	// This creates the actual visible window.
	//HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, className, WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 900, NULL, NULL, hInstance, NULL);
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, className, WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, 1000, 900, NULL, NULL, hInstance, NULL);
	if (!hwnd) {
		MessageBox(NULL, _T("WIndows Creation Failed"), _T("Error"), NULL);
		return FAILURE;
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
	/*
	static int scrollPosX = 0;
    static int scrollPosY = 0;
    static int maxScrollX = 1000; // max scroll range horizontally
    static int maxScrollY = 1000; // max scroll range vertically
   */
	switch (msg)
	{
		// To handle the dragging of the widnow without the title bar
	case WM_LBUTTONDOWN:
		ReleaseCapture();
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	case WM_CLOSE:
		// When user tries to close the window
		if (MessageBox(hwnd, L"Are you sure you want to exit?", L"Exit", MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			DestroyWindow(hwnd);  // Proceed with closing
		}
		return 0;
		/*
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// Optional: Create custom font
		HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
		// Fetching CPU details using WMI
		std::wstring cpuName = GetWMIInfo(L"Win32_Processor", L"Name");
		std::wstring cpuCores = GetWMIInfo(L"Win32_Processor", L"NumberOfCores");
		std::wstring cpuThreads = GetWMIInfo(L"Win32_Processor", L"ThreadCount");
		std::wstring cpuCodename = GetWMIInfo(L"Win32_Processor", L"Caption");
		// Motherboard and BIOS details
		std::wstring mobo = GetWMIInfo(L"Win32_BaseBoard", L"Product");
		std::wstring bios = GetWMIInfo(L"Win32_BIOS", L"SMBIOSBIOSVersion");
		// RAM details
		std::wstring ramSize = GetWMIInfo(L"Win32_ComputerSystem", L"TotalPhysicalMemory");
		std::wstring ramType = GetWMIInfo(L"Win32_PhysicalMemory", L"MemoryType");
		std::wstring ramSpeed = GetWMIInfo(L"Win32_PhysicalMemory", L"Speed");
		// GPU details
		std::wstring gpuName = GetWMIInfo(L"Win32_VideoController", L"Name");
		std::wstring gpuDriver = GetWMIInfo(L"Win32_VideoController", L"DriverVersion");
		std::wstring vram = GetWMIInfo(L"Win32_VideoController", L"AdapterRAM");
		// OS details
		std::wstring osVersion = GetWMIInfo(L"Win32_OperatingSystem", L"Caption");
		std::wstring build = GetWMIInfo(L"Win32_OperatingSystem", L"BuildNumber");
		// Storage details
		std::wstring diskSize = GetWMIInfo(L"Win32_DiskDrive", L"Size");
		std::wstring diskType = GetWMIInfo(L"Win32_DiskDrive", L"MediaType");





		RECT rect;
		GetClientRect(hwnd, &rect);
		TextOutW(hdc, 20, 20, cpuName.c_str(), (int)cpuName.length()); break;
		DrawTextW(hdc, cpuName.c_str(), -1, &rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		SelectObject(hdc, oldFont);
		DeleteObject(hFont);
		EndPaint(hwnd, &ps);
		return 0;
	
	}*/
	/*case WM_VSCROLL:
	{
		int yPos = scrollPosY;
		switch (LOWORD(wparam))
		{
		case SB_LINEUP:    yPos -= 10; break;
		case SB_LINEDOWN:  yPos += 10; break;
		case SB_PAGEUP:    yPos -= 50; break;
		case SB_PAGEDOWN:  yPos += 50; break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			yPos = HIWORD(wparam);
			break;
		}
		if (yPos < 0) yPos = 0;
		if (yPos > maxScrollY) yPos = maxScrollY;
		if (yPos != scrollPosY)
		{
			scrollPosY = yPos;
			ScrollWindow(hwnd, 0, scrollPosY - yPos, NULL, NULL);
			SetScrollPos(hwnd, SB_VERT, scrollPosY, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	}
	case WM_HSCROLL:
	{
		int xPos = scrollPosX;
		switch (LOWORD(wparam))
		{
		case SB_LINELEFT:  xPos -= 10; break;
		case SB_LINERIGHT: xPos += 10; break;
		case SB_PAGELEFT:  xPos -= 50; break;
		case SB_PAGERIGHT: xPos += 50; break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			xPos = HIWORD(wparam);
			break;
		}
		if (xPos < 0) xPos = 0;
		if (xPos > maxScrollX) xPos = maxScrollX;
		if (xPos != scrollPosX)
		{
			scrollPosX = xPos;
			ScrollWindow(hwnd, scrollPosX - xPos, 0, NULL, NULL);
			SetScrollPos(hwnd, SB_HORZ, scrollPosX, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	
	case WM_CREATE:
		// Initialize scrollbar range and position
		SetScrollRange(hwnd, SB_VERT, 0, maxScrollY, TRUE);
		SetScrollPos(hwnd, SB_VERT, scrollPosY, TRUE);
		SetScrollRange(hwnd, SB_HORZ, 0, maxScrollX, TRUE);
		SetScrollPos(hwnd, SB_HORZ, scrollPosX, TRUE);
		break;
		}*/
	case WM_PAINT: {
		collectWMIInfo(hwnd);
	} return 0;

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
