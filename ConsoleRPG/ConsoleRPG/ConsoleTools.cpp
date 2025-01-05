#include "ConsoleTools.h"

// ���������� ���, ��������������� ������ �������, ��������� � ������������ �����
void SetConsoleProperties(int cellWidth, int cellHeight, int width, int height) {
    // �������� ���������� ������������ ������
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // ������������� ������ ���� �������
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1;
    windowSize.Bottom = height - 1;

    // ����� ������ ���� �������
    if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize)) {
        std::cerr << "�� ������� ���������� ������ ����!" << std::endl;
        return;
    }

    // ������������� ������ ������
    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;

    if (!SetConsoleScreenBufferSize(hConsole, bufferSize)) {
        std::cerr << "�� ������� ���������� ������ ������!" << std::endl;
        return;
    }

    // ������������� ������������ �����
    CONSOLE_FONT_INFOEX fontInfo = { 0 };
    fontInfo.cbSize = sizeof(fontInfo);
    fontInfo.nFont = 0;
    fontInfo.dwFontSize.X = cellWidth; // ������ �������
    fontInfo.dwFontSize.Y = cellHeight; // ������ �������
    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(fontInfo.FaceName, L"Consolas"); // ������������� ����� "Consolas"
    if (!SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo)) {
        std::cerr << "�� ������� ���������� ����� �������!" << std::endl;
    }

    // ���������� ������� �� ������
    HWND consoleWindow = GetConsoleWindow();
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop); // �������� ������ ������
    int screenWidth = desktop.right;
    int screenHeight = desktop.bottom;

    // ��������� ������� ���� ������� � ��������
    int consolePixelWidth = cellWidth * width;
    int consolePixelHeight = cellHeight * height;

    // ����������
    int x = (screenWidth - consolePixelWidth) / 2;
    int y = (screenHeight - consolePixelHeight) / 2;

    // TODO �������� ��� ����� ������ � +100
    MoveWindow(consoleWindow, x, y, consolePixelWidth + 30, consolePixelHeight + 100, TRUE);
}

// ���������� ���, ������� ������ ���� ���� � ������
void SetColor(TextColor text, BackgroundColor background) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, text | background);
}

// ���������� ���, ������� ��������� ������
void MoveCursorToCoordinates(int a, int b) {
    COORD coord = { b, a };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// ���������� ���, ������� ������ ������ �������
void HideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // ��������� ��������� �������
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// ���������� ���, ������� ���������� ������ �������
void ShowCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // �������� ��������� �������
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}