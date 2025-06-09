#include "cpu-benchmark.h"
#include "fb-main.h"


std::atomic<bool> stopBenchmark(false);
//DWORD WINAPI BenchmarkCPUThread(LPVOID lpParam) {
//    stopBenchmark = false;
//
//    CPUInfo info;
//    //int numThreads = std::stoi(info.getCPUNumberOfCores());
//    int numThreads = std::thread::hardware_concurrency();
//    //int numThreads = 1;
//    std::vector<std::thread> threads;
//
//    // Start timer
//    auto startTime = std::chrono::high_resolution_clock::now();
//
//    for (int i = 0; i < numThreads; ++i) {
//        threads.emplace_back([]() {
//            // CPU-heavy workload
//            volatile double x = 0.0001;
//            for (int j = 0; j < 50000000; ++j) {
//                x += std::sqrt(x * x + 1.23456789);  // expensive operation
//            }
//            });
//    }
//
//    for (auto& t : threads) t.join();
//
//    // Stop timer
//    auto endTime = std::chrono::high_resolution_clock::now();
//    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
//
//    // Show result in milliseconds
//    std::wstring message = L"CPU Benchmark completed in " + std::to_wstring(durationMs) + L" ms";
//    MessageBoxW((HWND)lpParam, message.c_str(), L"Benchmark Result", MB_OK);
//
//    return 0;
//}

DWORD WINAPI BenchmarkCPUThread(LPVOID lpParam) {
    // Let user choose mode
    /*int result = MessageBoxW((HWND)lpParam,
        L"Run CPU benchmark in:\n\nYes = Multicore\nNo = Single Core",
        L"Choose Benchmark Mode", MB_YESNO | MB_ICONQUESTION);

    bool useMulticore = (result == IDYES);

    stopBenchmark = false;
    int numThreads = useMulticore ? getPhysicalCoreCount() : 1;
    if (numThreads == 0) numThreads = 4;*/

    int numThreads = 1;
    const int workloadPerThread = 250000000;  // increase for longer run
    std::vector<std::thread> threads;

    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([=]() {
            volatile double x = 1.0;
            for (int j = 0; j < workloadPerThread; ++j) {
                // Heavy math: 6 floating point ops per iteration
                x = std::sin(x) * std::cos(x) + std::sqrt(x * x + 1.23456789);
            }
            });
    }

    for (auto& t : threads) t.join();

    // Stop timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    resultLatency = durationMs;
    //std::wstring mode = useMulticore ? L"Multicore" : L"Single-core";
   // std::wstring message = mode + L" benchmark completed in " + std::to_wstring(durationMs) + L" ms using "
     //   + std::to_wstring(numThreads) + L" thread(s).";

    //MessageBoxW((HWND)lpParam, message.c_str(), L"Benchmark Result", MB_OK);

    return 0;
}

int resultLatency, forkScore;
int forkBenchScore(int latencyMs) {
    if (latencyMs <= 0) return 1000; // Max score for near-zero latency (unlikely)

    // Base reference time in ms for normalization (e.g., a good PC = 1000 ms)
    const double baseLatency = 10000.0; // Use 10 seconds as a baseline
    const double scoreScale = 1000.0;   // Max score achievable

    // Score is inversely proportional to latency
    double score = scoreScale * (baseLatency / latencyMs);

    // Clamp score to a reasonable range
    if (score > 1000.0) score = 1000.0;
    if (score < 0.0) score = 0.0;

    return static_cast<int>(score);
}
