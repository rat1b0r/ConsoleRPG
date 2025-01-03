#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
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

// Магический код, который переносит курсор в начало
void MoveCursorToСoordinates(int a, int b) {
    COORD coord = { b, a };  // Координаты верхнего левого угла
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

// функция для отрисовки шкал
void BarDraw(int value, BackgroundColor color, int coordinateX, int coordinateY) {
    int current = value / 5;
    int last = 20 - current;

    MoveCursorToСoordinates(coordinateX, coordinateY);
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
    // зачатки менеджера анимаций
    bool need = true;
    int count = 0;
    int pause = 0;
    int animationSpeed = 2;
    int deletingSpeed = 7;
    int duration = 11;
    int saveCells, reduceCells, deletingCursor;
    Animation() {
    }

    // анимация шкал
    void BarAnimation(int value, BackgroundColor color, int coordinateX, int coordinateYBar, int barLength, int _enemyDamage) {


        if (need == true) {
            if (count == 0 && pause == 0) {
                // начало анимации, первый кадр
                saveCells = (value - _enemyDamage) / 5;
                reduceCells = barLength - saveCells;
                deletingCursor = coordinateYBar + (value / 5) - 1;
                MoveCursorToСoordinates(coordinateX, coordinateYBar + saveCells);
                for (int i = coordinateYBar + saveCells; i < coordinateYBar + (value / 5); i++) {
                    SetColor(white, white_bg);
                    cout << " ";
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
                // плавное удаление закрашенных ячеек
                if (pause != deletingSpeed) {
                    pause++;
                }
                else if (deletingCursor >= coordinateYBar + saveCells) {
                    // еще не доудаляли
                    MoveCursorToСoordinates(coordinateX, deletingCursor);
                    SetColor(white, black_bg);
                    cout << " ";
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
    int health, armor, endurance;

    Character(int _health, int _armor, int _endurance) {
        health = _health;
        armor = _armor;
        endurance = _endurance;
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

    // рисуем рамку интерфейса
    SetColor(red, black_bg);
    MoveCursorToСoordinates(0, 0);
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    for (int i = 0; i < 50; i++) {
        cout << "|                                                                                                                      |" << endl;
    }
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // рисуем шкалы
    // здоровье
    SetColor(white, black_bg);
    MoveCursorToСoordinates(barHealthX, barHealthY);
    cout << "Здоровье     [";
    for (int i = 0; i < barHealthLength; i++) {
        cout << " ";
    }
    cout << "]";
    // броня
    SetColor(white, black_bg);
    MoveCursorToСoordinates(barArmorX, barArmorY);
    cout << "Броня        [";
    for (int i = 0; i < barArmorLength; i++) {
        cout << " ";
    }
    cout << "]";
    // выносливость
    SetColor(white, black_bg);
    MoveCursorToСoordinates(barEnduranceX, barEnduranceY);
    cout << "Выносливость [";
    for (int i = 0; i < barEnduranceLength; i++) {
        cout << " ";
    }
    cout << "]";

    // заполняем шкалы
    // здоровье
    BarDraw(Player.health, red_bg, barHealthX, barHealthYBar);
    // броня
    BarDraw(Player.armor, blue_bg, barArmorX, barArmorYBar);
    // выносливость
    BarDraw(Player.endurance, yellow_bg, barEnduranceX, barEnduranceYBar);

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









    // возвращаем дефолтный цвет
    SetColor(white, black_bg);


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



        BarAnimation.BarAnimation(Player.health, red_bg, barHealthX, barHealthYBar, barHealthLength, enemyDamage);

        if (BarAnimation.need == false) {
            BarAnimation.need = true;
            Player.health -= enemyDamage;
        }


        // Пауза 33 миллисекунды
        std::this_thread::sleep_for(std::chrono::milliseconds(33));









    }
    return 0;
}

//TODO сделать паузу в анимации чтобы плавно делалось, и потом чтобы по одной клеточке отнималось
//TODO контрастные надписи на шкалах
//TODO при взаимодействии мышкой с окном консоли включается пауза до нажатия любой кнопки
//TODO удаление ячеек не должно уходить левее нуля
//TODO сделать смерть гг
//TODO консоль начинает гнать при изменении размера, надо заблочить
//TODO консоль надо подогнать под ячейки четко без запаса по краям
//TODO масштабировать анимации на остальные шкалы
//TODO реализовать движок менеджера анимаций (с обработкой перекрывающегося урона например)
//TODO передвижение по карте с клавиатуры
//TODO реализация тумана войны в 2 слоя (с полным незнанием и с потерей актуализации со временем)
//TODO движок боя и первая драка
//TODO шмотки у врагов и гг с модификаторами на статы
//TODO прокачка уровня