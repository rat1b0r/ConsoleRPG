#pragma once
#include "ConsoleTools.h"

#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>

using namespace std;
using std::cout;

// ���������� ������� � ��������, ���� ��� ����� � ������ ������
int countDigits(int number);

// ��������� ��� ����
struct BarData {
    char barText;
    TextColor barTextColor;
    TextColor barTextColorNegative;
    BackgroundColor barBackgroundColor;

    BarData();
    BarData(char _barText, TextColor _barTextColor, TextColor _barTextColorNegative, BackgroundColor _barBackgroundColor);
};