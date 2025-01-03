#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
using namespace std;
using std::cout;

// ������������ ��� ������ ������
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

// ������������ ��� ������ ����
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

// ���������� ���, ������� ��������� ������ � ������
void MoveCursorTo�oordinates(int a, int b) {
    COORD coord = { b, a };  // ���������� �������� ������ ����
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

// ������� ��� ��������� ����
void BarDraw(int value, BackgroundColor color, int coordinateX, int coordinateY) {
    int current = value / 5;
    int last = 20 - current;

    MoveCursorTo�oordinates(coordinateX, coordinateY);
    SetColor(white, color);
    for (int i = 0; i < current; i++) {
        cout << " ";
    }
    SetColor(white, black_bg);
    for (int i = 0; i < last; i++) {
        cout << " ";
    }
}

class Animation {
public:
    // ������� ��������� ��������
    bool need = true;
    int count = 0;
    int pause = 0;
    int animationSpeed = 2;
    int deletingSpeed = 7;
    int duration = 11;
    int saveCells, reduceCells, deletingCursor;
    Animation() {
    }

    // �������� ����
    void BarAnimation(int value, BackgroundColor color, int coordinateX, int coordinateYBar, int barLength, int _enemyDamage) {


        if (need == true) {
            if (count == 0 && pause == 0) {
                // ������ ��������, ������ ����
                saveCells = (value - _enemyDamage) / 5;
                reduceCells = barLength - saveCells;
                deletingCursor = coordinateYBar + (value / 5) - 1;
                MoveCursorTo�oordinates(coordinateX, coordinateYBar + saveCells);
                for (int i = coordinateYBar + saveCells; i < coordinateYBar + (value / 5); i++) {
                    SetColor(white, white_bg);
                    cout << " ";
                }
                count++;

            }
            else if (pause < animationSpeed) {
                // ��������� �����
                pause++;
            }
            else if (pause == animationSpeed && count < duration) {
                // ������ � ����������� �����
                MoveCursorTo�oordinates(coordinateX, coordinateYBar + saveCells);
                if (count % 2 == 0) {
                    SetColor(white, white_bg);
                    for (int i = coordinateYBar + saveCells; i < coordinateYBar + (value / 5); i++) {
                        cout << " ";
                    }
                }
                else {
                    SetColor(white, color);
                    for (int i = coordinateYBar + saveCells; i < coordinateYBar + (value / 5); i++) {
                        cout << " ";
                    }
                }
                pause = 0;
                count++;
            }
            else if (count == duration) {
                // ������� �������� ����������� �����
                if (pause != deletingSpeed) {
                    pause++;
                }
                else if (deletingCursor >= coordinateYBar + saveCells) {
                    // ��� �� ���������
                    MoveCursorTo�oordinates(coordinateX, deletingCursor);
                    SetColor(white, black_bg);
                    cout << " ";
                    pause = 0;
                    deletingCursor--;
                }
                else {
                    // ���������, ������ ��� �� false, �������� ����������
                    count = 0;
                    pause = 0;
                    need = false;
                }

            }
        }

    }
};

class Character {
public:
    int health, armor, endurance;

    Character(int _health, int _armor, int _endurance) {
        health = _health;
        armor = _armor;
        endurance = _endurance;
    }
};

int main() {
    // ����� � ��� ��������� �������� ����� � �������
    setlocale(LC_ALL, "");

    // ���������� �������
    int cellWidth = 12;    // ������ ����� "��������" (�������)
    int cellHeight = 18;   // ������ ����� "��������" (�������)
    int width = 120;        // ���������� �������� �� �����������
    int height = 50;       // ���������� �������� �� ���������

    SetConsoleProperties(cellWidth, cellHeight, width, height);





    /////////////////////////////////////////////////////////////
    // ���������� ��� ����!!!!!!!!
    /////////////////////////////////////////////////////////////

    // �������� ����� ������ �������
    HideCursor();

    // ���������� ��� chrono
    // ����� ��� ������������ FPS
    auto lastTime = std::chrono::high_resolution_clock::now();

    // �������� fps
    int frameCount = 0;
    int fps = 0;


    // �������� ����������
    Character Player(100, 100, 100);
    Animation BarAnimation;

    int barHealthX = 1;
    int barHealthY = 2;
    int barHealthYBar = 16;
    int barHealthLength = Player.health / 5;

    int barArmorX = 3;
    int barArmorY = 2;
    int barArmorYBar = 16;
    int barArmorLength = Player.armor / 5;

    int barEnduranceX = 5;
    int barEnduranceY = 2;
    int barEnduranceYBar = 16;
    int barEnduranceLength = Player.endurance / 5;

    int debugFPSX = 1;
    int debugFPSY = 116;
    int debugFramesX = 2;
    int debugFramesY = 116;

    int enemyDamage = 15;

    // ������ ����� ����������
    SetColor(red, black_bg);
    MoveCursorTo�oordinates(0, 0);
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    for (int i = 0; i < 50; i++) {
        cout << "|                                                                                                                      |" << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // ������ �����
    // ��������
    SetColor(white, black_bg);
    MoveCursorTo�oordinates(barHealthX, barHealthY);
    cout << "��������     [";
    for (int i = 0; i < barHealthLength; i++) {
        cout << " ";
    }
    cout << "]";
    // �����
    SetColor(white, black_bg);
    MoveCursorTo�oordinates(barArmorX, barArmorY);
    cout << "�����        [";
    for (int i = 0; i < barArmorLength; i++) {
        cout << " ";
    }
    cout << "]";
    // ������������
    SetColor(white, black_bg);
    MoveCursorTo�oordinates(barEnduranceX, barEnduranceY);
    cout << "������������ [";
    for (int i = 0; i < barEnduranceLength; i++) {
        cout << " ";
    }
    cout << "]";

    // ��������� �����
    // ��������
    BarDraw(Player.health, red_bg, barHealthX, barHealthYBar);
    // �����
    BarDraw(Player.armor, blue_bg, barArmorX, barArmorYBar);
    // ������������
    BarDraw(Player.endurance, yellow_bg, barEnduranceX, barEnduranceYBar);

    // ��� �����
    MoveCursorTo�oordinates(49, 2);
    SetColor(white, blue_bg);
    cout << " ";
    SetColor(white, cyan_bg);
    cout << " ";
    SetColor(white, black_bg);
    cout << " ";
    SetColor(white, green_bg);
    cout << " ";
    SetColor(white, red_bg);
    cout << " ";
    SetColor(white, magenta_bg);
    cout << " ";
    SetColor(white, yellow_bg);
    cout << " ";
    SetColor(white, white_bg);
    cout << " ";









    // ���������� ��������� ����
    SetColor(white, black_bg);


    ////////////////////////////////////////////////
    // ������� ���� ����
    ////////////////////////////////////////////////
    while (true) {
        // �������������� ������� ������
        frameCount++;

        // ��������� ��������� �����
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;

        // ���� ������ ���� �������, ��������� FPS
        if (elapsed.count() >= 1.0) {
            fps = frameCount;
            frameCount = 0;  // ����� �������� ������
            lastTime = currentTime;  // ���������� �������
        }

        // ������� ����� � ��������� FPS
        MoveCursorTo�oordinates(debugFPSX, debugFPSY);  // ���������� ������ � ������ ������
        SetColor(white, black_bg);  // ����� ����� �� ������ ����
        cout << fps << " ";
        MoveCursorTo�oordinates(debugFramesX, debugFramesY);  // ���������� ������ � ������ ������
        cout << frameCount << " ";



        BarAnimation.BarAnimation(Player.health, red_bg, barHealthX, barHealthYBar, barHealthLength, enemyDamage);

        if (BarAnimation.need == false) {
            BarAnimation.need = true;
            Player.health -= enemyDamage;
        }


        // ����� 33 ������������
        std::this_thread::sleep_for(std::chrono::milliseconds(33));









    }
    return 0;
}

//TODO ������� ����� � �������� ����� ������ ��������, � ����� ����� �� ����� �������� ����������
//TODO ����������� ������� �� ������
//TODO ��� �������������� ������ � ����� ������� ���������� ����� �� ������� ����� ������
//TODO �������� ����� �� ������ ������� ����� ����
//TODO ������� ������ ��
//TODO ������� �������� ����� ��� ��������� �������, ���� ���������
//TODO ������� ���� ��������� ��� ������ ����� ��� ������ �� �����
//TODO �������������� �������� �� ��������� �����
//TODO ����������� ������ ��������� �������� (� ���������� ���������������� ����� ��������)
//TODO ������������ �� ����� � ����������
//TODO ���������� ������ ����� � 2 ���� (� ������ ��������� � � ������� ������������ �� ��������)
//TODO ������ ��� � ������ �����
//TODO ������ � ������ � �� � �������������� �� �����
//TODO �������� ������