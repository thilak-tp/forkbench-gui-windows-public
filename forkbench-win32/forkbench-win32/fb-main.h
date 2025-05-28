#pragma once
enum RetVal {
	SUCCESS,
	FAILURE
};
// The window procedure function prototype
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);