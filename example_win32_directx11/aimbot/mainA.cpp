#include "mainA.h"
#include "opencv2/opencv.hpp"
#include "main.h"
#include <Windows.h>
#include <cmath>
#include <chrono>
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
    static auto last_time = std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - last_time).count();
    last_time = now;
    if (dt <= 0.0f || dt > 0.1f) dt = 0.016f;

    const float dist = sqrtf((float)(x_offset * x_offset + y_offset * y_offset));

    if (dist < 2.0f)
    {
        x_smooth = x_offset;
        y_smooth = y_offset;
    }
    else if (dist > 40.0f)
    {
        float snap_speed = 0.70f * dt * 60.0f;
        if (snap_speed > 0.85f) snap_speed = 0.85f;
        x_smooth = x_smooth + (x_offset - x_smooth) * snap_speed;
        y_smooth = y_smooth + (y_offset - y_smooth) * snap_speed;
    }
    else
    {
        float base = var::smooth / 100.0f;
        float factor = base * 0.6f * dt * 60.0f;
        if (factor < 0.05f) factor = 0.05f;
        if (factor > 0.90f) factor = 0.90f;
        x_smooth = x_smooth * (1.0 - factor) + x_offset * factor;
        y_smooth = y_smooth * (1.0 - factor) + y_offset * factor;
    }

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

        float sens_h = var::sensitivity_h > 0.0f ? var::sensitivity_h : BASE_SENSITIVITY;
        float sens_v = var::sensitivity_v > 0.0f ? var::sensitivity_v : BASE_SENSITIVITY;

        float ads_per_scope[] = { var::ads_standard, var::ads_1x, var::ads_2_5x, var::ads_3_5x, var::ads_5x, var::ads_12x };
        float ads_val = ads_per_scope[var::selected_sight < SIGHT_COUNT ? var::selected_sight : 0];
        float ads_mult = ads_val / 50.0f;

        float vert_scale = (BASE_SENSITIVITY / sens_v) * ads_mult;
        float horz_scale = (BASE_SENSITIVITY / sens_h) * ads_mult;

        float vert_mod = barrel_vert_mod[var::selected_barrel] * grip_vert_mod[var::selected_grip] * sight_recoil_mod[var::selected_sight];
        float horz_mod = barrel_horz_mod[var::selected_barrel] * grip_horz_mod[var::selected_grip] * sight_recoil_mod[var::selected_sight];

        float target_y = pattern.vertical * 20.0f * vert_scale * vert_mod;
        float target_x = pattern.horizontal * 10.0f * horz_scale * horz_mod;

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
