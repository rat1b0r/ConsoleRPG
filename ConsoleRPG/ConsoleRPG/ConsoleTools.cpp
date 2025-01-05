#include "ConsoleTools.h"

// Магический код, устанавливающий размер консоли, положение и моноширинный шрифт
void SetConsoleProperties(int cellWidth, int cellHeight, int width, int height) {
    // Получаем дескриптор стандартного вывода
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Устанавливаем размер окна консоли
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1;
    windowSize.Bottom = height - 1;

    // Задаём размер окна консоли
    if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize)) {
        std::cerr << "Не удалось установить размер окна!" << std::endl;
        return;
    }

    // Устанавливаем размер буфера
    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;

    if (!SetConsoleScreenBufferSize(hConsole, bufferSize)) {
        std::cerr << "Не удалось установить размер буфера!" << std::endl;
        return;
    }

    // Устанавливаем моноширинный шрифт
    CONSOLE_FONT_INFOEX fontInfo = { 0 };
    fontInfo.cbSize = sizeof(fontInfo);
    fontInfo.nFont = 0;
    fontInfo.dwFontSize.X = cellWidth; // Ширина символа
    fontInfo.dwFontSize.Y = cellHeight; // Высота символа
    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(fontInfo.FaceName, L"Consolas"); // Устанавливаем шрифт "Consolas"
    if (!SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo)) {
        std::cerr << "Не удалось установить шрифт консоли!" << std::endl;
    }

    // Центрируем консоль на экране
    HWND consoleWindow = GetConsoleWindow();
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop); // Получаем размер экрана
    int screenWidth = desktop.right;
    int screenHeight = desktop.bottom;

    // Вычисляем размеры окна консоли в пикселях
    int consolePixelWidth = cellWidth * width;
    int consolePixelHeight = cellHeight * height;

    // Центрируем
    int x = (screenWidth - consolePixelWidth) / 2;
    int y = (screenHeight - consolePixelHeight) / 2;

    // TODO починить эту тупую залупу с +100
    MoveWindow(consoleWindow, x, y, consolePixelWidth + 30, consolePixelHeight + 100, TRUE);
}

// Магический код, который задает цвет фона и текста
void SetColor(TextColor text, BackgroundColor background) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, text | background);
}

// Магический код, который переносит курсор
void MoveCursorToCoordinates(int a, int b) {
    COORD coord = { b, a };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Магический код, который прячет курсор консоли
void HideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // Отключаем видимость курсора
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// Магический код, который показывает курсор консоли
void ShowCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // Включаем видимость курсора
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}