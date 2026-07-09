#pragma once
#include <windows.h>

enum InjectedInputMouseOptions
{
    move = 1,
    left_down = 2,
    left_up = 4,
    right_down = 16,
    right_up = 8,
    none = 0
};

struct InjectedInputMouseInfo
{
    int move_direction_x;
    int move_direction_y;
    unsigned int mouse_data;
    InjectedInputMouseOptions mouse_options;
    unsigned int time_offset_in_miliseconds;
    void* extra_info;
};