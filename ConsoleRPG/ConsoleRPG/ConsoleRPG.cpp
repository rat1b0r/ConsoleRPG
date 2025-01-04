#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>

using namespace std;
using std::cout;

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
void MoveCursorToСoordinates(int a, int b) {
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

// считаем количество цифр в числе
int countDigits(int number) {
    return std::to_string(abs(number)).length();
}

// структура для шкал
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
    // зачатки менеджера анимаций
    bool need = true;
    int count = 0;
    int pause = 0;
    int animationSpeed = 2;
    int deletingSpeed = 6;
    int duration = 11;
    int saveCells, reduceCells, deletingCursor, deletingValues;
    Animation() {
    }

    // анимация шкал
    void AnimationDamage(int valueCurrent, int valueMax, BarData* barData, int coordinateX, int coordinateYBar, int barLength, int _enemyDamage) {
        int barCenter = valueMax / 2;

        if (need == true) {
            if (count == 0 && pause == 0) {
                // начало анимации, первый кадр
                saveCells = (valueCurrent - _enemyDamage) / 5;
                reduceCells = barLength - saveCells;
                deletingCursor = coordinateYBar + (valueCurrent / 5) - 1;
                deletingValues = valueCurrent - _enemyDamage;
                MoveCursorToСoordinates(coordinateX, coordinateYBar + saveCells);
                for (int i = coordinateYBar + saveCells; i < coordinateYBar + (valueCurrent / 5); i++) {
                    SetColor(black , white_bg);
                    cout << barData[i-coordinateYBar].barText;
                }
                count++;

            }
            else if (pause < animationSpeed) {
                // обработка паузы
                pause++;
            }
            else if (pause == animationSpeed && count < duration) {
                // второй и последующие кадры
                MoveCursorToСoordinates(coordinateX, coordinateYBar + saveCells);
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
                // плавное удаление закрашенных ячеек
                if (pause != deletingSpeed) {
                    pause++;
                }
                else if (deletingCursor >= coordinateYBar + saveCells) {
                    // еще не доудаляли
                    MoveCursorToСoordinates(coordinateX, deletingCursor);
                    barData[deletingCursor - coordinateYBar].barBackgroundColor = black_bg;
                    SetColor(barData[deletingCursor-coordinateYBar].barTextColor, barData[deletingCursor-coordinateYBar].barBackgroundColor);
                    //сам момент удаления
                    cout << barData[deletingCursor-coordinateYBar].barText;
                    // пересохраняем актуальное количество жизней в тексте и выводим на шкалу
                    if (deletingValues / 5 > 0) {
                        deletingValues -= 5;

                        /////////
                        //////////
                        ///////////
                        for (int i = (barCenter - 4); i < (barCenter - 2); i++) {
                            MoveCursorToСoordinates(coordinateX, i);
                            cout << barData[i - 1].barText;
                        }
                        
                    }
                    else {

                    }

                    pause = 0;
                    deletingCursor--;
                }
                else {
                    // доудаляли, меняем бул на false, обнуляем переменные
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
    // инициализация шкал
    void InitializeBar(int currentValue, int maxValue, BarData* barData, int barLength, BackgroundColor barBackgroundColor, TextColor barTextColor, TextColor barTextColorNegative) {
        // раскрашиваем шкалу и определяем цвет текста
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
        MoveCursorToСoordinates(barX, barY);
        SetColor(white, black_bg);
        cout << description;
        MoveCursorToСoordinates(barX, barYBar + barLength);
        cout << "]";
        for (int i = 0; i < barLength; i++) {
            MoveCursorToСoordinates(barX, i+barYBar);
            SetColor(barData[i].barTextColor, barData[i].barBackgroundColor);
            cout << barData[i].barText;
        }
    }
    void UpdateBarText(int currentValue, int maxValue, int barLength, BarData* barData) {
        // создаем строку текста для шкалы
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
        // пишем текст в шкалу
        for (int i = 0; i < barLength; i++) {
            barData[i].barText = barText[i];
        }
    }
};

int main() {
    // здесь у нас поддержка русского языка в консоли
    setlocale(LC_ALL, "");

    // подготовка консоли
    int cellWidth = 12;    // Ширина одной "клеточки" (пиксели)
    int cellHeight = 18;   // Высота одной "клеточки" (пиксели)
    int width = 120;        // Количество символов по горизонтали
    int height = 50;       // Количество символов по вертикали

    SetConsoleProperties(cellWidth, cellHeight, width, height);





    /////////////////////////////////////////////////////////////
    // НАЧИНАЕТСЯ КОД ИГРЫ!!!!!!!!
    /////////////////////////////////////////////////////////////

    // отрубаем нахуй курсор консоли
    HideCursor();

    // Магический код chrono
    // Время для отслеживания FPS
    auto lastTime = std::chrono::high_resolution_clock::now();

    // счетчики fps
    int frameCount = 0;
    int fps = 0;


    // основные переменные
    Character Player(100, 100, red_bg, white, black, 100, 100, blue_bg, white, black, 100, 100, yellow_bg, black, white);
    Animation BarAnimation;

    int barHealthX = 1;
    int barHealthY = 2;
    int barHealthYBar = 16;
    int barHealthLength = Player.healthMax / 5;
    string barHealthDescription = "Здоровье     [";

    int barArmorX = 3;
    int barArmorY = 2;
    int barArmorYBar = 16;
    int barArmorLength = Player.armorMax / 5;
    string barArmorDescription = "Броня        [";

    int barEnduranceX = 5;
    int barEnduranceY = 2;
    int barEnduranceYBar = 16;
    int barEnduranceLength = Player.enduranceMax / 5;
    string barEnduranceDescription = "Выносливость [";

    int debugFPSX = 1;
    int debugFPSY = 116;
    int debugFramesX = 2;
    int debugFramesY = 116;

    int enemyDamage = 14;

    // рисуем рамку интерфейса
    SetColor(red, black_bg);
    MoveCursorToСoordinates(0, 0);
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    for (int i = 0; i < 50; i++) {
        cout << "|                                                                                                                      |" << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // рисуем шкалы
    Player.BarDraw(barHealthX, barHealthY, barHealthYBar, barHealthLength, barHealthDescription, Player.barDataHealth);
    Player.BarDraw(barArmorX, barArmorY, barArmorYBar, barArmorLength, barArmorDescription, Player.barDataArmor );
    Player.BarDraw(barEnduranceX, barEnduranceY, barEnduranceYBar, barEnduranceLength, barEnduranceDescription, Player.barDataEndurance );

    // все цвета
    MoveCursorToСoordinates(49, 2);
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
    // главный цикл игры
    ////////////////////////////////////////////////
    while (true) {
        // Инкрементируем счетчик кадров
        frameCount++;

        // Вычисляем прошедшее время
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;

        // Если прошла одна секунда, обновляем FPS
        if (elapsed.count() >= 1.0) {
            fps = frameCount;
            frameCount = 0;  // Сброс счетчика кадров
            lastTime = currentTime;  // Обновление времени
        }

        // Очищаем экран и обновляем FPS
        MoveCursorToСoordinates(debugFPSX, debugFPSY);  // Перемещаем курсор в начало экрана
        SetColor(white, black_bg);  // Белый текст на черном фоне
        cout << fps << " ";
        MoveCursorToСoordinates(debugFramesX, debugFramesY);  // Перемещаем курсор в начало экрана
        cout << frameCount << " ";



        BarAnimation.AnimationDamage(Player.healthCurrent, Player.healthMax, Player.barDataHealth, barHealthX, barHealthYBar, barHealthLength, enemyDamage);
        
        if (BarAnimation.need == false) {
            BarAnimation.need = true;
            Player.healthCurrent -= enemyDamage;
        }
        // Пауза 33 миллисекунды
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    return 0;
}

/*
                                        v. 0.0.3a 
Изменения:

    переделал шкалы через массив struct, теперь можно независимо менять текст и цвета текста и фона для удобного обращения через менеджер анимаций
    написал текст на шкалах, реализовал контрастные надписи при анимации и неодинаковой раскраске шкалы
    масштабировал анимации на остальные шкалы, теперь вообще похер где шкала и прочее, все на переменных

Известные проблемы и планы на будущее (чем ближе к концу тем позже будет реализовано +-, порядок может незначительно меняться):

    удаление ячеек не должно уходить левее нуля
    перерисовка значения шкалы (пока не получается передать объект в параметр метода, остальное в целом реализовано)
    при взаимодействии мышкой с окном консоли включается пауза до нажатия любой кнопки
    сделать смерть гг
    консоль начинает гнать при изменении размера, надо заблочить
    консоль надо подогнать под ячейки четко без запаса по краям
    реализовать движок менеджера анимаций (с обработкой перекрывающегося урона например)
    передвижение по карте с клавиатуры
    реализация тумана войны в 2 слоя (с полным незнанием и с потерей актуализации со временем)
    движок боя и первая драка
    шмотки у врагов и гг с модификаторами на статы
    прокачка уровня
    разобраться с деструкторами и поубивать персонажей



                                        v. 0.0.2a 
Изменения:

    создано консольное окно с поддержкой русского языка, фиксированным игровым полем, реалтайм циклом
    включен счетчик fps для дебага
    оптимизирован код для отрисовки только изменений
    реализован метод отрисовки шкалы здоровья
    реализован метод анимации шкалы здоровья при получении урона
    реализован прототип класса менеджера анимаций

Известные проблемы и планы на будущее (чем ближе к концу тем позже будет реализовано +-, порядок может незначительно меняться):

    сделать паузу в анимации чтобы плавно делалось, и потом чтобы по одной клеточке отнималось
    контрастные надписи на шкалах
    при взаимодействии мышкой с окном консоли включается пауза до нажатия любой кнопки
    удаление ячеек не должно уходить левее нуля
    сделать смерть гг
    консоль начинает гнать при изменении размера, надо заблочить
    консоль надо подогнать под ячейки четко без запаса по краям
    масштабировать анимации на остальные шкалы
    реализовать движок менеджера анимаций (с обработкой перекрывающегося урона например)
    передвижение по карте с клавиатуры
    реализация тумана войны в 2 слоя (с полным незнанием и с потерей актуализации со временем)
    движок боя и первая драка
    шмотки у врагов и гг с модификаторами на статы
    прокачка уровня
*/