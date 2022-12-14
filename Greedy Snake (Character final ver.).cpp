#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cmath>
#include <conio.h>
#include <ctime>
#include <cstring>
#include <WinCon.h>
#include <vector>
using namespace std;

#define WALL_X 1
#define WALL_Y 1
#define STAGE_WIDTH 30
#define STAGE_HEIGHT 30
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 60
#define CORNER_X 1
#define CORNER_Y 1
#define THICKNESS 1
#define MAXLENGTH 100
#define MAX_LETTER_LENGTH 20
#define MAX_RANKING_BOARD_PLAYER_NUMBER 5

#define COLOR_WALL 0x06
#define COLOR_TEXT 0x0F
#define COLOR_TEXT2 0xec
#define COLOR_TEXT3 0x7c
#define COLOR_LETTER 0x04
#define COLOR_BURST_LETTER 0x0b
#define COLOR_SNAKE_HEAD 0x09
#define COLOR_SNAKE_BODY 0x0a
#define COLOR_SNAKE_BURST_BODY 0x0c

#define CENTER_TEXT_GAMEOVER_X 10
#define CENTER_TEXT_GAMEOVER_Y 8
#define CENTER_TEXT_RESTART_X 6
#define CENTER_TEXT_RESTART_Y 9
#define CENTER_TEXT_LOAD_X 2
#define CENTER_TEXT_LOAD_Y 10
#define CENTER_TEXT_RANK_NAME_X 6
#define CENTER_TEXT_RANK_NAME_Y 11
#define CENTER_TEXT_PAUSED_X 13
#define CENTER_TEXT_PAUSED_Y 8
#define WORD_ADJUSTMENT_Y 31

#define BURST_WORD_TEXT_POSITION_X 2
#define BURST_WORD_TEXT_POSITION_Y 34

#define RANKING_QUEST_TEXT_Y 36
#define RANKING_NAME_INPUT_Y 37
#define RANK_BOARD_POS_X 80
#define RANK_BOARD_POS_Y 30

#define ORIGINAL_SKIP_TICK 150

typedef struct name_with_score{
    char name[100];
    int score;
}nws;

char name[100];

nws rankName[5];
int rankNum = 0;
bool rankOn = false;
bool ranked = false;

const int FRAME_PER_SECOND = 60;
int SKIP_TICKS = ORIGINAL_SKIP_TICK;
DWORD nextGameTick = GetTickCount();
int sleepTime = 0;

int difficulty = 1;
int difficultyCounter = 0;

FILE *wordFile;
char burstLetter;
char testLetter;
bool whetherBurstLetter;
bool whetherWordRunOut;
int letterPosInFile;
char collectLetter[MAX_LETTER_LENGTH];
int letterNum;
int wordNum;
int collectLetterPosX, collectLetterPosY;

bool whetherBursted;
int burstCounter = 0;

bool gameOver;
bool gameQuit = false;
bool gamePaused = false;

const int width = STAGE_WIDTH;
const int height = STAGE_HEIGHT;
int x, y, letterX, letterY, score;
int tailX[100], tailY[100];
vector<int> tailX2, tailY2;
int nTail;

enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
enum eFont {FULL_WIDTH = 0, HALF_WIDTH};
eDirection dir;
eFont ft;
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

void insertionSort(nws all[], int nameCount)
{
    for (int i = 0; i < nameCount; i++)
    {
        nws key = all[i];
        int j = i -1;
        while (j >= 0 && (key.score < all[j].score))
        {
            all[j + 1] = all[j];
            j--;
        }
        all[j + 1] = key;
    }
}

void convertHalfFull(char letter)
{
    switch (letter)
    {
        case 'A':
            cout << "???";
            break;
        case 'B':
            cout << "???";
            break;
        case 'C':
            cout << "???";
            break;
        case 'D':
            cout << "???";
            break;
        case 'E':
            cout << "???";
            break;
        case 'F':
            cout << "???";
            break;
        case 'G':
            cout << "???";
            break;
        case 'H':
            cout << "???";
            break;
        case 'I':
            cout << "???";
            break;
        case 'J':
            cout << "???";
            break;
        case 'K':
            cout << "???";
            break;
        case 'L':
            cout << "???";
            break;
        case 'M':
            cout << "???";
            break;
        case 'N':
            cout << "???";
            break;
        case 'O':
            cout << "???";
            break;
        case 'P':
            cout << "???";
            break;
        case 'Q':
            cout << "???";
            break;
        case 'R':
            cout << "???";
            break;
        case 'S':
            cout << "???";
            break;
        case 'T':
            cout << "???";
            break;
        case 'U':
            cout << "???";
            break;
        case 'V':
            cout << "???";
            break;
        case 'W':
            cout << "???";
            break;
        case 'X':
            cout << "???";
            break;
        case 'Y':
            cout << "???";
            break;
        case 'Z':
            cout << "???";
            break;
        default:
            return;
    }
}

void setPos(int x, int y)
{
    COORD pos;
    if (ft == HALF_WIDTH)
    {
        pos.X = x + WALL_X;
        pos.Y = y + WALL_Y;
    }
    else
    {
        pos.X = x * 2 + WALL_X * 2;
        pos.Y = y + WALL_Y;
    }
    SetConsoleCursorPosition(h, pos);
}

void eraseSnake()
{
    for (int i = 0; i < nTail; i++)
    {
        setPos(tailX[i], tailY[i]);
        if (ft == HALF_WIDTH)
            cout << " ";
        else
            cout << "  ";
    }
}

void erase()
{
    SetConsoleTextAttribute(h, COLOR_TEXT);

    setPos(letterX, letterY);
    if (ft == HALF_WIDTH)
        cout << " ";
    else
        cout << "  ";

    eraseSnake();
}

void readRankBoard()
{
    FILE *fp;
    fp = fopen("rank board.txt", "r");
    while (true)
    {
        if(fscanf(fp, "%s %d\n", rankName[rankNum].name, &rankName[rankNum].score) != EOF)
        {
            rankNum++;
        }
        else
        {
            break;
        }
    }
    rankNum--;
    fclose(fp);
}

void showRankBoard()
{
    SetConsoleTextAttribute(h, COLOR_TEXT);
    if (ft == HALF_WIDTH)
    {
        setPos(RANK_BOARD_POS_X, RANK_BOARD_POS_Y - 1);
    }
    else
    {
        setPos(RANK_BOARD_POS_X - 20, RANK_BOARD_POS_Y - 1);
    }
    cout << "????????????";
    for (int j = 0; j <= rankNum; j++)
    {
        if (ft == HALF_WIDTH)
        {
            setPos(RANK_BOARD_POS_X, RANK_BOARD_POS_Y + j);
        }
        else
        {
            setPos(RANK_BOARD_POS_X - 20, RANK_BOARD_POS_Y + j);
        }
        cout << j + 1 << "???" << rankName[j].name << " " << rankName[j].score;
    }
}

void removeRankBoard()
{
    SetConsoleTextAttribute(h, COLOR_TEXT);
    for (int i = 0; i < 6; i++)
    {
        if (ft == HALF_WIDTH)
        {
            setPos(RANK_BOARD_POS_X, RANK_BOARD_POS_Y + i - 1);
        }
        else
        {
            setPos(RANK_BOARD_POS_X - 20, RANK_BOARD_POS_Y + i - 1);
        }
        cout << "                    ";
    }
}

void init()
{
    SetConsoleTitleA("Console_Greedy_Snake");
    COORD dSize = {WINDOW_WIDTH, WINDOW_HEIGHT};
    SMALL_RECT rc;
    rc.Left = 0;
    rc.Top = 0;
    rc.Right = 160;
    rc.Bottom = 37;
    SetConsoleScreenBufferSize(h, dSize);
    SetConsoleWindowInfo(h, true, &rc);
    CONSOLE_CURSOR_INFO _cursor = {1, false};
    SetConsoleCursorInfo(h, &_cursor);

    gameOver = false;
    dir = STOP;
    ft = HALF_WIDTH;
    x = width / 2;
    y = height / 2;

    srand(time(0));

    while (true)
    {
        letterX = rand() % (width - THICKNESS) + THICKNESS;
        letterY = rand() % (height - THICKNESS) + THICKNESS;

        if (x == letterX && y == letterY)
            continue;
        else
            break;
    }

    score = 0;
    nTail = 1;

    difficulty = 1;
    difficultyCounter = 0;
    SKIP_TICKS = ORIGINAL_SKIP_TICK;

    letterPosInFile = 0;
    whetherBurstLetter = false;
    collectLetterPosX = BURST_WORD_TEXT_POSITION_X;
    collectLetterPosY = BURST_WORD_TEXT_POSITION_Y;

    whetherBursted = false;
    burstCounter = 0;

    wordNum = 0;
    letterNum = 0;
    for (int i = 0; i < MAX_LETTER_LENGTH; i++)
    {
        collectLetter[i] = '\0';
    }
    whetherWordRunOut = false;

    rankOn = false;
    ranked = false;
    readRankBoard();
}

void drawMap()
{
    system("cls");
    int textColor = COLOR_WALL;
    SetConsoleTextAttribute(h, textColor);

    if (ft == HALF_WIDTH)
    {
        setPos(-THICKNESS, -THICKNESS);
        for (int i = 0; i < width + THICKNESS * 2; i++)
        {
            cout << "#";
        }

        for (int i = 0; i < height; i++)
        {
            setPos(-THICKNESS, i);
            for (int j = 0; j < width + THICKNESS * 2; j++)
            {
                if (j == 0)
                    cout << "#";
                else if (j == width + 1)
                    cout << "#";
                else
                    cout << " ";
            }
            cout << endl;
        }

        setPos(-THICKNESS, height);
        for (int i = 0; i < width + THICKNESS * 2; i++)
        {
            cout << "#";
        }
        
    }
    else
    {
        setPos(-THICKNESS, -THICKNESS);
        for (int i = 0; i < width + THICKNESS * 2; i++)
        {
            cout << "???";
        }

        for (int i = 0; i < height; i++)
        {
            setPos(-THICKNESS, i);
            for (int j = 0; j < width + THICKNESS * 2; j++)
            {
                if (j == 0)
                    cout << "???";
                else if (j == width + 1)
                    cout << "???";
                else
                    cout << "  ";
            }
            cout << endl;
        }

        setPos(-THICKNESS, height);
        for (int i = 0; i < width + THICKNESS * 2; i++)
        {
            cout << "???";
        }
    }
}

void promptInfo(int _x, int _y)
{
    int initX = STAGE_WIDTH + 4, initY = 0;

    SetConsoleTextAttribute(h, COLOR_TEXT);
    setPos(_x + initX, _y + initY);
    cout << "??? ????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "   ??????????????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ???????????????????????????????????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ?????????????????????G??????????????????????????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "       ????????????????????????????????????????????????????????????????????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "       ?????????????????????????????????????????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ?????????????????????????????????????????????????????????????????????????????? - 1???* 5?????????";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ?????????????????????????????????10";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "??? ????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ???????????????esc";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ????????????/???????????????R";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ??????/??????????????????I";
    
    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "??? ????????????????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ???????????????A / ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ???????????????D / ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ???????????????W / ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ???????????????S / ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ?????????????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "??? ????????????????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ?????????S";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ?????????L";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "   ??? ??????????????????????????????";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "??? ?????????????????????" << difficulty << " ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "??? ???????????????" << score << "  ???";

    initY++;
    setPos(_x + initX, _y + initY);
    cout << "??? ?????????????????????" << wordNum << " ???";

    initY += 2;
    setPos(_x + initX, _y + initY);
    cout << "??? ????????????Queen";
}

void restart()
{
    SetConsoleTextAttribute(h, COLOR_TEXT);
    setPos(letterX, letterY);
    if (ft == HALF_WIDTH)
        cout << " ";
    else
        cout << "  ";

    eraseSnake();

    if (ft == HALF_WIDTH)
    {
        setPos(CENTER_TEXT_GAMEOVER_X ,CENTER_TEXT_GAMEOVER_Y);
        cout << "               ";
        setPos(CENTER_TEXT_RESTART_X, CENTER_TEXT_RESTART_Y);
        cout << "                      ";
        setPos(CENTER_TEXT_LOAD_X, CENTER_TEXT_LOAD_Y);
        cout << "                           ";
        setPos(CENTER_TEXT_RANK_NAME_X, CENTER_TEXT_RANK_NAME_Y);
        cout << "                      ";
    }
    else
    {
        setPos(CENTER_TEXT_GAMEOVER_X ,CENTER_TEXT_GAMEOVER_Y);
        cout << "               ";
        setPos(CENTER_TEXT_RESTART_X, CENTER_TEXT_RESTART_Y);
        cout << "                                          ";
        setPos(CENTER_TEXT_LOAD_X, CENTER_TEXT_LOAD_Y);
        cout << "                                           ";
        setPos(CENTER_TEXT_RANK_NAME_X, CENTER_TEXT_RANK_NAME_Y);
        cout << "                               ";
    }
    

    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    letterX = rand() % (width - THICKNESS) + THICKNESS;
    letterY = rand() % (height - THICKNESS) + THICKNESS;

    score = 0;
    nTail = 1;

    difficulty = 1;
    difficultyCounter = 0;
    SKIP_TICKS = ORIGINAL_SKIP_TICK;

    letterPosInFile = 0;
    whetherBurstLetter = false;
    collectLetterPosX = BURST_WORD_TEXT_POSITION_X;
    collectLetterPosY = BURST_WORD_TEXT_POSITION_Y;

    whetherBursted = false;
    burstCounter = 0;

    wordNum = 0;
    letterNum = 0;
    for (int i = 0; i < MAX_LETTER_LENGTH; i++)
    {
        collectLetter[i] = '\0';
    }
    whetherWordRunOut = false;

    rankNum = 0;
    ranked = false;
    readRankBoard();
}

void logic()
{
    switch(dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }

    if (ft == HALF_WIDTH)
    {
        if (x > width - 1)
        {
            x = 0;
        }

        if (y > height - 1)
        {
            y = 0;
        }

        if (x < 0)
        {
            x = width - 1;
        }

        if (y < 0)
        {
            y = height - 1;
        }
    }
    else
    {
        if (x > width - 1)
        {
            x = 0;
        }

        if (y > height - 1)
        {
            y = 0;
        }

        if (x < 0)
        {
            x = width - 1;
        }

        if (y < 0)
        {
            y = height - 1;
        }
    }
    
    if (burstCounter > 0)
    {
        burstCounter--;
    }
    else
    {
        for (int i = 0; i < nTail; i++)
        {
            if (i == 0)
                continue;

            if (tailX[i] == x && tailY[i] == y)
                gameOver = true;
        }
        whetherBursted = false;
    }

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    if (x == letterX && y == letterY)
    {
        score += 10;

        bool letterPosCtrl = true;
        while (letterPosCtrl)
        {
            letterX = rand() % (width- 1) + 1;
            letterY = rand() % (height - 1) + 1;
            for (int i = 0; i < nTail; i++)
            {
                if (letterX == tailX[i] && letterY == tailY[i])
                {
                    letterPosCtrl = true;
                }
                else
                {
                    letterPosCtrl = false;
                }
            }
        }
        
        nTail++;
        difficultyCounter++;

        if (whetherBurstLetter == true)
        {
            whetherBurstLetter = false;
            whetherBursted = true;
            burstCounter = 30;
            collectLetter[letterNum] = burstLetter;
            letterNum++;

            if (testLetter == '@')
            {
                testLetter = '\0';
                for (int i = 0; i < MAX_LETTER_LENGTH; i++)
                {
                    collectLetter[i] = '\0';
                }
                wordNum++;
                score += 200;
                nTail -= 5 * (letterNum - 1);
                letterNum = 0;
                letterPosInFile += 3;
            }
        }
    }

    if (score % 50 == 0 && score != 0 && whetherBurstLetter == false)
    {
        wordFile = fopen("word.txt", "r");
        if(!fseek(wordFile, letterPosInFile, SEEK_SET))
        {
            fscanf(wordFile, "%c%c", &burstLetter, &testLetter);
            letterPosInFile++;
            whetherBurstLetter = true;
        }
        else
        {
            whetherWordRunOut = true;
        }
        fclose(wordFile);
    }

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    if (difficulty < 10)
    {
        if (difficultyCounter >= 5)
        {
            difficulty++;
            difficultyCounter = 0;
            SKIP_TICKS -= 10;
        }
    }
}

void drawLocally()
{
    setPos(letterX, letterY);
    if (!whetherBurstLetter || whetherWordRunOut)
    {
        SetConsoleTextAttribute(h, COLOR_LETTER);
        if (ft == HALF_WIDTH)
            cout << "G";
        else
            cout << "???";
    }
    else
    {
        SetConsoleTextAttribute(h, COLOR_BURST_LETTER);

        if (ft == HALF_WIDTH)
            cout << burstLetter;
        else
            convertHalfFull(burstLetter);

        SetConsoleTextAttribute(h, COLOR_TEXT);
    }

    for (int i = 0; i < nTail; i++)
    {
        setPos(tailX[i], tailY[i]);
        if (i == 0)
        {
            SetConsoleTextAttribute(h, COLOR_SNAKE_HEAD);
            if (ft == HALF_WIDTH)
                cout << "O";
            else
                cout << "???";
        }
        else
        {
            if (burstCounter > 0)
                SetConsoleTextAttribute(h, COLOR_SNAKE_BURST_BODY);
            else
                SetConsoleTextAttribute(h, COLOR_SNAKE_BODY);

            if (ft == HALF_WIDTH)
                cout << "o";
            else
                cout << "???";
        }
    }

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "                        ";

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "??? ?????????????????????" << difficulty << " ???";

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y + 1);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "                        ";

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y + 1);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "??? ???????????????" << score << " ???";

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y + 2);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "                          ";

    setPos(STAGE_WIDTH + 7, WORD_ADJUSTMENT_Y + 2);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "??? ?????????????????????" << wordNum << " ???";

    setPos(collectLetterPosX, collectLetterPosY);
    SetConsoleTextAttribute(h, COLOR_TEXT);
    cout << "                           ";

    setPos(collectLetterPosX, collectLetterPosY);
    SetConsoleTextAttribute(h, COLOR_BURST_LETTER);
    for (int i = 0; i < letterNum; i++)
    {
        convertHalfFull(collectLetter[i]);
    }
    SetConsoleTextAttribute(h, COLOR_TEXT);
}

void gameOverInfo()
{
    SetConsoleTextAttribute(h, COLOR_TEXT2);

    if (ft == HALF_WIDTH)
    {
        setPos(CENTER_TEXT_GAMEOVER_X, CENTER_TEXT_GAMEOVER_Y);
        cout << "???????????????";
        setPos(CENTER_TEXT_RESTART_X, CENTER_TEXT_RESTART_Y);
        cout << "??? ???????????? / ??? ??????";
        setPos(CENTER_TEXT_LOAD_X, CENTER_TEXT_LOAD_Y);
        cout << "??? ????????????????????????????????????";
        if (!ranked)
        {
            setPos(CENTER_TEXT_RANK_NAME_X, CENTER_TEXT_RANK_NAME_Y);
            cout << "??? ????????????????????????";
        }
    }
    else
    {
        setPos(CENTER_TEXT_GAMEOVER_X + 1, CENTER_TEXT_GAMEOVER_Y);
        cout << "???????????????";
        setPos(CENTER_TEXT_RESTART_X + 3, CENTER_TEXT_RESTART_Y);
        cout << "??? ???????????? / ??? ??????";
        setPos(CENTER_TEXT_LOAD_X + 5, CENTER_TEXT_LOAD_Y);
        cout << "??? ????????????????????????????????????";
        if (!ranked)
        {
            setPos(CENTER_TEXT_RANK_NAME_X + 3, CENTER_TEXT_RANK_NAME_Y);
            cout << "??? ????????????????????????";
        }
    }

    SetConsoleTextAttribute(h, COLOR_TEXT);
}

void gamePauseInfo()
{
    SetConsoleTextAttribute(h, COLOR_TEXT2);
    setPos(CENTER_TEXT_PAUSED_X, CENTER_TEXT_PAUSED_Y);
    cout << "?????????";

    SetConsoleTextAttribute(h, COLOR_TEXT);
}

void save()
{
    int initX, initY = 15;
    if (ft == HALF_WIDTH)
        initX = STAGE_WIDTH + 50;
    else
        initX = STAGE_WIDTH + 25;
    
    SetConsoleTextAttribute(h, COLOR_TEXT2);

    setPos(initX, initY);
    cout << "??????1?????????1";

    initY++;
    setPos(initX, initY);
    cout << "??????2?????????2";

    initY++;
    setPos(initX, initY);
    cout << "??????3?????????3";

    initY++;
    setPos(initX, initY);
    cout << "??????4?????????4";

    initY++;
    setPos(initX, initY);
    cout << "??????5?????????5";

    initY++;
    setPos(initX, initY);
    cout << "??????????????????????????????";

    SetConsoleTextAttribute(h, COLOR_TEXT);

    FILE *fp;

    while (true)
    {
        if (kbhit())
        {
            switch (getch())
            {
                case '1':
                    fp = fopen("save1.txt", "w");
                    break;
                case '2':
                    fp = fopen("save2.txt", "w");
                    break;
                case '3':
                    fp = fopen("save3.txt", "w");
                    break;
                case '4':
                    fp = fopen("save4.txt", "w");
                    break;
                case '5':
                    fp = fopen("save5.txt", "w");
                    break;
                case 'r':
                case 'R':
                    if (ft == HALF_WIDTH)
                        ft = FULL_WIDTH;
                    else
                        ft = HALF_WIDTH;

                    drawMap();
                    drawLocally();
                    promptInfo(3, 1);
                    initY = 15;
                    if (ft == HALF_WIDTH)
                        initX = STAGE_WIDTH + 50;
                    else
                        initX = STAGE_WIDTH + 25;
                    
                    SetConsoleTextAttribute(h, COLOR_TEXT2);

                    setPos(initX, initY);
                    cout << "??????1?????????1";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????2?????????2";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????3?????????3";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????4?????????4";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????5?????????5";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????????????????????????????";

                    gamePauseInfo();
                    SetConsoleTextAttribute(h, COLOR_TEXT);
                    continue;
                case 27:
                    gameOver = true;
                    gameQuit = true;
                    break;
                case 'I':
                case 'i':
                    if (rankOn == false)
                    {
                        showRankBoard();
                        rankOn = true;
                    }
                    else
                    {
                        removeRankBoard();
                        rankOn = false;
                    }
                    continue;
                default:
                    initY = 15;
                    if (ft == HALF_WIDTH)
                        initX = STAGE_WIDTH + 50;
                    else
                        initX = STAGE_WIDTH + 25;
                    SetConsoleTextAttribute(h, COLOR_TEXT);

                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "                    ";
                    return;
            }
            break;
        }
    }

    fprintf(fp, "%d\n", nTail);
    fprintf(fp, "%d %d\n", x, y);
    for (int i = 0; i < nTail; i++)
    {
        fprintf(fp, "%d %d\n", tailX[i], tailY[i]);
    }
    fprintf(fp, "%d %d\n", letterX, letterY);
    fprintf(fp, "%d\n", dir);
    fprintf(fp, "%d\n", score);

    fprintf(fp, "%d\n", difficulty);
    fprintf(fp, "%d\n", difficultyCounter);
    fprintf(fp, "%d\n", SKIP_TICKS);

    fprintf(fp, "%c %c\n", burstLetter, testLetter);
    for (int i = 0; i < MAX_LETTER_LENGTH; i++)
    {
        fprintf(fp, "%c", collectLetter[i]);
    }
    fprintf(fp, "\n");
    fprintf(fp, "%d %d\n", letterNum, wordNum);
    fprintf(fp, "%d\n", whetherBurstLetter);
    fprintf(fp, "%d\n", whetherBursted);
    fprintf(fp, "%d\n", whetherWordRunOut);
    fprintf(fp, "%d\n", burstCounter);
    fprintf(fp, "%d\n", letterPosInFile);

    fclose(fp);

    initY = 15;
    if (ft == HALF_WIDTH)
        initX = STAGE_WIDTH + 50;
    else
        initX = STAGE_WIDTH + 25;
    SetConsoleTextAttribute(h, COLOR_TEXT);

    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "                    ";
}

void load()
{
    bool error = false;

    int initX, initY = 15;
    if (ft == HALF_WIDTH)
        initX = STAGE_WIDTH + 50;
    else
        initX = STAGE_WIDTH + 25;

    SetConsoleTextAttribute(h, COLOR_TEXT2);

    setPos(initX, initY);
    cout << "??????1?????????1";

    initY++;
    setPos(initX, initY);
    cout << "??????2?????????2";

    initY++;
    setPos(initX, initY);
    cout << "??????3?????????3";

    initY++;
    setPos(initX, initY);
    cout << "??????4?????????4";

    initY++;
    setPos(initX, initY);
    cout << "??????5?????????5";

    initY++;
    setPos(initX, initY);
    cout << "???????????????Q";

    SetConsoleTextAttribute(h, COLOR_TEXT);

    FILE *fp;
    
    while (true)
    {
        if (kbhit())
        {
            switch (getch())
            {
                case '1':
                    fp = fopen("save1.txt", "r");
                    break;
                case '2':
                    fp = fopen("save2.txt", "r");
                    break;
                case '3':
                    fp = fopen("save3.txt", "r");
                    break;
                case '4':
                    fp = fopen("save4.txt", "r");
                    break;
                case '5':
                    fp = fopen("save5.txt", "r");
                    break;
                case 'r':
                case 'R':
                    if (ft == HALF_WIDTH)
                        ft = FULL_WIDTH;
                    else
                        ft = HALF_WIDTH;

                    drawMap();
                    drawLocally();
                    promptInfo(3, 1);
                    initY = 15;
                    if (ft == HALF_WIDTH)
                        initX = STAGE_WIDTH + 50;
                    else
                        initX = STAGE_WIDTH + 25;

                    SetConsoleTextAttribute(h, COLOR_TEXT2);

                    setPos(initX, initY);
                    cout << "??????1?????????1";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????2?????????2";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????3?????????3";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????4?????????4";

                    initY++;
                    setPos(initX, initY);
                    cout << "??????5?????????5";

                    initY++;
                    setPos(initX, initY);
                    cout << "???????????????Q";

                    if (error == true)
                    {
                        SetConsoleTextAttribute(h, COLOR_TEXT2);
                        if (ft == HALF_WIDTH)
                            setPos(STAGE_WIDTH + 50, 21);
                        else
                            setPos(STAGE_WIDTH + 25, 21);
                        cout << "????????????????????????";
                        error = true;
                        SetConsoleTextAttribute(h, COLOR_TEXT);
                    }

                    gamePauseInfo();
                    SetConsoleTextAttribute(h, COLOR_TEXT);
                    continue;
                case 27:
                    gameOver = true;
                    gameQuit = true;
                    break;
                case 'q':
                case 'Q':
                    initY = 15;
                    if (ft == HALF_WIDTH)
                        initX = STAGE_WIDTH + 50;
                    else
                        initX = STAGE_WIDTH + 25;

                    SetConsoleTextAttribute(h, COLOR_TEXT);

                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "            ";

                    initY++;
                    setPos(initX, initY);
                    cout << "                    ";

                    initY++;
                    setPos(initX, initY);
                    cout << "                    ";
                    return;
                case 'I':
                case 'i':
                    if (rankOn == false)
                    {
                        showRankBoard();
                        rankOn = true;
                    }
                    else
                    {
                        removeRankBoard();
                        rankOn = false;
                    }
                    continue;
                default:
                    SetConsoleTextAttribute(h, COLOR_TEXT2);
                    if (ft == HALF_WIDTH)
                        setPos(STAGE_WIDTH + 50, 21);
                    else
                        setPos(STAGE_WIDTH + 25, 21);
                    cout << "????????????????????????";
                    error = true;
                    SetConsoleTextAttribute(h, COLOR_TEXT);
                    continue;
            }
            break;
        }
    }

    error = false;

    if (gameOver)
    {
        restart();
        gameOver = false;
        gameQuit = false;
        gamePaused = true;
    }

    if (fp != NULL)
    {
        erase();

        fscanf(fp, "%d\n", &nTail);
        fscanf(fp, "%d %d\n", &x, &y);
        for (int i = 0; i < nTail; i++)
        {
            fscanf(fp, "%d %d\n", &tailX[i], &tailY[i]);
        }
        fscanf(fp, "%d %d\n", &letterX, &letterY);
        fscanf(fp, "%d\n", &dir);
        fscanf(fp, "%d", &score);
        fscanf(fp, "%d\n", &difficulty);
        fscanf(fp, "%d\n", &difficultyCounter);
        fscanf(fp, "%d\n", &SKIP_TICKS);

        fscanf(fp, "%c %c\n", &burstLetter, &testLetter);
        for (int i = 0; i < MAX_LETTER_LENGTH; i++)
        {
            fscanf(fp, "%c", &collectLetter[i]);
        }
        fscanf(fp, "%d %d\n", &letterNum, &wordNum);
        fscanf(fp, "%d\n", &whetherBurstLetter);
        fscanf(fp, "%d\n", &whetherBursted);
        fscanf(fp, "%d\n", &whetherWordRunOut);
        fscanf(fp, "%d\n", &burstCounter);
        fscanf(fp, "%d\n", &letterPosInFile);
        
        fclose(fp);
    }
    else
    {
        SetConsoleTextAttribute(h, COLOR_TEXT3);
        fclose(fp);
        setPos(initX, initY + 1);
        cout << "????????????????????????";
        SetConsoleTextAttribute(h, COLOR_TEXT);
        Sleep(1000);

        if (gameOver)
        {
            restart();
            gameOver = false;
            gameQuit = false;
        }
    }

    initY = 15;
    if (ft == HALF_WIDTH)
        initX = STAGE_WIDTH + 50;
    else
        initX = STAGE_WIDTH + 25;

    SetConsoleTextAttribute(h, COLOR_TEXT);

    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "            ";

    initY++;
    setPos(initX, initY);
    cout << "                    ";

    initY++;
    setPos(initX, initY);
    cout << "                    ";

    initY++;
    setPos(initX, initY);
    cout << "               ";
}

void rankBoard()
{
    if (gameOver)
    {
        setPos(3, RANKING_QUEST_TEXT_Y);
        SetConsoleTextAttribute(h, COLOR_TEXT);

        cout << "?????????????????????????????????????????????20???????????????";

        setPos(3, RANKING_NAME_INPUT_Y);

        cin >> name;
    }

    FILE *fp;
    fp = fopen("rank board.txt", "a");
    fprintf(fp, "%s %d", name, score);
    fprintf(fp, "\n");
    fclose(fp);

    nws all[MAX_RANKING_BOARD_PLAYER_NUMBER + 2];
    int nameCount = 0;

    fp = fopen("rank board.txt", "r");
    while (true)
    {
        if(fscanf(fp, "%s %d\n", all[nameCount].name, &all[nameCount].score) != EOF)
        {
            nameCount++;
        }
        else
        {
            break;
        }
    }
    fclose(fp);

    insertionSort(all, nameCount);

    fp = fopen("rank board.txt", "w");
    int k = 0;
    if (nameCount > 5)
    {
        k = 1;
    }

    for (int i = nameCount - 1; i >= k; i--)
    {
        fprintf(fp, "%s %d", all[i].name, all[i].score);
        fprintf(fp, "\n");
    }
    fclose(fp);

    setPos(3, RANKING_QUEST_TEXT_Y);
    cout << "                                            ";
    setPos(3, RANKING_NAME_INPUT_Y);
    cout << "                                            ";

    ranked = true;
    if (ft == HALF_WIDTH)
    {
        setPos(CENTER_TEXT_RANK_NAME_X, CENTER_TEXT_RANK_NAME_Y);
        cout << "                   ";
    }
    else
    {
        setPos(CENTER_TEXT_RANK_NAME_X + 3, CENTER_TEXT_RANK_NAME_Y);
        cout << "                   ";
    }

    rankNum = 0;
    readRankBoard();
    drawLocally();

    if (rankOn)
    {
        removeRankBoard();
        showRankBoard();
    }
}

void input()
{
    if (kbhit())
    {
        switch(getch())
        {
            case 'a':
            case 'A':
            case 75:
                if (dir == RIGHT)
                    break;
                dir = LEFT;
                break;
            case 'd':
            case 'D':
            case 77:
                if (dir == LEFT)
                    break;
                dir = RIGHT;
                break;
            case 'w':
            case 'W':
            case 72:
                if (dir == DOWN)
                    break;
                dir = UP;
                break;
            case 's':
            case 'S':
            case 80:
                if (dir == UP)
                    break;
                dir = DOWN;
                break;
            case 'r':
            case 'R':
                if (ft == HALF_WIDTH)
                    ft = FULL_WIDTH;
                else
                    ft = HALF_WIDTH;

                drawMap();
                drawLocally();
                promptInfo(3, 1);

                if (rankOn)
                {
                    showRankBoard();
                }
                break;
            case 'I':
            case 'i':
                if (rankOn == false)
                {
                    showRankBoard();
                    rankOn = true;
                }
                else
                {
                    removeRankBoard();
                    rankOn = false;
                }
                break;
            case 27:
                gameOver = true;
                gameQuit = true;
                break;
            case 32:
                gamePaused = true;
                break;
            default:
                break;
        }
    }
}

void input2()
{
    if (kbhit())
    {
        switch (getch())
        {
            case 27:
                gameOver = true;
                gameQuit = true;
                break;
            case 's':
            case 'S':
                if (!gameOver)
                {
                    save();
                }
                break;
            case 'l':
            case 'L':
                SetConsoleTextAttribute(h, COLOR_TEXT);
                load();
                drawLocally();
                break;
            case 'y':
            case 'Y':
                if (gameOver)
                {
                    restart();
                    gameOver = false;
                    gameQuit = false;
                }
                break;
            case 'n':
            case 'N':
                if (gameOver)
                {
                    gameQuit = true;
                }
                break;
            case 'r':
            case 'R':
                if (ft == HALF_WIDTH)
                    ft = FULL_WIDTH;
                else
                    ft = HALF_WIDTH;

                drawMap();
                drawLocally();
                promptInfo(3, 1);
                break;
            case 'I':
            case 'i':
                if (rankOn == false)
                {
                    showRankBoard();
                    rankOn = true;
                }
                else
                {
                    removeRankBoard();
                    rankOn = false;
                }
                break;
            case 't':
            case 'T':
                if (gameOver && !ranked)
                {
                    rankBoard();
                }
                break;
            default:
                if (!gameOver)
                {
                    gamePaused = false;
                    gameQuit = false;

                    SetConsoleTextAttribute(h, COLOR_TEXT);
                    setPos(CENTER_TEXT_PAUSED_X ,CENTER_TEXT_PAUSED_Y);
                    cout << "      ";
                }
                else
                    gameQuit = true;
                break;
        }
    }
}

int main()
{
    init();
    drawMap();
    promptInfo(3, 1);

    while (!gameQuit)
    {
        while (!gameOver && !gamePaused)
        {
            input();
            eraseSnake();
            logic();
            drawLocally();

            nextGameTick += SKIP_TICKS;
            sleepTime = nextGameTick - GetTickCount();
            if (sleepTime >= 0)
                Sleep(sleepTime);
        }

        if (gameOver)
        {
            gameOverInfo();
        }

        if (gamePaused)
        {
            gamePauseInfo();
        }

        nextGameTick += SKIP_TICKS;
        sleepTime = nextGameTick - GetTickCount();
        if (sleepTime >= 0)
            Sleep(sleepTime);

        input2();
    }

    return 0;
}