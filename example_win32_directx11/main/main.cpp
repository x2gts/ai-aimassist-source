
#include "main.h"
#include <dwmapi.h>
#include "bind.h"
#include "auth_system.h"
#include <sstream>
#include <fstream>

#include "config.h"

static ID3D11ShaderResourceView* dr = nullptr;
static ID3D11ShaderResourceView* dr1 = nullptr;
static RECT rc = { 0 };
static bool open = true;

void Particles()
{
    const ImVec2 screen_size = { static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) };

    static ImVec2 particle_pos[50];
    static ImVec2 particle_target_pos[50];
    static float particle_speed[50];
    static float particle_radius[50];

    for (int i = 0; i < 50; ++i)
    {
        if (particle_pos[i].x == 0.0f || particle_pos[i].y == 0.0f)
        {
            particle_pos[i].x = static_cast<float>(rand() % static_cast<int>(screen_size.x) + 1);
            particle_pos[i].y = 15.0f;
            particle_speed[i] = static_cast<float>(1 + rand() % 25);
            particle_radius[i] = static_cast<float>(rand() % 4);

            particle_target_pos[i].x = static_cast<float>(rand() % static_cast<int>(screen_size.x));
            particle_target_pos[i].y = screen_size.y * 2.0f;
        }

        particle_pos[i] = ImLerp(particle_pos[i], particle_target_pos[i], ImGui::GetIO().DeltaTime * (particle_speed[i] / 60.0f));

        if (particle_pos[i].y > screen_size.y)
        {
            particle_pos[i] = ImVec2(0.0f, 0.0f);
        }

        ImGui::GetBackgroundDrawList()->AddCircleFilled(particle_pos[i], particle_radius[i], ImColor(255, 255, 255, 255));
    }
}
static HWND hwnd = nullptr;
static bool check = true;
void hide()
{
    if (GetAsyncKeyState(var::key4) && check)
    {
        check = false;
        open = !open;
    }
    else if (!GetAsyncKeyState(var::key4))
    {
        check = true;
    }
}

static int login_tab = 0;
static char login_username[256] = "";
static char login_password[256] = "";
static char login_key[256] = "";
static char login_error_message[512] = "";

static std::string getKeyFilePath()
{
    wchar_t buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring path(buffer);
    size_t pos = path.find_last_of(L"\\/");
    std::wstring dir = path.substr(0, pos);
    std::wstring file = dir + L"\\keyvex_license.key";
    return std::string(file.begin(), file.end());
}

static void saveKey(const char* key)
{
    std::ofstream f(getKeyFilePath());
    if (f.is_open())
    {
        f << key;
        f.close();
    }
}

static void deleteKey()
{
    DeleteFileA(getKeyFilePath().c_str());
}

static bool loadKey(std::string& out_key)
{
    std::ifstream f(getKeyFilePath());
    if (f.is_open())
    {
        std::getline(f, out_key);
        f.close();
        return !out_key.empty();
    }
    return false;
}
void move_window()
{
    GetWindowRect(hwnd, &rc);
    const ImVec2 window_pos = ImGui::GetWindowPos();

    if (window_pos.x != 0.0f || window_pos.y != 0.0f)
    {
        MoveWindow(hwnd, rc.left + static_cast<int>(window_pos.x), rc.top + static_cast<int>(window_pos.y), 855, 650, TRUE);
        ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    }
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = "ImGui";
    wc.lpszClassName = "Example";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassExA(&wc);
    
    const int screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int screen_height = GetSystemMetrics(SM_CYSCREEN);
    const int window_width = 855;
    const int window_height = 650;
    
    hwnd = CreateWindowExA(0, wc.lpszClassName, "dd", WS_POPUP,
        (screen_width / 2) - (window_width / 2),
        (screen_height / 2) - (window_height / 2),
        window_width, window_height, nullptr, nullptr, nullptr, nullptr);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    const auto* glyph_ranges = io.Fonts->GetGlyphRangesCyrillic();
    io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 16.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text1 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 12.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text2 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 24.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text3 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 40.0f * dpi_scale, nullptr, glyph_ranges);
    ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 25.0f * dpi_scale, nullptr, glyph_ranges);
    ico_2 = io.Fonts->AddFontFromMemoryTTF(&Menuicon, sizeof(Menuicon), 20.0f * dpi_scale, nullptr, glyph_ranges);
    ico_subtab = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 35.0f * dpi_scale, nullptr, glyph_ranges);
    ico_logo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 31.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 19.0f * dpi_scale, nullptr, glyph_ranges);
    ico_minimize = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 27.0f * dpi_scale, nullptr, glyph_ranges);
    ImGui::StyleColorsDark();

    if (var::debug_console)
    {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        std::cout << "[Debug] Console Allocated" << std::endl;
    }

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    loadConfig();

    // Auto-login with saved key
    {
        std::string saved_key;
        if (loadKey(saved_key))
        {
            strcpy_s(login_key, saved_key.c_str());
            if (auth.validate(saved_key))
            {
                login = true;
            }
            else if (auth.getLastErrorCode() == "KEY_BANNED")
            {
                deleteKey();
                login_key[0] = '\0';
                strcpy_s(login_error_message, "Your key has been banned");
            }
            else
            {
                strcpy_s(login_error_message, "Key saved but could not verify. Check your connection.");
            }
        }
    }

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        hide();

        if (done || GetAsyncKeyState(VK_END))
            break;
        if (var::iteration == 0)
            thread1::threadstart();
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (var::esp && var::Width > 0 && var::Height > 0)
        {
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2((float)var::boxX, (float)var::boxY),
                ImVec2((float)(var::boxX + var::Width), (float)(var::boxY + var::Height)),
                ImColor(255, 0, 0, 255),
                0.0f, 0, 2.0f
            );
        }

        if (var::fovCircle)
        {
            ImVec2 screenCenter = ImVec2(
                GetSystemMetrics(SM_CXSCREEN) * 0.5f,
                GetSystemMetrics(SM_CYSCREEN) * 0.5f
            );
            ImGui::GetBackgroundDrawList()->AddCircle(
                screenCenter,
                var::fovRadius,
                ImColor(theme::accent[0], theme::accent[1], theme::accent[2], 0.5f),
                64,
                1.5f
            );
        }

        theme::updateColors();
        CustomStyleColor();

        D3DX11_IMAGE_LOAD_INFO info;
        ID3DX11ThreadPump* pump = nullptr;

        if (us == nullptr)
            D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, user, sizeof(user), &info, pump, &us, nullptr);

        ImGui::SetNextWindowSize(ImVec2(855.0f * dpi_scale, 650.0f * dpi_scale));
        if (dr1 == nullptr)
            D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, roll, sizeof(roll), &info, pump, &dr1, nullptr);

        if (open)
        {
            if (!login)
            {
                ImGui::Begin("Login", &menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                move_window();
                
                const ImVec2& p = ImGui::GetWindowPos();
                ImGuiStyle& s = ImGui::GetStyle();
                
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f * dpi_scale + p.x, 650.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                
                ImGui::SetCursorPos(ImVec2(227.5f * dpi_scale, 30.0f * dpi_scale));
                ImGui::BeginChild("LoginWindow", ImVec2(400.0f * dpi_scale, 520.0f * dpi_scale), true);
                {
                    float cw = 400.0f * dpi_scale;
                    float pad = 30.0f * dpi_scale;
                    float inner_w = cw - pad * 2.0f;

                    ImGui::SetCursorPos(ImVec2((cw - ImGui::CalcTextSize("License Activation").x) / 2.0f, 30.0f * dpi_scale));
                    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "License Activation");

                    ImGui::SetCursorPos(ImVec2(pad, 90.0f * dpi_scale));
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "License Key:");

                    ImGui::SetCursorPos(ImVec2(pad, 115.0f * dpi_scale));
                    ImGui::PushItemWidth(inner_w);
                    ImGui::InputText("##license_key", login_key, IM_ARRAYSIZE(login_key));
                    ImGui::PopItemWidth();

                    ImGui::SetCursorPos(ImVec2(pad, 270.0f * dpi_scale));
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "HWID: %s", auth.getHWID().c_str());

                    ImGui::SetCursorPos(ImVec2(pad, 310.0f * dpi_scale));
                    if (strlen(login_error_message) > 0)
                    {
                        ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "%s", login_error_message);
                    }

                    ImGui::SetCursorPos(ImVec2((cw - 160.0f * dpi_scale) / 2.0f, 370.0f * dpi_scale));
                    if (ImGui::Button("Activate", ImVec2(160.0f * dpi_scale, 40.0f * dpi_scale)))
                    {
                        if (strlen(login_key) < 10) {
                            strcpy_s(login_error_message, "Please enter a valid license key");
                        }
                        else if (auth.validate(login_key))
                        {
                            login = true;
                            saveKey(login_key);
                            strcpy_s(login_error_message, "");
                        }
                        else
                        {
                            strcpy_s(login_error_message, "Invalid or expired license key");
                        }
                    }

                    ImGui::SetCursorPos(ImVec2((cw - ImGui::CalcTextSize("Subscription: None").x) / 2.0f, 440.0f * dpi_scale));
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Subscription: %s",
                        auth.isValid() ? auth.getSubscription().c_str() : "None");
                    if (auth.isValid() && auth.getExpiresAt() != "never") {
                        auto exp_text = "Expires: " + auth.getExpiresAt();
                        ImGui::SetCursorPos(ImVec2((cw - ImGui::CalcTextSize(exp_text.c_str()).x) / 2.0f, 470.0f * dpi_scale));
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Expires: %s",
                            auth.getExpiresAt().c_str());
                    }
                }
                ImGui::EndChild();
                ImGui::End();
            }
            else if (login)
            {
                ImGui::Begin("Menu", &menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                move_window();

                const ImVec2& p = ImGui::GetWindowPos();
                ImGuiStyle& s = ImGui::GetStyle();

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
                ImGui::BeginChild("G-Tab", ImVec2(173.0f * dpi_scale, 790.0f * dpi_scale), false);
                {
                    ImGui::GetForegroundDrawList()->AddText(tab_text3, 20.0f * dpi_scale, ImVec2(20.0f * dpi_scale + p.x, 12.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "       Bloodbot");
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(273.0f * dpi_scale + p.x, 790.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::Tab_Child), s.WindowRounding);

                    ImGui::SetCursorPosY(60.0f);
                    ImGui::SetWindowFontScale(dpi_scale);
                    
                    if (ImGui::Tab("H", "Aimbot", "AI based aimbot", tabs == 0, ImVec2(150.0f * dpi_scale, 42.0f * dpi_scale)))
                        tabs = 0;
                    if (ImGui::Tab("G", "Recoil", "Recoil control", tabs == 1, ImVec2(150.0f * dpi_scale, 42.0f * dpi_scale)))
                        tabs = 1;
                    if (ImGui::Tab("E", "Misc", "Other settings", tabs == 2, ImVec2(150.0f * dpi_scale, 42.0f * dpi_scale)))
                        tabs = 2;
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f * dpi_scale + p.x, 790.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 755.0f * dpi_scale + p.y), ImVec2(855.0f * dpi_scale + p.x, 755.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::lite_color), s.WindowRounding);

                const float delta_time = ImGui::GetIO().DeltaTime;
                tab_alpha = ImClamp(tab_alpha + (7.0f * delta_time * (tabs == active_tab ? 1.0f : -1.0f)), 0.0f, 1.0f);
                tab_add = ImClamp(tab_add + (50.0f * delta_time * (tabs == active_tab ? 1.0f : -1.0f)), 0.0f, 1.0f);

                if (tab_alpha == 0.0f && tab_add == 0.0f)
                    active_tab = tabs;

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * s.Alpha);
                ImGui::SetCursorPos(ImVec2(203.0f * dpi_scale, 30.0f * dpi_scale));

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
                ImGui::BeginChild("General", ImVec2(717.0f * dpi_scale, 650.0f * dpi_scale), false);
                {
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(colors::lite_color));
                    switch (active_tab)
                    {
                    case 0:
                {
                    ImGui::SetCursorPosY(5.0f * dpi_scale);
                    ImGui::BeginChildPos("ambt", ImVec2(300.0f * dpi_scale, 580.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        ImGui::Checkbox("Aimbot", &var::checkbox);
                        ImGui::Checkbox("Triggerbot", &var::triggerbot);
                        ImGui::Keybind("Keybind", &var::key0, true);
                        ImGui::SliderFloat("Smoothness", &var::smooth, 1.0f, 100.0f, "%.1f", 0);
                        ImGui::SliderFloat("Aim speed", &var::aim_speed, 0.1f, 5.0f, "%.1f", 0);
                        ImGui::SliderFloat("Aim height", &var::aim_height, 1.0f, 100.0f, "%.3f", 0);
                        ImGui::SliderInt("Scanning frequence", &var::scannFPS, 1, 250, "%d FPS", 0);
                        ImGui::SameLine(270.0f);
                        ImGui::Text("[?]");
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip("Lowering the AI scanning FPS reduces computer resource usage but may impact aiming accuracy");
                        }
                        ImGui::Checkbox("Show detections", &var::esp);
                        ImGui::SameLine(270.0f);
                        ImGui::Text("[?]");
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip("Soon");
                        }
                        ImGui::Checkbox("Show FOV", &var::fovCircle);
                        if (var::fovCircle)
                        {
                            ImGui::SliderFloat("FOV Radius", &var::fovRadius, 50.0f, 500.0f, "%.0f", 0);
                        }
                    }
                    ImGui::EndChild();

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f + p.x, 790.0f + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 759.0f + p.y), ImVec2(855.0f + p.x, 790.0f + p.y), ImGui::GetColorU32(colors::lite_color), s.WindowRounding);

                    if (dr == nullptr)
                        D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, dragon, sizeof(dragon), &info, pump, &dr, nullptr);
                    
                    ImGui::SetCursorPos(ImVec2(320.0f * dpi_scale, 5.0f * dpi_scale));
                    ImGui::BeginChildPos("Visual Preview", ImVec2(300.0f * dpi_scale, 580.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        
                        ImVec2 pos = ImGui::GetWindowPos();
                        ImDrawList* draw = ImGui::GetWindowDrawList();
                        float boxH = 500.0f;
                        float imgAspect = 426.0f / 683.0f;
                        float boxW = boxH * imgAspect;
                        float centerX = pos.x + 150.0f;
                        draw->AddImageRounded(dr, ImVec2(centerX - boxW / 2.0f, pos.y + 80.0f), ImVec2(centerX + boxW / 2.0f, pos.y + 80.0f + boxH), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(255, 255, 255, 255), 10.0f);

                        ImGui::SetCursorPosY(135.0f);
                        ImGui::SetCursorPosX(10.0f);
                        ImGui::VSliderFloat(" ", ImVec2(30.0f, 415.0f), &var::aim_height, 0.0f, 100.0f, "", 0);

                        if (var::esp)
                        {
                            ImGui::SetCursorPos(ImVec2(35.0f, 70.0f));
                            const ImVec2 pos1 = ImGui::GetCursorScreenPos();
                            draw->AddRect(ImVec2(pos1.x, pos1.y), ImVec2(pos1.x + 220.0f, pos1.y + 480.0f), ImColor(255, 255, 255, 255), 0.0f, 0.0f, 1.0f);
                        }
                    }
                    ImGui::EndChild();
                }
                break;
                case 1:
                {
                    ImGui::BeginChildPos("", ImVec2(620.0f * dpi_scale, 100.0f * dpi_scale));
                    {
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 26.0f * dpi_scale, ImVec2(450.0f * dpi_scale + p.x, 55.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Recoil Control");
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 16.0f * dpi_scale, ImVec2(390.0f * dpi_scale + p.x, 85.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Configure sensitivity and attachments");
                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPosY(110.0f * dpi_scale);
                    {
                        float tab_w = 310.0f * dpi_scale;
                        float tab_h = 35.0f * dpi_scale;
                        ImVec4 activeCol(theme::accent[0], theme::accent[1], theme::accent[2], 1.0f);
                        ImVec4 inactiveCol(theme::bg[0] * 0.85f, theme::bg[1] * 0.85f, theme::bg[2] * 0.85f, 1.0f);
                        ImVec4 inactiveHover(theme::bg[0] * 1.05f, theme::bg[1] * 1.05f, theme::bg[2] * 1.05f, 1.0f);

                        if (var::recoil_subtab == 0)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, activeCol);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, activeCol);
                            ImGui::Button("ATK##tab", ImVec2(tab_w, tab_h));
                            ImGui::PopStyleColor(2);
                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Button, inactiveCol);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, inactiveHover);
                            if (ImGui::Button("DEF##tab", ImVec2(tab_w, tab_h)))
                                var::recoil_subtab = 1;
                            ImGui::PopStyleColor(2);
                        }
                        else
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, inactiveCol);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, inactiveHover);
                            if (ImGui::Button("ATK##tab", ImVec2(tab_w, tab_h)))
                                var::recoil_subtab = 0;
                            ImGui::PopStyleColor(2);
                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Button, activeCol);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, activeCol);
                            ImGui::Button("DEF##tab", ImVec2(tab_w, tab_h));
                            ImGui::PopStyleColor(2);
                        }
                    }

                    ImGui::SetCursorPosY(155.0f * dpi_scale);
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(colors::lite_color));
                    ImGui::BeginChild("recoil_ops", ImVec2(620.0f * dpi_scale, 530.0f * dpi_scale), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                    {
                        ImGui::SetWindowFontScale(dpi_scale);

                        ImGui::Checkbox("Recoil control", &var::recoil_control);
                        ImGui::Spacing();

                        if (!var::operator_textures_loaded)
                        {
                            for (int i = 0; i < 63; i++)
                            {
                                D3DX11CreateShaderResourceViewFromMemory(
                                    g_pd3dDevice,
                                    operator_icons[i].data,
                                    operator_icons[i].size,
                                    nullptr, nullptr,
                                    &operator_textures[i], nullptr);
                            }
                            var::operator_textures_loaded = true;
                        }

                        float btn_size = 64.0f * dpi_scale;
                        float padding = 6.0f * dpi_scale;
                        float grid_w = 600.0f * dpi_scale;
                        int cols = (int)((grid_w + padding) / (btn_size + padding));
                        if (cols < 1) cols = 1;

                        ImGuiStyle& style = ImGui::GetStyle();
                        ImVec2 old_spacing = style.ItemSpacing;
                        style.ItemSpacing = ImVec2(padding, padding);

                        const int* order = var::recoil_subtab == 0 ? atk_order : def_order;
                        int order_count = var::recoil_subtab == 0 ? atk_order_count : def_order_count;

                        int col = 0;
                        for (int idx = 0; idx < order_count; idx++)
                        {
                            int i = order[idx];
                            bool selected = (var::selected_operator == i);

                            ImGui::PushID(i);

                            ImVec2 btn_pos = ImGui::GetCursorScreenPos();

                            bool clicked = ImGui::InvisibleButton("##opbtn", ImVec2(btn_size, btn_size));
                            if (clicked)
                                var::selected_operator = i;

                            ImDrawList* dl = ImGui::GetWindowDrawList();
                            ImU32 bg_col = ImGui::GetColorU32(selected ? colors::Op_Selected : colors::Op_Normal);
                            dl->AddRectFilled(btn_pos, ImVec2(btn_pos.x + btn_size, btn_pos.y + btn_size), bg_col, 6.0f);

                            if (operator_textures[i])
                            {
                                float pad = 2.0f * dpi_scale;
                                dl->AddImage(
                                    (ImTextureID)operator_textures[i],
                                    ImVec2(btn_pos.x + pad, btn_pos.y + pad),
                                    ImVec2(btn_pos.x + btn_size - pad, btn_pos.y + btn_size - pad),
                                    ImVec2(0, 0), ImVec2(1, 1),
                                    IM_COL32(255, 255, 255, 255));
                            }
                            else
                            {
                                ImVec2 ts = ImGui::CalcTextSize(recoil_data[i].name);
                                dl->AddText(
                                    ImVec2(btn_pos.x + btn_size / 2 - ts.x / 2, btn_pos.y + btn_size / 2 - ts.y / 2),
                                    IM_COL32(200, 200, 200, 255), recoil_data[i].name);
                            }

                            if (selected)
                                dl->AddRect(btn_pos, ImVec2(btn_pos.x + btn_size, btn_pos.y + btn_size),
                                    ImGui::GetColorU32(colors::Op_Border), 6.0f, 0, 2.0f);

                            ImGui::PopID();

                            col++;
                            if (col % cols != 0)
                                ImGui::SameLine();
                        }

                        style.ItemSpacing = old_spacing;

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "%s", recoil_data[var::selected_operator].name);
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), " - %s", recoil_data[var::selected_operator].weapon);

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Sensitivity");

                        auto drawLabeledSlider = [&](const char* label, const char* id, float* value, float min, float max) {
                            char val_buf[32];
                            snprintf(val_buf, sizeof(val_buf), "%.0f", *value);
                            ImVec2 label_size = ImGui::CalcTextSize(label);
                            ImVec2 val_size = ImGui::CalcTextSize(val_buf);
                            float slider_w = 580.0f * dpi_scale;
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (slider_w - label_size.x) / 2.0f);
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", label);
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (slider_w - val_size.x) / 2.0f - (label_size.x - val_size.x) / 2.0f);
                            ImGui::TextColored(ImVec4(theme::accent[0], theme::accent[1], theme::accent[2], 1.0f), "%s", val_buf);
                            ImGui::PushItemWidth(slider_w);
                            ImGui::SliderFloat(id, value, min, max, "");
                            ImGui::PopItemWidth();
                        };

                        drawLabeledSlider("Horizontal", "##sens_h", &var::sensitivity_h, 1.0f, 100.0f);
                        ImGui::Spacing();
                        drawLabeledSlider("Vertical", "##sens_v", &var::sensitivity_v, 1.0f, 100.0f);

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "ADS Sensitivity (per scope)");

                        const char* ads_labels[] = { "Standard", "1.0x", "2.5x", "3.5x", "5.0x", "12.0x" };
                        float* ads_values[] = { &var::ads_standard, &var::ads_1x, &var::ads_2_5x, &var::ads_3_5x, &var::ads_5x, &var::ads_12x };
                        for (int a = 0; a < 6; a++)
                        {
                            drawLabeledSlider(ads_labels[a], ("##ads_" + std::to_string(a)).c_str(), ads_values[a], 1.0f, 100.0f);
                            if (a < 5) ImGui::Spacing();
                        }

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        float drop_w = 190.0f * dpi_scale;
                        ImVec4 btnActive(theme::accent[0], theme::accent[1], theme::accent[2], 1.0f);
                        ImVec4 btnInactive(theme::bg[0] * 0.85f, theme::bg[1] * 0.85f, theme::bg[2] * 0.85f, 1.0f);
                        ImVec4 btnHover(theme::bg[0] * 1.05f, theme::bg[1] * 1.05f, theme::bg[2] * 1.05f, 1.0f);

                        {
                            char sight_label[64];
                            snprintf(sight_label, sizeof(sight_label), "%s ##sight", sight_names[var::selected_sight]);
                            ImGui::PushStyleColor(ImGuiCol_Button, btnInactive);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnHover);
                            if (ImGui::Button(sight_label, ImVec2(drop_w, 28.0f * dpi_scale)))
                                ImGui::OpenPopup("##sight_popup");
                            ImGui::PopStyleColor(2);
                            if (ImGui::BeginPopup("##sight_popup"))
                            {
                                for (int s = 0; s < SIGHT_COUNT; s++)
                                {
                                    bool is_selected = (var::selected_sight == s);
                                    if (ImGui::Selectable(sight_names[s], is_selected))
                                        var::selected_sight = s;
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "ADS: %.2fx  Recoil: %.0f%%",
                                sight_ads_mult[var::selected_sight], sight_recoil_mod[var::selected_sight] * 100.0f);
                        }

                        {
                            char barrel_label[64];
                            snprintf(barrel_label, sizeof(barrel_label), "%s ##barrel", barrel_names[var::selected_barrel]);
                            ImGui::PushStyleColor(ImGuiCol_Button, btnInactive);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnHover);
                            if (ImGui::Button(barrel_label, ImVec2(drop_w, 28.0f * dpi_scale)))
                                ImGui::OpenPopup("##barrel_popup");
                            ImGui::PopStyleColor(2);
                            if (ImGui::BeginPopup("##barrel_popup"))
                            {
                                for (int b = 0; b < BARREL_COUNT; b++)
                                {
                                    bool is_selected = (var::selected_barrel == b);
                                    if (ImGui::Selectable(barrel_names[b], is_selected))
                                        var::selected_barrel = b;
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "V: %.0f%%  H: %.0f%%",
                                barrel_vert_mod[var::selected_barrel] * 100.0f, barrel_horz_mod[var::selected_barrel] * 100.0f);
                        }

                        {
                            char grip_label[64];
                            snprintf(grip_label, sizeof(grip_label), "%s ##grip", grip_names[var::selected_grip]);
                            ImGui::PushStyleColor(ImGuiCol_Button, btnInactive);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnHover);
                            if (ImGui::Button(grip_label, ImVec2(drop_w, 28.0f * dpi_scale)))
                                ImGui::OpenPopup("##grip_popup");
                            ImGui::PopStyleColor(2);
                            if (ImGui::BeginPopup("##grip_popup"))
                            {
                                for (int g = 0; g < GRIP_COUNT; g++)
                                {
                                    bool is_selected = (var::selected_grip == g);
                                    if (ImGui::Selectable(grip_names[g], is_selected))
                                        var::selected_grip = g;
                                }
                                ImGui::EndPopup();
                            }
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "V: %.0f%%  H: %.0f%%",
                                grip_vert_mod[var::selected_grip] * 100.0f, grip_horz_mod[var::selected_grip] * 100.0f);
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }
                break;
                case 2:
                {
                    ImGui::BeginChildPos("", ImVec2(620.0f * dpi_scale, 100.0f * dpi_scale));
                    {
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 26.0f * dpi_scale, ImVec2(450.0f * dpi_scale + p.x, 55.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Miscellaneous");
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 16.0f * dpi_scale, ImVec2(390.0f * dpi_scale + p.x, 85.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Modify menu games and other functions");
                    }
                    ImGui::EndChild();
                    ImGui::SetCursorPosY(120.0f * dpi_scale);
                    ImGui::BeginChildPos("Misc", ImVec2(620.0f * dpi_scale, 490.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        ImGui::Checkbox("Render animated background", &var::animated_background);
                        ImGui::Keybind("Hide menu", &var::key4, true);
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Theme");

                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(theme::accent[0] * 0.3f, theme::accent[1] * 0.3f, theme::accent[2] * 0.3f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(theme::accent[0] * 0.4f, theme::accent[1] * 0.4f, theme::accent[2] * 0.4f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(theme::accent[0] * 0.5f, theme::accent[1] * 0.5f, theme::accent[2] * 0.5f, 1.0f));
                        ImGui::PushItemWidth(300.0f * dpi_scale);
                        ImGui::ColorEdit3("Accent##theme", theme::accent, ImGuiColorEditFlags_NoAlpha);
                        ImGui::ColorEdit3("Background##theme", theme::bg, ImGuiColorEditFlags_NoAlpha);
                        ImGui::ColorEdit3("Highlight##theme", theme::highlight, ImGuiColorEditFlags_NoAlpha);
                        ImGui::PopItemWidth();
                        ImGui::PopStyleColor(3);
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Detection: %s", var::detection_backend.c_str());
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        if (ImGui::Button("Save config", ImVec2(200.0f * dpi_scale, 35.0f * dpi_scale)))
                        {
                            saveConfig();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Load config", ImVec2(200.0f * dpi_scale, 35.0f * dpi_scale)))
                        {
    loadConfig();
    loadRecoilPatterns();
                        }
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        if (ImGui::Button("Logout", ImVec2(200.0f * dpi_scale, 35.0f * dpi_scale)))
                        {
                            login = false;
                            login_key[0] = '\0';
                            deleteKey();
                        }
                    }
                    ImGui::EndChild();
                }
                break;
                }
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }

                if (var::animated_background)
                    Particles();

                ImGui::PopStyleVar();
                
                const ImVec2 window_pos = ImGui::GetWindowPos();
                const ImVec2 window_size = ImGui::GetWindowSize();
                ImGui::GetForegroundDrawList()->AddText(nullptr, 11.0f * dpi_scale, 
                    ImVec2(window_pos.x + window_size.x - 120.0f * dpi_scale, window_pos.y + window_size.y - 20.0f * dpi_scale),
                    ImColor(100, 100, 100, 150), var::detection_backend.c_str());
                
                ImGui::End();
            }
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassA(wc.lpszClassName, wc.hInstance);
    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (FAILED(res))
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 1;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = static_cast<UINT>(LOWORD(lParam));
        g_ResizeHeight = static_cast<UINT>(HIWORD(lParam));
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
