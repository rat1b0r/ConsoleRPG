#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>

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

// ���������� ���, ������� ��������� ������
void MoveCursorTo�oordinates(int a, int b) {
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

// ������� ���������� ���� � �����
int countDigits(int number) {
    return std::to_string(abs(number)).length();
}

// ��������� ��� ����
struct BarData {
    char barText;
    TextColor barTextColor;
    TextColor barTextColorNegative;
    BackgroundColor barBackgroundColor;

    BarData() : barText(' '), barTextColor(white), barTextColorNegative(black), barBackgroundColor(black_bg) {}

    BarData (char _barText, TextColor _barTextColor, TextColor _barTextColorNegative, BackgroundColor _barBackgroundColor) {
        barText = _barText;
        barTextColor = _barTextColor;
        barTextColorNegative = _barTextColorNegative;
        barBackgroundColor = _barBackgroundColor;
    }
};

class Animation {
public:
    // ������� ��������� ��������
    bool need = true;
    int count = 0;
    int pause = 0;
    int animationSpeed = 2;
    int deletingSpeed = 6;
    int duration = 11;
    int saveCells, reduceCells, deletingCursor, deletingValues;
    Animation() {
    }

    // �������� ����
    void AnimationDamage(int valueCurrent, int valueMax, BarData* barData, int coordinateX, int coordinateYBar, int barLength, int _enemyDamage) {
        int barCenter = valueMax / 2;

        if (need == true) {
            if (count == 0 && pause == 0) {
                // ������ ��������, ������ ����
                saveCells = (valueCurrent - _enemyDamage) / 5;
                reduceCells = barLength - saveCells;
                deletingCursor = coordinateYBar + (valueCurrent / 5) - 1;
                deletingValues = valueCurrent - _enemyDamage;
                MoveCursorTo�oordinates(coordinateX, coordinateYBar + saveCells);
                for (int i = coordinateYBar + saveCells; i < coordinateYBar + (valueCurrent / 5); i++) {
                    SetColor(black , white_bg);
                    cout << barData[i-coordinateYBar].barText;
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
                    SetColor(black, white_bg);
                    for (int i = coordinateYBar + saveCells; i < coordinateYBar + (valueCurrent / 5); i++) {
                        cout << barData[i-coordinateYBar].barText;
                    }
                }
                else {
                    for (int i = coordinateYBar + saveCells; i < coordinateYBar + (valueCurrent / 5); i++) {
                        SetColor(barData[i-coordinateYBar].barTextColor, barData[i-coordinateYBar].barBackgroundColor);
                        cout << barData[i-coordinateYBar].barText;
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
                    barData[deletingCursor - coordinateYBar].barBackgroundColor = black_bg;
                    SetColor(barData[deletingCursor-coordinateYBar].barTextColor, barData[deletingCursor-coordinateYBar].barBackgroundColor);
                    //��� ������ ��������
                    cout << barData[deletingCursor-coordinateYBar].barText;
                    // ������������� ���������� ���������� ������ � ������ � ������� �� �����
                    if (deletingValues / 5 > 0) {
                        deletingValues -= 5;

                        /////////
                        //////////
                        ///////////
                        for (int i = (barCenter - 4); i < (barCenter - 2); i++) {
                            MoveCursorTo�oordinates(coordinateX, i);
                            cout << barData[i - 1].barText;
                        }
                        
                    }
                    else {

                    }

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
    int healthCurrent, healthMax, healthBarLength, 
        armorCurrent, armorMax, armorBarLength,
        enduranceCurrent, enduranceMax, enduranceBarLength;

    BarData* barDataHealth;
    BarData* barDataArmor;
    BarData* barDataEndurance;

    Character(int _healthCurrent, int _healthMax, BackgroundColor _barHealthBGColor, TextColor _barHealthTextColor, TextColor _barHealthTextColorNegative,
              int _armorCurrent, int _armorMax, BackgroundColor _barArmorBGColor, TextColor _barArmorTextColor, TextColor _barArmorTextColorNegative,
              int _enduranceCurrent, int _enduranceMax, BackgroundColor _barEnduranceBGColor, TextColor _barEnduranceTextColor, TextColor _barEnduranceTextColorNegative) {
        healthCurrent = _healthCurrent;
        healthMax = _healthMax;
        armorCurrent = _armorCurrent;
        armorMax = _armorMax;
        enduranceCurrent = _enduranceCurrent;
        enduranceMax = _enduranceMax;

        healthBarLength = (healthMax / 5) + 1;
        if (healthMax % 5 == 0) {
            healthBarLength--;
        }
        armorBarLength = (armorMax / 5) + 1;
        if (armorMax % 5 == 0) {
            armorBarLength--;
        }
        enduranceBarLength = (enduranceMax / 5) + 1;
        if (enduranceMax % 5 == 0) {
            enduranceBarLength--;
        }


        barDataHealth = new BarData[healthMax];
        barDataArmor = new BarData[armorMax];
        barDataEndurance = new BarData[enduranceMax];
        InitializeBar(healthCurrent, healthMax, barDataHealth, healthBarLength, _barHealthBGColor, _barHealthTextColor, _barHealthTextColorNegative);
        InitializeBar(armorCurrent, armorMax, barDataArmor, armorBarLength, _barArmorBGColor, _barArmorTextColor, _barArmorTextColorNegative);
        InitializeBar(enduranceCurrent, enduranceMax, barDataEndurance, enduranceBarLength, _barEnduranceBGColor, _barEnduranceTextColor, _barEnduranceTextColorNegative);

    }
    // ������������� ����
    void InitializeBar(int currentValue, int maxValue, BarData* barData, int barLength, BackgroundColor barBackgroundColor, TextColor barTextColor, TextColor barTextColorNegative) {
        // ������������ ����� � ���������� ���� ������
        for (int i = 0; i < barLength; i++) {
            if (currentValue / 5 > i) {
                barData[i].barBackgroundColor = barBackgroundColor;
                barData[i].barTextColor = barTextColor;
            }
            else {
                barData[i].barBackgroundColor = black_bg;
                barData[i].barTextColor = barTextColorNegative;
            }
        }
        UpdateBarText(currentValue, maxValue, barLength, barData);
    }
    void BarDraw(int barX, int barY, int barYBar, int barLength, string description, BarData* barData) {
        MoveCursorTo�oordinates(barX, barY);
        SetColor(white, black_bg);
        cout << description;
        MoveCursorTo�oordinates(barX, barYBar + barLength);
        cout << "]";
        for (int i = 0; i < barLength; i++) {
            MoveCursorTo�oordinates(barX, i+barYBar);
            SetColor(barData[i].barTextColor, barData[i].barBackgroundColor);
            cout << barData[i].barText;
        }
    }
    void UpdateBarText(int currentValue, int maxValue, int barLength, BarData* barData) {
        // ������� ������ ������ ��� �����
        string barText(barLength, ' ');
        int barCenter = barLength / 2;
        int currentTemp = currentValue;
        int maxTemp = maxValue;
        int currentTempLength = countDigits(currentTemp);
        int maxTempLength = countDigits(maxTemp);

        if (currentTemp / 100 > 0) {
            barText[barCenter - 4] = '0' + (currentTemp / 100);
        }
        currentTemp -= (currentTemp / 100) * 100;
        if (currentTemp / 10 > 0) {
            barText[barCenter - 3] = '0' + (currentTemp / 10);
        }
        else if (currentTempLength >= 2) {
            barText[barCenter - 3] = '0' + (currentTemp / 10);
        }
        currentTemp -= (currentTemp / 10) * 10;
        if (currentTemp / 1 > 0) {
            barText[barCenter - 2] = '0' + currentTemp;
        }
        else if (currentTempLength >= 1) {
            barText[barCenter - 2] = '0' + currentTemp;
        }

        barText[barCenter - 1] = '/';

        if (maxTemp / 100 > 0) {
            barText[barCenter] = '0' + (maxTemp / 100);
        }
        maxTemp -= (maxTemp / 100) * 100;
        if (maxTemp / 10 > 0) {
            barText[barCenter + 1] = '0' + (maxTemp / 10);
        }
        else if (maxTempLength >= 2) {
            barText[barCenter + 1] = '0' + (maxTemp / 10);
        }
        maxTemp -= (maxTemp / 10) * 10;
        if (maxTemp / 1 > 0) {
            barText[barCenter + 2] = '0' + maxTemp;
        }
        else if (maxTempLength >= 1) {
            barText[barCenter + 2] = '0' + maxTemp;
        }
        // ����� ����� � �����
        for (int i = 0; i < barLength; i++) {
            barData[i].barText = barText[i];
        }
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
    Character Player(100, 100, red_bg, white, black, 100, 100, blue_bg, white, black, 100, 100, yellow_bg, black, white);
    Animation BarAnimation;

    int barHealthX = 1;
    int barHealthY = 2;
    int barHealthYBar = 16;
    int barHealthLength = Player.healthMax / 5;
    string barHealthDescription = "��������     [";

    int barArmorX = 3;
    int barArmorY = 2;
    int barArmorYBar = 16;
    int barArmorLength = Player.armorMax / 5;
    string barArmorDescription = "�����        [";

    int barEnduranceX = 5;
    int barEnduranceY = 2;
    int barEnduranceYBar = 16;
    int barEnduranceLength = Player.enduranceMax / 5;
    string barEnduranceDescription = "������������ [";

    int debugFPSX = 1;
    int debugFPSY = 116;
    int debugFramesX = 2;
    int debugFramesY = 116;

    int enemyDamage = 14;

    // ������ ����� ����������
    SetColor(red, black_bg);
    MoveCursorTo�oordinates(0, 0);
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    for (int i = 0; i < 50; i++) {
        cout << "|                                                                                                                      |" << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // ������ �����
    Player.BarDraw(barHealthX, barHealthY, barHealthYBar, barHealthLength, barHealthDescription, Player.barDataHealth);
    Player.BarDraw(barArmorX, barArmorY, barArmorYBar, barArmorLength, barArmorDescription, Player.barDataArmor );
    Player.BarDraw(barEnduranceX, barEnduranceY, barEnduranceYBar, barEnduranceLength, barEnduranceDescription, Player.barDataEndurance );

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



        BarAnimation.AnimationDamage(Player.healthCurrent, Player.healthMax, Player.barDataHealth, barHealthX, barHealthYBar, barHealthLength, enemyDamage);
        
        if (BarAnimation.need == false) {
            BarAnimation.need = true;
            Player.healthCurrent -= enemyDamage;
        }
        // ����� 33 ������������
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    return 0;
}

/*
                                        v. 0.0.3a 
���������:

    ��������� ����� ����� ������ struct, ������ ����� ���������� ������ ����� � ����� ������ � ���� ��� �������� ��������� ����� �������� ��������
    ������� ����� �� ������, ���������� ����������� ������� ��� �������� � ������������ ��������� �����
    ������������� �������� �� ��������� �����, ������ ������ ����� ��� ����� � ������, ��� �� ����������

��������� �������� � ����� �� ������� (��� ����� � ����� ��� ����� ����� ����������� +-, ������� ����� ������������� ��������):

    �������� ����� �� ������ ������� ����� ����
    ����������� �������� ����� (���� �� ���������� �������� ������ � �������� ������, ��������� � ����� �����������)
    ��� �������������� ������ � ����� ������� ���������� ����� �� ������� ����� ������
    ������� ������ ��
    ������� �������� ����� ��� ��������� �������, ���� ���������
    ������� ���� ��������� ��� ������ ����� ��� ������ �� �����
    ����������� ������ ��������� �������� (� ���������� ���������������� ����� ��������)
    ������������ �� ����� � ����������
    ���������� ������ ����� � 2 ���� (� ������ ��������� � � ������� ������������ �� ��������)
    ������ ��� � ������ �����
    ������ � ������ � �� � �������������� �� �����
    �������� ������
    ����������� � ������������� � ��������� ����������



                                        v. 0.0.2a 
���������:

    ������� ���������� ���� � ���������� �������� �����, ������������� ������� �����, �������� ������
    ������� ������� fps ��� ������
    ������������� ��� ��� ��������� ������ ���������
    ���������� ����� ��������� ����� ��������
    ���������� ����� �������� ����� �������� ��� ��������� �����
    ���������� �������� ������ ��������� ��������

��������� �������� � ����� �� ������� (��� ����� � ����� ��� ����� ����� ����������� +-, ������� ����� ������������� ��������):

    ������� ����� � �������� ����� ������ ��������, � ����� ����� �� ����� �������� ����������
    ����������� ������� �� ������
    ��� �������������� ������ � ����� ������� ���������� ����� �� ������� ����� ������
    �������� ����� �� ������ ������� ����� ����
    ������� ������ ��
    ������� �������� ����� ��� ��������� �������, ���� ���������
    ������� ���� ��������� ��� ������ ����� ��� ������ �� �����
    �������������� �������� �� ��������� �����
    ����������� ������ ��������� �������� (� ���������� ���������������� ����� ��������)
    ������������ �� ����� � ����������
    ���������� ������ ����� � 2 ���� (� ������ ��������� � � ������� ������������ �� ��������)
    ������ ��� � ������ �����
    ������ � ������ � �� � �������������� �� �����
    �������� ������
*/