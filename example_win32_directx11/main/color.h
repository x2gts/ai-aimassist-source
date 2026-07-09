#pragma once
#include "imgui.h"



namespace colors {

    inline ImVec4 general_color = ImColor(195, 105, 48, 255);
    inline ImVec4 main_color = ImColor(16, 16, 16, 255);
    inline ImVec4 lite_color = ImColor(20, 20, 20, 255);
    inline ImVec4 gray_color = ImColor(43, 43, 43, 255);
    inline ImVec4 Tab_Child = ImColor(15, 15, 15, 255);
    inline ImVec4 Tab_Border = ImColor(26, 26, 26, 255);

    inline ImVec4 Tab_Selected = ImColor(75, 50, 237,255);
    inline ImVec4 Tab_Hovered = ImColor(130, 130, 130, 255);
    inline ImVec4 Tab = ImColor(80, 80, 80, 255);

    inline ImVec4 Checkbox = ImColor(14, 14, 14, 255);
    inline ImVec4 Checkbox_Hovered = ImColor(12, 12, 12, 255);
    inline ImVec4 Checkbox_Active = ImColor(218, 96, 21, 255);

    inline ImVec4 Car_Slider = ImColor(221, 97, 23, 255);
    inline ImVec4 Car_Slider_Hovered = ImColor(223, 99, 25, 255);
    inline ImVec4 Car_Slider_Active = ImColor(223, 99, 25, 255);

    inline ImVec4 Slider = ImColor(16, 16, 16, 255);
    inline ImVec4 Slider_Hovered = ImColor(18, 18, 18, 255);
    inline ImVec4 Slider_Active = ImColor(18, 18, 18, 255);

    inline ImVec4 Circle_Slider = ImColor(255, 255, 255, 255);
    inline ImVec4 Circle_SliderHovered = ImColor(255, 255, 255, 255);
    inline ImVec4 Circle_SliderActive = ImColor(255, 255, 255, 255);

    inline ImVec4 Combo = ImColor(26, 26, 26, 255);
    inline ImVec4 Combo_Hovered = ImColor(26, 26, 26, 255);
    inline ImVec4 Combo_Active = ImColor(26, 26, 26, 255);

    inline ImVec4 InputText = ImColor(16, 16, 16, 255);
    inline ImVec4 InputText_Hovered = ImColor(18, 18, 18, 255);
    inline ImVec4 InputText_Active = ImColor(18, 18, 18, 255);

    inline ImVec4 Button = ImColor(26, 26, 26, 255);
    inline ImVec4 Button_Hovered = ImColor(30, 30, 30, 255);
    inline ImVec4 Button_Active = ImColor(33, 33, 33, 255);

    inline ImVec4 Selectable_Hovered = ImColor(150, 150, 150, 255);
    inline ImVec4 Selectable_Active = ImColor(250, 250, 250, 255);

    inline ImVec4 Picker_Active = ImColor(20, 20, 20, 255);

    inline ImVec4 Text = ImColor(71, 71, 71, 255);
    inline ImVec4 Text_Hovered = ImColor(200, 200, 200, 255);
    inline ImVec4 Text_Active = ImColor(235, 245, 255, 255);

    inline ImVec4 CheckMark = ImColor(255, 255, 255, 255);

    inline ImVec4 Transparent = ImColor(0, 0, 0, 0);

    inline ImVec4 tab_tooltip = ImColor(180, 180, 180, 255);

    inline ImVec4 keybind_background = ImColor(17, 17, 17, 255);
    inline ImVec4 keybind_border = ImColor(28, 28, 28, 255);
}

namespace theme {
    inline float accent[3] = { 0.855f, 0.376f, 0.090f };
    inline float bg[3] = { 0.063f, 0.063f, 0.063f };
    inline float highlight[3] = { 0.294f, 0.196f, 0.929f };

    inline void updateColors()
    {
        float bgDim = 0.8f;
        float bgLight = 1.2f;

        colors::Car_Slider = ImColor(accent[0], accent[1], accent[2], 1.0f);
        colors::Car_Slider_Hovered = ImColor(accent[0] * 1.05f, accent[1] * 1.05f, accent[2] * 1.05f, 1.0f);
        colors::Car_Slider_Active = ImColor(accent[0] * 1.05f, accent[1] * 1.05f, accent[2] * 1.05f, 1.0f);

        colors::Checkbox_Active = ImColor(accent[0], accent[1], accent[2], 1.0f);

        colors::Tab_Selected = ImColor(highlight[0], highlight[1], highlight[2], 1.0f);

        colors::main_color = ImColor(bg[0], bg[1], bg[2], 1.0f);
        colors::lite_color = ImColor(bg[0] * bgLight, bg[1] * bgLight, bg[2] * bgLight, 1.0f);
        colors::Slider = ImColor(bg[0], bg[1], bg[2], 1.0f);
        colors::Slider_Hovered = ImColor(bg[0] * bgLight, bg[1] * bgLight, bg[2] * bgLight, 1.0f);
        colors::Slider_Active = ImColor(bg[0] * bgLight, bg[1] * bgLight, bg[2] * bgLight, 1.0f);
        colors::InputText = ImColor(bg[0], bg[1], bg[2], 1.0f);
        colors::Button = ImColor(bg[0] * bgLight, bg[1] * bgLight, bg[2] * bgLight, 1.0f);
        colors::Tab_Child = ImColor(bg[0] * bgDim, bg[1] * bgDim, bg[2] * bgDim, 1.0f);
    }
}
