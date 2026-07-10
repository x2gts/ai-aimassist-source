#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "auth_system.h"
#include "operators.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Data
inline static ID3D11Device* g_pd3dDevice = nullptr;
inline static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline static IDXGISwapChain* g_pSwapChain = nullptr;
inline static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
inline static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "imgui_internal.h"

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

#include "user_circle.h"
#include "inter.h"
#include "ico_pack.h"

#include "color.h"


inline ImFont* ico = nullptr;
inline ImFont* ico_2 = nullptr;
inline ImFont* ico_minimize = nullptr;
inline ImFont* tab_text = nullptr;
inline ImFont* tab_text1 = nullptr;
inline ImFont* tab_text2 = nullptr;
inline ImFont* tab_text3 = nullptr;
inline ImFont* ico_logo = nullptr;
inline ImFont* ico_subtab = nullptr;
inline ID3D11ShaderResourceView* us = nullptr;
inline ID3D11ShaderResourceView* operator_textures[63] = {};

namespace thread1
{
   void threadstart();
   void POC();
  
}
namespace var {

   inline float frequency_factor_x = 0.1; // Ajustez le facteur de fr�quence selon vos pr�f�rences pour le mouvement en x
    inline float frequency_factor_y = 0.1; // Ajustez le facteur de fr�quence selon vos pr�f�rences pour le mouvement en y



    inline float randomness_factor = 0.02;
    inline float kp = 0.2, ki = 0.005, kd = 0.02;
    inline bool RTrigger;
    inline bool LTrigger ;
    inline float smoothing_factor = 0.05;
    inline float x_smooth = 0, y_smooth = 0;
   inline int iteration = 0;
  inline float jitter_factor = 1;
   inline int boxY;
   inline int boxX;
   inline int Width;
   inline int Height;
    inline bool fovCircle = false;
    inline float fovRadius = 250.0f;
    inline bool checkbox = true;
    inline bool triggerbot = false;
    inline bool esp = false;
    inline int scannFPS = 100;
    inline float smooth = 5.0f;
    inline float aim_speed = 1.0f;
    inline float aim_height = 50.0f;
    inline bool recoil_control = false;
    inline int selected_operator = 0;
    inline int recoil_subtab = 0; // 0=ATK, 1=DEF
    inline int recoil_config_tab = 0; // 0=operator, 1=sights, 2=barrel, 3=grip
    inline bool operator_textures_loaded = false;
    inline float sensitivity_h = 25.0f;
    inline float sensitivity_v = 25.0f;
    inline float ads_standard = 50.0f;
    inline float ads_1x = 55.0f;
    inline float ads_2_5x = 60.0f;
    inline float ads_3_5x = 82.0f;
    inline float ads_5x = 73.0f;
    inline float ads_12x = 62.0f;
    inline int selected_sight = 0;
    inline int selected_barrel = 0;
    inline int selected_grip = 0;
    inline int key0 = 0;
    inline int key4 = VK_INSERT;
    inline bool animated_background = false;
    inline std::string detection_backend = "CPU";
    inline bool debug_console = false;
}

inline static float tab_alpha = 0.0f;
inline static float tab_add = 0.0f;
inline static int active_tab = 0;
inline int tabs = 0;

inline bool menu = true;
inline float dpi_scale = 1.0f;

// Auth state (shared between main and aimbot thread)
inline std::string api_url = "https://ai-aimassist-source.onrender.com";
inline AuthSystem auth(api_url);
inline bool login = false;

inline void CustomStyleColor()
{
    ImGuiStyle& s = ImGui::GetStyle();
    ImVec4 accent(theme::accent[0], theme::accent[1], theme::accent[2], 1.0f);
    ImVec4 bg(theme::bg[0], theme::bg[1], theme::bg[2], 1.0f);
    ImVec4 hl(theme::highlight[0], theme::highlight[1], theme::highlight[2], 1.0f);
    ImVec4 bgDim(theme::bg[0] * 1.05f, theme::bg[1] * 1.05f, theme::bg[2] * 1.05f, 1.0f);
    ImVec4 bgLight(theme::bg[0] * 1.15f, theme::bg[1] * 1.15f, theme::bg[2] * 1.15f, 1.0f);

    s.Colors[ImGuiCol_WindowBg] = ImVec4(bg.x, bg.y, bg.z, 0.38f);
    s.Colors[ImGuiCol_ChildBg] = ImVec4(bg.x * 0.85f, bg.y * 0.85f, bg.z * 0.85f, 1.0f);
    s.Colors[ImGuiCol_PopupBg] = ImVec4(bg.x * 0.85f, bg.y * 0.85f, bg.z * 0.85f, 1.0f);
    s.Colors[ImGuiCol_Text] = ImVec4(0.47f, 0.47f, 0.47f, 1.0f);
    s.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    s.Colors[ImGuiCol_Border] = ImVec4(bg.x, bg.y, bg.z, 1.0f);
    s.Colors[ImGuiCol_TextSelectedBg] = ImVec4(hl.x, hl.y, hl.z, 0.4f);

    s.Colors[ImGuiCol_ScrollbarGrab] = bgDim;
    s.Colors[ImGuiCol_ScrollbarGrabHovered] = bgDim;
    s.Colors[ImGuiCol_ScrollbarGrabActive] = bgDim;

    s.Colors[ImGuiCol_SliderGrab] = accent;
    s.Colors[ImGuiCol_SliderGrabActive] = accent;

    s.Colors[ImGuiCol_Button] = bgDim;
    s.Colors[ImGuiCol_ButtonHovered] = bgLight;
    s.Colors[ImGuiCol_ButtonActive] = bgLight;

    s.Colors[ImGuiCol_Header] = hl;
    s.Colors[ImGuiCol_HeaderHovered] = hl;
    s.Colors[ImGuiCol_HeaderActive] = accent;

    s.Colors[ImGuiCol_FrameBg] = bgDim;
    s.Colors[ImGuiCol_FrameBgHovered] = bgLight;
    s.Colors[ImGuiCol_FrameBgActive] = bgLight;

    s.Colors[ImGuiCol_CheckMark] = accent;

    s.WindowBorderSize = 0;
    s.WindowPadding = ImVec2(0, 0);
    s.WindowRounding = 5.f;
    s.PopupBorderSize = 0.f;
    s.PopupRounding = 5.f;
    s.ChildRounding = 7;
    s.ChildBorderSize = 1.f;
    s.FrameBorderSize = 1.0f;
    s.ScrollbarSize = 3.0f;
    s.FrameRounding = 5.f;
    s.ItemSpacing = ImVec2(0, 20);
    s.ItemInnerSpacing = ImVec2(10, 0);
}
