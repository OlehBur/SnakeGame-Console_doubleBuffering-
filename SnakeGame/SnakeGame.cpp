#include <iostream>
#include <conio.h> //get buttons clicks
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <cstdlib>

#pragma comment(lib,"Winmm.lib")

using namespace std;

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) {
        this->x = x;
        this->y = y;
    };
    inline bool operator==(Point p) {
        return (x == p.x && y == p.y) ? true : false;
    };
};

enum class eDir { Pause, Right, Left, Up, Down, Close };//вичеслення
eDir dir;
double curPerc = 0;
bool endGame = false;
bool lose = false;
bool isBoost;
bool upLevel;
int width = 30;
int height = 15;
int maxSize = (width - 2) * (height - 2);
int score = 0;
int prevlvlScore = 0;
int countBoost = 0;
int timerBoost = 0;
int lvlBoost = 0;
Point head(0, 0), food(0, 0), boost(1, 1);
Point* bodyParts = new Point[maxSize];
int currentCountParts = 0;
TCHAR eatSong[9] = _T("eat.wav");
TCHAR upScoreSong[12] = _T("upScore.wav");
TCHAR wonSong[8] = _T("won.wav");
TCHAR loseSong[9] = _T("lose.wav");

HANDLE hBufferMain = NULL;
HANDLE hBufferSecond = NULL;

const int nScreenWidth = 300;
const int nScreenHeight = 120;

short iCurrentPosX = 0;
short iCurrentPosY = 0;

DWORD dwBytesWritten = 0;

void ColorConsoleSumbol(const char* pStr, WORD color) {
    int iLen = strlen(pStr);

    for (int i = 0; i < iLen; ++i)
    {
        WORD attribute = color;
        WriteConsoleOutputAttribute(hBufferMain, &attribute, 1, { iCurrentPosX, iCurrentPosY }, &dwBytesWritten);
        WriteConsoleOutputCharacterA(hBufferMain, pStr + i, 1, { iCurrentPosX, iCurrentPosY }, &dwBytesWritten);

        if (iCurrentPosX + 1 >= nScreenWidth || pStr[i] == '\n')
        {
            iCurrentPosX = 0;
            iCurrentPosY++;
        }
        else
            iCurrentPosX++;
    }
}

void ColorConsoleDouble(double num, WORD color) {
    char chBuff[128];
    sprintf(chBuff, "%.1f", (float)num);

    ColorConsoleSumbol(chBuff, color);
}

void ColorConsoleInt(int num, WORD color) {
    char chBuff[128];
    sprintf(chBuff, "%d", num);

    ColorConsoleSumbol(chBuff, color);
}

void MovConsole(int iX, int iY) {
    iCurrentPosX = iX;
    iCurrentPosY = iY;
}

void FlushMainBuffer() {
    char chBuffer[nScreenWidth * nScreenHeight];
    memset(chBuffer, ' ', nScreenWidth * nScreenHeight);

    WriteConsoleOutputCharacterA(hBufferMain, chBuffer, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
}

void EndWindow(bool modeWin) {
    string resultTittle;
    int color = 0;
    if (modeWin) {
        color = 11;
        resultTittle = "ВИ_ПЕРЕМОГЛИ!";
        PlaySound(wonSong, NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        color = 12;
        resultTittle = "ГРУ-ЗАКІНЧЕНО";
        PlaySound(loseSong, NULL, SND_FILENAME | SND_ASYNC);
    }
    MovConsole(0, 8);
    for (int i = 0; i < 115 + width; i++)
        ColorConsoleSumbol(" ", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 9);
    for (int i = 0; i < 115 + width; i++)
        ColorConsoleSumbol(" ", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 10);
    for (int i = 0; i < 90 + width; i++)
        ColorConsoleSumbol("X", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 11);
    ColorConsoleSumbol("||", color);
    for (int i = 0; i < 48; i++)
        ColorConsoleSumbol(" ", color);
    MovConsole(49, 11);

    ColorConsoleSumbol(resultTittle.c_str(), 15);
    MovConsole(62, 11);
    for (int i = 0; i < 56; i++)
        ColorConsoleSumbol(" ", color);
    ColorConsoleSumbol("||", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 12);
    for (int i = 0; i < 90 + width; i++)
        ColorConsoleSumbol("X", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 13);
    for (int i = 0; i < 115 + width; i++)
        ColorConsoleSumbol(" ", color);
    ColorConsoleSumbol("\n", color);
    MovConsole(0, 14);
    for (int i = 0; i < 115 + width; i++)
        ColorConsoleSumbol(" ", color);
    ColorConsoleSumbol("\n", color);

    system("pause");
}

void Vialize() {
    for (int i = 0; i < width + 2; i++)
        ColorConsoleSumbol("#", 1);
    ColorConsoleSumbol("\n", 1);

    Point currPoint;
    for (int y = 0; y <= height; y++) {
        for (int x = 0; x <= width + 1; x++) {
            currPoint = { x, y };
            if (x == 0 || x == width) {
                ColorConsoleSumbol("#", 1);
            }
            if (currPoint == head) {
                ColorConsoleSumbol("O", 10);
            }
            else if (currPoint == food) {
                ColorConsoleSumbol("*", 11 + rand() % 4);
            }
            else if (currPoint == boost/* && !isBoost*/) {
                //isBoost = true;
                ColorConsoleSumbol("@", 22);
                ColorConsoleSumbol("", 15);
            }
            else {
                bool isBody = false;
                for (int part = 0; part < currentCountParts; part++)
                    if (bodyParts[part] == currPoint) {
                        if (!countBoost)
                            ColorConsoleSumbol("o", 2);
                        else
                            ColorConsoleSumbol("o", 5);
                        isBody = true;
                    }
                if (!isBody)
                    ColorConsoleSumbol(" ", 15);
            }
            if (!isBoost)
                timerBoost++;
        }
        ColorConsoleSumbol("\n", 15);
    }
    for (int i = 0; i < width + 2; i++)
        ColorConsoleSumbol("#", 1);
    ColorConsoleSumbol("\n", 1);

    MovConsole(0, 18);
    ColorConsoleSumbol("#", 9);

    ColorConsoleSumbol("           Очки: ", 8);
    ColorConsoleInt(score, 15);
    ColorConsoleSumbol("\n", 8);

    MovConsole(width + 1, 18);
    ColorConsoleSumbol("#", 9);
    MovConsole(0, 19);
    for (int i = 0; i < width + 2; i++)
        ColorConsoleSumbol("#", 9);

    MovConsole(width + 30, 18);
    ColorConsoleSumbol("By Oleh Burdeyniy.\n\n\n", 8);
    /*cout << "Timer: " << timerBoost/1000;*/

    MovConsole(width + 2, 0);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("-", 8);
    MovConsole(width + 2, 1);
    ColorConsoleSumbol("       << Для керування змійкою використовуйте стрілки або клавіші : A, S, D, W >> ", 14);

    MovConsole(width + 2, 2);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("-", 8);

    MovConsole(width + 2, 3);
    ColorConsoleSumbol("                        << Щоб перейти до паузи, натискайте: P >>", 10);

    MovConsole(width + 2, 4);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("-", 8);

    MovConsole(width + 2, 5);
    ColorConsoleSumbol("                         << Для виходу з програми натисніть: X >>", 12);

    MovConsole(width + 2, 6);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("-", 8);

    MovConsole(width + 2, 7);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("_", 15);

    MovConsole(width + 2, 8);
    ColorConsoleSumbol("                  << Збирайте їжу та отримуйте по 5 очок -> ", 14);
    ColorConsoleSumbol("*", 11 + rand() % 4);
    ColorConsoleSumbol(" >>     ", 14);

    MovConsole(width + 2, 9);
    ColorConsoleSumbol("         << Їжте, щоб отримати додатковий бонус перелазу через хвіст -> ", 2);
    ColorConsoleSumbol("@", 22);
    ColorConsoleSumbol(" >>     ", 2);

    MovConsole(width + 2, 10);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("_", 15);

    MovConsole(width + 2, 11);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("*", 8);

    MovConsole(width + 2, 12);


    ColorConsoleSumbol("           >>> Прогрес Росту (", 11);
    ColorConsoleDouble(curPerc, 7);
    ColorConsoleSumbol("%) ", 11);

    ColorConsoleSumbol("|", 15);
    ColorConsoleSumbol("0", 3);
    for (int i = 0; i < currentCountParts; i++)
        ColorConsoleSumbol("0", 3);
    ColorConsoleSumbol("|", 12);
    for (int i = 0; i <= 70 - width - currentCountParts; i++)
        ColorConsoleSumbol("0", 8);
    ColorConsoleSumbol("|", 15);

    MovConsole(width + 2, 13);
    for (int i = 0; i < 115 - width; i++)
        ColorConsoleSumbol("_", 15);

    MovConsole(width + 2, 14);
    ColorConsoleSumbol("           >>> Доступних > ", 4);
    ColorConsoleSumbol("@", 22);
    ColorConsoleSumbol(" < -- ", 4);
    ColorConsoleInt(countBoost, 7);
}

void ButtonsCallback() {
    if (_kbhit())
        switch (_getch()) {
        case 97:    //228
            dir = eDir::Left;
            break;
        case 115:   //63
            dir = eDir::Down;
            break;
        case 100:   //162
            dir = eDir::Right;
            break;
        case 119:   //50
            dir = eDir::Up;
            break;
            //case 224:
        case 75:
            dir = eDir::Left;
            break;
        case 80:
            dir = eDir::Down;
            break;
        case 77:
            dir = eDir::Right;
            break;
        case 72:
            dir = eDir::Up;
            break;
        case 112:
            dir = eDir::Pause;
            break;
        case 120://x 120 231
            dir = eDir::Close;
        }
}

void BoostRandSpawn() {
    boost = { -width / 4 + rand() % (width + width / 3), -height / 3 + rand() % (height + height / 3) };
    for (int part = 0; part < currentCountParts; part++)
        if (boost == bodyParts[part] || boost.y == 0 || boost.x == 0)
            BoostRandSpawn();
}

void FoodRandomSpawn() {
    food = { rand() % width, rand() % height };

    for (int part = 0; part < currentCountParts; part++)
        if (food == bodyParts[part])
            FoodRandomSpawn();
}

int Reactions() {
    if (currentCountParts) {
        curPerc = (currentCountParts * 100) / 40;
        if ((int)curPerc - prevlvlScore >= 100 + 2 * lvlBoost) {
            lvlBoost++;
            upLevel = true;
        }
    }
    Point prevBody = bodyParts[0];
    bodyParts[0] = head;
    Point prePrevBody;
    for (int part = 1; part < currentCountParts; part++) {//tail phis
        prePrevBody = bodyParts[part];
        bodyParts[part] = prevBody;
        prevBody = prePrevBody;
    }

    switch (dir)
    {
    case eDir::Left:
        head.x--;
        break;

    case eDir::Right:
        head.x++;
        break;

    case eDir::Down:
        head.y++;
        break;

    case eDir::Up:
        head.y--;
        break;

    case eDir::Close:
        exit(0);
        break;

    case eDir::Pause:
        return 0;
    }

    if (head.x > width - 1)
        head.x = 0;
    else if (head.x <= -1)
        head.x = width - 1;
    if (head.y > height)
        head.y = 0;
    else if (head.y <= -1)
        head.y = height - 1;

    for (int part = 0; part < currentCountParts; part++)
        if (bodyParts[part] == head && !countBoost) {
            endGame = true;
            EndWindow(0);
            return 0;
        }

        else if (bodyParts[part] == head && countBoost)
            countBoost--;

    if (food == head) {
        PlaySound(eatSong, NULL, SND_FILENAME | SND_ASYNC);
        currentCountParts++;
        score += 5 + lvlBoost;
        FoodRandomSpawn();
        if (score % 100 == 0)
            PlaySound(upScoreSong, NULL, SND_FILENAME | SND_ASYNC);
    }
    if (boost == head) {
        countBoost++;
        BoostRandSpawn();
    }
    if (timerBoost > 50000) {
        isBoost = false;
        timerBoost = 0;
        BoostRandSpawn();
    }
    return 1;
}

void Start() {
    if (bodyParts)
        delete[] bodyParts;
    currentCountParts = 0;
    curPerc = 0;
    prevlvlScore = score;
    bodyParts = new Point[maxSize];
    isBoost = false;
    countBoost = 0;
    //prevDir = eDir::Stop;
    dir = eDir::Pause;
    head = { width / 2 - 1, height / 2 - 1 };
    food = { rand() % width, rand() % height };// Point(rand() % width, rand() % height);
}

void Render()
{ 
        if (upLevel) {
            EndWindow(1);
            Start();
            if (_kbhit())
                upLevel = false;
        }
        else
        {
            Vialize();
            ButtonsCallback();
            Reactions();
        }    
}

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    hBufferMain = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    hBufferSecond = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    Start();
     
    while (!endGame)
    {
        FlushMainBuffer();

        MovConsole(0, 0);

        Render();

        SetConsoleActiveScreenBuffer(hBufferMain);//double buff
        swap(hBufferMain, hBufferSecond);
    }
   
    return 0;
}
