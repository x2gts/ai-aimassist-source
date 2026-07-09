#pragma once
#include "json.hpp"
#include <fstream>
#include <Windows.h>

// config.h must be included AFTER main.h (uses var:: namespace and animated_background)

using json = nlohmann::json;

inline std::string getConfigPath()
{
    char exePath[MAX_PATH] = { 0 };
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string path(exePath);
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos)
        path = path.substr(0, lastSlash + 1);
    return path + "keyvex_config.json";
}

inline void saveConfig()
{
    json j;
    j["aimbot"]["enabled"] = var::checkbox;
    j["aimbot"]["triggerbot"] = var::triggerbot;
    j["aimbot"]["smoothness"] = var::smooth;
    j["aimbot"]["aim_speed"] = var::aim_speed;
    j["aimbot"]["aim_height"] = var::aim_height;
    j["aimbot"]["scan_fps"] = var::scannFPS;
    j["aimbot"]["show_detections"] = var::esp;
    j["aimbot"]["show_fov"] = var::fovCircle;
    j["aimbot"]["fov_radius"] = var::fovRadius;
    j["aimbot"]["keybind"] = var::key0;

    j["recoil"]["enabled"] = var::recoil_control;
    j["recoil"]["selected_operator"] = var::selected_operator;
    j["recoil"]["subtab"] = var::recoil_subtab;

    j["misc"]["hide_menu_key"] = var::key4;
    j["misc"]["animated_background"] = var::animated_background;

    j["theme"]["accent"] = { theme::accent[0], theme::accent[1], theme::accent[2] };
    j["theme"]["bg"] = { theme::bg[0], theme::bg[1], theme::bg[2] };
    j["theme"]["highlight"] = { theme::highlight[0], theme::highlight[1], theme::highlight[2] };

    std::ofstream f(getConfigPath());
    if (f.is_open())
    {
        f << j.dump(4);
        f.close();
    }
}

inline void loadConfig()
{
    std::ifstream f(getConfigPath());
    if (!f.is_open()) return;

    try
    {
        json j = json::parse(f);

        if (j.contains("aimbot"))
        {
            auto& a = j["aimbot"];
            if (a.contains("enabled")) var::checkbox = a["enabled"];
            if (a.contains("triggerbot")) var::triggerbot = a["triggerbot"];
            if (a.contains("smoothness")) var::smooth = a["smoothness"];
            if (a.contains("aim_speed")) var::aim_speed = a["aim_speed"];
            if (a.contains("aim_height")) var::aim_height = a["aim_height"];
            if (a.contains("scan_fps")) var::scannFPS = a["scan_fps"];
            if (a.contains("show_detections")) var::esp = a["show_detections"];
            if (a.contains("show_fov")) var::fovCircle = a["show_fov"];
            if (a.contains("fov_radius")) var::fovRadius = a["fov_radius"];
            if (a.contains("keybind")) var::key0 = a["keybind"];
        }

        if (j.contains("recoil"))
        {
            auto& r = j["recoil"];
            if (r.contains("enabled")) var::recoil_control = r["enabled"];
            if (r.contains("selected_operator")) var::selected_operator = r["selected_operator"];
            if (r.contains("subtab")) var::recoil_subtab = r["subtab"];
        }

        if (j.contains("misc"))
        {
            auto& m = j["misc"];
            if (m.contains("hide_menu_key")) var::key4 = m["hide_menu_key"];
            if (m.contains("animated_background")) var::animated_background = m["animated_background"];
        }

        if (j.contains("theme"))
        {
            auto& t = j["theme"];
            if (t.contains("accent") && t["accent"].is_array() && t["accent"].size() == 3) {
                theme::accent[0] = t["accent"][0]; theme::accent[1] = t["accent"][1]; theme::accent[2] = t["accent"][2];
            }
            if (t.contains("bg") && t["bg"].is_array() && t["bg"].size() == 3) {
                theme::bg[0] = t["bg"][0]; theme::bg[1] = t["bg"][1]; theme::bg[2] = t["bg"][2];
            }
            if (t.contains("highlight") && t["highlight"].is_array() && t["highlight"].size() == 3) {
                theme::highlight[0] = t["highlight"][0]; theme::highlight[1] = t["highlight"][1]; theme::highlight[2] = t["highlight"][2];
            }
        }
    }
    catch (...) {}
}
