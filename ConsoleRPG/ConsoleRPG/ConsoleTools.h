#pragma once

#include <windows.h>
#include <iostream>

// Перечисление для цветов текста
enum TextColor {
    black = 0,
    blue = 1,
    green = 2,
    cyan = 3,
    red = 4,
    magenta = 5,
    yellow = 6,
    white = 7
};

// Перечисление для цветов фона
enum BackgroundColor {
    black_bg = 0,
    blue_bg = 16,
    green_bg = 32,
    cyan_bg = 48,
    red_bg = 64,
    magenta_bg = 80,
    yellow_bg = 96,
    white_bg = 112
};

void SetConsoleProperties(int cellWidth, int cellHeight, int width, int height);
void SetColor(TextColor text, BackgroundColor background);
void MoveCursorToCoordinates(int a, int b);
void HideCursor();
void ShowCursor();