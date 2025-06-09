
#include "fetch-details.h"
#pragma once
#define IDC_CLOSE_BUTTON 101
#define IDC_MIN_BUTTON   102
#define IDC_BENCH_BUTTON 103


// The window procedure function prototype
extern int resultLatency, forkScore;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void updateCPUInfo(CPUInfo& info);
void updateSystemMemoryInfo(SystemMemoryInfo& info);
void updateGPUInfo(GPUandDisplayInfo& gpuInfo);
void updateOSInfo(OSInfo& osInfo);
void populateClassObjects();
void initFonts();
void initBitmaps();
void displayWindows(HWND hwnd);
void updateFonts();
void paintBitmaps(HWND hwnd);
void windowButtons(HWND hwnd);
int getPhysicalCoreCount();
