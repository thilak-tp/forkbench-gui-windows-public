#pragma once
#include "fetch-details.h"
// The window procedure function prototype
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void updateCPUInfo(CPUInfo& info);
void updateSystemMemoryInfo(SystemMemoryInfo& info);
void updateGPUInfo(GPUandDisplayInfo& gpuInfo);
void populateClassObjects();
void initFonts();
void initBitmaps();
void displayWindows(HWND hwnd);
void updateFonts();
void paintBitmaps(HWND hwnd);
