#include <opencv2/opencv.hpp>
#include "screenshot.h"
#include "scanner.h"
#include <filesystem>
#include "defines.h"
#include "main.h"
#include <chrono>
#include "auth_system.h"

static std::wstring GetCurrentDirectory()
{
    wchar_t buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

static DWORD WINAPI RunningThread(LPVOID)
{
    screenshot screen;
    cv::Mat image = screen.get();
    const std::wstring current_directory = GetCurrentDirectory();
    const std::string str_exe_path(current_directory.begin(), current_directory.end());

    detector detect(
        str_exe_path + "\\" + LABELS_FILE_NAME,
        str_exe_path + "\\" + YOLO_CFG_FILE_NAME,
        str_exe_path + "\\" + YOLO_WEIGHTS_FILE_NAME
    );

    var::detection_backend = detect.getBackendName();

    // Re-validate key periodically to detect bans
    auto lastCheck = std::chrono::steady_clock::now();

    Sleep(1000);

    while (true)
    {
        thread1::POC();

        // Periodic ban check (every 30 seconds)
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= 30)
        {
            lastCheck = now;
            if (login && !auth.validate(auth.getLicenseKey()))
            {
                if (auth.getLastErrorCode() == "KEY_BANNED")
                {
                    MessageBoxA(NULL, "Your key has been banned.", "Access Revoked", MB_ICONERROR);
                    ExitProcess(0);
                }
            }
        }

        if ((GetAsyncKeyState(var::key0) & 0x8000) || (var::LTrigger && var::checkbox))
        {
            image = screen.get();
            detect.start(image);
            const int fpslimit = 251 - var::scannFPS;
            Sleep(fpslimit);
        }
        else
        {
            Sleep(10);
        }
    }

    return 0;
}

void thread1::threadstart()
{
    if (var::iteration == 0)
    {
        var::iteration = 1;
        CreateThread(nullptr, 0, RunningThread, nullptr, 0, nullptr);
    }
}
