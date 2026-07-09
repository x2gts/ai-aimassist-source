#include "mainA.h"
#include "opencv2/opencv.hpp"
#include "main.h"
#include <Windows.h>
#include <cmath>
#include "mouse_interface.h"

static mouse_interface mouse;

void aimbot::aim_to(int x, int y, int box_w, int box_h)
{
    const int screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int screen_height = GetSystemMetrics(SM_CYSCREEN);

    x = static_cast<int>(screen_width / 2 - ACTIVATION_RANGE / 2 + x + box_w / 2 + 5);
    y = static_cast<int>(screen_height / 2 - ACTIVATION_RANGE / 2 + y + (101 - static_cast<int>(var::aim_height)) + box_h / 4);

    const int x_offset = x - screen_width / 2;
    const int y_offset = y - screen_height / 2;

    static double x_smooth = 0.0;
    static double y_smooth = 0.0;

    float factor = var::smooth / 100.0f;
    if (factor < 0.01f) factor = 0.01f;
    if (factor > 0.99f) factor = 0.99f;

    x_smooth = x_smooth * (1.0 - factor) + x_offset * factor;
    y_smooth = y_smooth * (1.0 - factor) + y_offset * factor;

    const int move_x = static_cast<int>(x_smooth * var::aim_speed);
    const int move_y = static_cast<int>(y_smooth * var::aim_speed);

    mouse.move(move_x, move_y);
}

void aimbot::recoil_control()
{
    if (!var::recoil_control) return;
    if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) return;
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) return;

    if (var::selected_operator >= 0 && var::selected_operator < recoil_data_count)
    {
        const RecoilPattern& pattern = recoil_data[var::selected_operator];
        float target_y = pattern.vertical * 20.0f;
        float target_x = pattern.horizontal * 10.0f;

        static float recoil_x_smooth = 0.0f;
        static float recoil_y_smooth = 0.0f;

        float factor = 0.25f;
        recoil_x_smooth = recoil_x_smooth * (1.0f - factor) + target_x * factor;
        recoil_y_smooth = recoil_y_smooth * (1.0f - factor) + target_y * factor;

        mouse.move(static_cast<int>(recoil_x_smooth), static_cast<int>(recoil_y_smooth));
    }
    else
    {
        static float default_recoil_y = 0.0f;
        float target = 2.0f;
        default_recoil_y = default_recoil_y * 0.75f + target * 0.25f;
        mouse.move(0, static_cast<int>(default_recoil_y));
    }
}
