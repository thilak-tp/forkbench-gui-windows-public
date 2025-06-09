#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cmath>
#include <windows.h>

DWORD WINAPI BenchmarkCPUThread(LPVOID lpParam);
int forkBenchScore(int latencyMs);