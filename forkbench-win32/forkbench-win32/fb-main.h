#pragma once
#include "fetch-details.h"
// The window procedure function prototype
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void UpdateSystemInfo(CPUInfo& info);