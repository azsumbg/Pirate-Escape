#include "framework.h"
#include "Pirate Escape.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "seaslope.h"
#include "seafactory.h"
#include <vector>
#include <fstream>
#include <chrono>

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "errh.lib")
#pragma comment (lib, "fcheck.lib")
#pragma comment (lib, "d2bmploader.lib")
#pragma comment (lib, "seaslope.lib")
#pragma comment (lib, "seafactory.lib")

#define bWinClassName L"MyPirateGame"

#define mNew 1001
#define mSpeed 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define record 2001
#define no_record 2002
#define first_record 2003

#define snd_file L".\\res\\snd\\main.wav"
#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define hlp_file L".\\res\\data\\help.dat"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"

WNDCLASS bWinClass = { 0 };
HINSTANCE bIns = nullptr;
HWND bHwnd = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
HICON mainIcon = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
POINT cur_pos = { 0 };
MSG bMsg = { 0 };
BOOL bRet = 0;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint = { 0 };
UINT bTimer = 0;

bool pause = false;
bool show_help = false;
bool in_client = true;
bool sound = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool set_name = false;
bool hero_killed = false;
bool win_game = false;

D2D1_RECT_F b1Rect = { 0, 0, 275.0f, 50.0f };
D2D1_RECT_F b2Rect = { 375.0f, 0, 625.0f, 50.0f };
D2D1_RECT_F b3Rect = { 725, 0, scr_width, 50.0f };

D2D1_RECT_F b1TxtRect = { 25.0f, 0, 275.0f, 50.0f };
D2D1_RECT_F b2TxtRect = { 400.0f, 0, 625.0f, 50.0f };
D2D1_RECT_F b3TxtRect = { 750, 0, scr_width, 50.0f };

wchar_t current_player[16] = L"A CAPTAIN";

int game_speed = 1;
int seconds = 0;
int minutes = 0;
int score = 0;

int island1_hts = 5;
int island2_hts = 5;
int island3_hts = 5;

/////////////////////////////////////////////////

struct SHOTDATA
{
    dll::BOULDER Shot;
    move::DATA Path;
    dirs dir = dirs::stop;
};
struct EXPLOSION
{
    dll::ATOM Dims;
    int frame;
};

move::DATA MyShotData;
dirs MyShotDir = dirs::stop;

move::DATA EnemyShotData;
dirs EnemyShotDir = dirs::stop;

dll::obj_ptr Hero = nullptr;
std::vector<dll::obj_ptr> vPirates;

std::vector<SHOTDATA>vMyBoulders;
std::vector<SHOTDATA>vEvilBoulders;

dll::SCREENDATA ActiveScreen;
dll::SCREENDATA AllGameScreens[8];

int current_field_frame = 0;

std::vector<EXPLOSION> vExplosions;

///////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

ID2D1RadialGradientBrush* butBckg = nullptr;
ID2D1SolidColorBrush* TxtBrush = nullptr;
ID2D1SolidColorBrush* InactBrush = nullptr;
ID2D1SolidColorBrush* HgltBrush = nullptr;

ID2D1SolidColorBrush* GreenBrush = nullptr;
ID2D1SolidColorBrush* YellowBrush = nullptr;
ID2D1SolidColorBrush* RedBrush = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmText = nullptr;
IDWriteTextFormat* bigText = nullptr;
IDWriteTextFormat* middleText = nullptr;

ID2D1Bitmap* bmpBall = nullptr;
ID2D1Bitmap* bmpFinal = nullptr;
ID2D1Bitmap* bmpIsland1 = nullptr;
ID2D1Bitmap* bmpIsland2 = nullptr;
ID2D1Bitmap* bmpIsland3 = nullptr;

ID2D1Bitmap* bmpField[16] = { nullptr };
ID2D1Bitmap* bmpExplosion[24] = { nullptr };

ID2D1Bitmap* bmpHeroL[7] = { nullptr };
ID2D1Bitmap* bmpHeroR[7] = { nullptr };

ID2D1Bitmap* bmpBad1L[3] = { nullptr };
ID2D1Bitmap* bmpBad1R[3] = { nullptr };

ID2D1Bitmap* bmpBad2L[5] = { nullptr };
ID2D1Bitmap* bmpBad2R[5] = { nullptr };

ID2D1Bitmap* bmpBad3L[10] = { nullptr };
ID2D1Bitmap* bmpBad3R[10] = { nullptr };

///////////////////////////////////////////////

template <typename GARB> void Swipe(GARB** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
    }
}
void LogError(LPCWSTR what)
{
    std::wofstream log(L".\\res\\data\\errorlog.dat", std::ios::app);
    log << what << L" ! Time stamp: " << std::chrono::system_clock::now() << std::endl;
    log.close();
}
void ClearRes()
{
    Swipe(&iFactory);
    Swipe(&Draw);
    Swipe(&butBckg);
    Swipe(&TxtBrush);
    Swipe(&InactBrush);
    Swipe(&HgltBrush);
    Swipe(&GreenBrush);
    Swipe(&YellowBrush);
    Swipe(&RedBrush);

    Swipe(&iWriteFactory);
    Swipe(&nrmText);
    Swipe(&middleText);
    Swipe(&bigText);

    Swipe(&bmpBall);
    Swipe(&bmpFinal);
    Swipe(&bmpIsland1);
    Swipe(&bmpIsland2);
    Swipe(&bmpIsland3);

    for (int i = 0; i < 16; i++)Swipe(&bmpField[i]);
    for (int i = 0; i < 24; i++)Swipe(&bmpExplosion[i]);

    for (int i = 0; i < 7; i++)Swipe(&bmpHeroL[i]);
    for (int i = 0; i < 7; i++)Swipe(&bmpHeroR[i]);

    for (int i = 0; i < 3; i++)Swipe(&bmpBad1L[i]);
    for (int i = 0; i < 3; i++)Swipe(&bmpBad1R[i]);

    for (int i = 0; i < 5; i++)Swipe(&bmpBad2L[i]);
    for (int i = 0; i < 5; i++)Swipe(&bmpBad2R[i]);

    for (int i = 0; i < 10; i++)Swipe(&bmpBad3L[i]);
    for (int i = 0; i < 10; i++)Swipe(&bmpBad3R[i]);
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    ClearRes();
    std::remove(tmp_file);
    exit(1);
}

void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    game_speed = 1;
    seconds = 0;
    minutes = 0;
    score = 0;

    wcscpy_s(current_player, L"A CAPTAIN");
    set_name = false;
    Swipe(&Hero);

    if(!vPirates.empty())
        for (int i = 0; i < vPirates.size(); i++)
        {
            Swipe(&vPirates[i]);
            vPirates.erase(vPirates.begin() + i);
        }
    vPirates.clear();

    vMyBoulders.clear();

    vEvilBoulders.clear();

    vExplosions.clear();
    ////////////////////////////////////////////////////

    dll::InitScreenData(AllGameScreens);
    ActiveScreen = AllGameScreens[0];

    Hero = dll::iFactory(types::hero, 50.0f, 75.0f, dirs::stop);


}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return (INT_PTR)(TRUE);
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 15) < 1)
            {
                wcscpy_s(current_player, L"A CAPTAIN");
                if (sound)MessageBeep(MB_ICONEXCLAMATION);
                MessageBox(bHwnd, L"Ха, ха, па ! Забрави си името !",
                    L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
                EndDialog(hwnd, IDCANCEL);
                break;
            }

            EndDialog(hwnd, IDOK);
            break;
        }
    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        srand((unsigned int)(time(0)));
        SetTimer(hwnd, bTimer, 1000, NULL);

        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();
        
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mSpeed, L"Турбо режим");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");
        
        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

        SetMenu(hwnd, bBar);
        InitGame();

        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !\n\nНаистина ли излизаш !", L"Изход !",
            MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(50, 50, 50)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        seconds++;
        minutes = (int)(floor(seconds / 60));
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }
            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                    }
                    if (b2Hglt || b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                    }
                    if (b1Hglt || b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                    }
                    if (b1Hglt || b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                    }
                }
            
                SetCursor(outCursor);
                return true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(mainCursor);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Ако рестартираш, ще загубиш тази игра !\n\nНаистина ли рестартираш !", L"Рестарт !",
                MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mSpeed:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Наистина ли искаш по-бързо ?", L"Турбо !",
                MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            game_speed++;
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;


        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            if (Hero)
            {
                Hero->dir = dirs::left;
                dll::ATOM Dummy(Hero->x - (1.0f + game_speed), Hero->y, Hero->GetWidth(), Hero->GetHeight());
                if (ActiveScreen.Island1.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island1.ex || Dummy.ex <= ActiveScreen.Island1.x
                        || Dummy.y >= ActiveScreen.Island1.ey || Dummy.ey <= ActiveScreen.Island1.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island2.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island2.ex || Dummy.ex <= ActiveScreen.Island2.x
                        || Dummy.y >= ActiveScreen.Island2.ey || Dummy.ey <= ActiveScreen.Island2.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island3.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island3.ex || Dummy.ex <= ActiveScreen.Island3.x
                        || Dummy.y >= ActiveScreen.Island3.ey || Dummy.ey <= ActiveScreen.Island3.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
            }
            break;

        case VK_RIGHT:
            if (Hero)
            {
                Hero->dir = dirs::right;
                dll::ATOM Dummy(Hero->x + (1.0f + game_speed), Hero->y, Hero->GetWidth(), Hero->GetHeight());
                if (ActiveScreen.Island1.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island1.ex || Dummy.ex <= ActiveScreen.Island1.x
                        || Dummy.y >= ActiveScreen.Island1.ey || Dummy.ey <= ActiveScreen.Island1.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island2.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island2.ex || Dummy.ex <= ActiveScreen.Island2.x
                        || Dummy.y >= ActiveScreen.Island2.ey || Dummy.ey <= ActiveScreen.Island2.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island3.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island3.ex || Dummy.ex <= ActiveScreen.Island3.x
                        || Dummy.y >= ActiveScreen.Island3.ey || Dummy.ey <= ActiveScreen.Island3.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
            }
            break;

        case VK_UP:
            if (Hero)
            {
                Hero->dir = dirs::up;
                dll::ATOM Dummy(Hero->x, Hero->y - (1.0f + game_speed), Hero->GetWidth(), Hero->GetHeight());
                if (ActiveScreen.Island1.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island1.ex || Dummy.ex <= ActiveScreen.Island1.x
                        || Dummy.y >= ActiveScreen.Island1.ey || Dummy.ey <= ActiveScreen.Island1.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island2.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island2.ex || Dummy.ex <= ActiveScreen.Island2.x
                        || Dummy.y >= ActiveScreen.Island2.ey || Dummy.ey <= ActiveScreen.Island2.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island3.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island3.ex || Dummy.ex <= ActiveScreen.Island3.x
                        || Dummy.y >= ActiveScreen.Island3.ey || Dummy.ey <= ActiveScreen.Island3.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
            }
            break;

        case VK_DOWN:
            if (Hero)
            {
                Hero->dir = dirs::down;
                dll::ATOM Dummy(Hero->x, Hero->y + (1.0f + game_speed), Hero->GetWidth(), Hero->GetHeight());
                if (ActiveScreen.Island1.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island1.ex || Dummy.ex <= ActiveScreen.Island1.x
                        || Dummy.y >= ActiveScreen.Island1.ey || Dummy.ey <= ActiveScreen.Island1.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island2.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island2.ex || Dummy.ex <= ActiveScreen.Island2.x
                        || Dummy.y >= ActiveScreen.Island2.ey || Dummy.ey <= ActiveScreen.Island2.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
                if (ActiveScreen.Island3.x >= 0)
                {
                    if (!(Dummy.x >= ActiveScreen.Island3.ex || Dummy.ex <= ActiveScreen.Island3.x
                        || Dummy.y >= ActiveScreen.Island3.ey || Dummy.ey <= ActiveScreen.Island3.y))
                    {
                        Hero->dir = dirs::stop;
                        break;
                    }
                }
            }
            break;

        case VK_SPACE:
            if (Hero)Hero->dir = dirs::stop;
        }
        break;

    case WM_LBUTTONDOWN:
         if (Hero)
            {
                if (!Hero->now_shooting)
                {
                    Hero->Shoot();
                    MyShotData = move::Init(Hero->x + 70.0f, Hero->y + 70.0f, LOWORD(lParam), HIWORD(lParam));
                    if (Hero->x <= LOWORD(lParam))MyShotDir = dirs::right;
                    else MyShotDir = dirs::left;
                }
            }
         break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2) - (int)(scr_width / 2);
    int win_y = 20;
    int result = 0;

    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream tmp(Ltmp_file);
        tmp << L"IgrUtu ruboty";
        tmp.close();
    }

    if (GetSystemMetrics(SM_CXSCREEN) < win_x + scr_width || GetSystemMetrics(SM_CYSCREEN) < win_y + scr_height)ErrExit(eScreen);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\bmain.ico", IMAGE_ICON, 227, 255, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFile(L".\\res\\main.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCursor || !outCursor)ErrExit(eCursor);

    bWinClass.lpszClassName = bWinClassName;
    bWinClass.hInstance = bIns;
    bWinClass.lpfnWndProc = &WinProc;
    bWinClass.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));
    bWinClass.hIcon = mainIcon;
    bWinClass.hCursor = mainCursor;
    bWinClass.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWinClass))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ПИРАТСКА АТАКА !", WS_CAPTION | WS_SYSMENU, win_x, win_y, (int)(scr_width), 
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else ShowWindow(bHwnd, SW_SHOWDEFAULT);
    ////////////////////////////////////////////////////////

    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 Factory");
        ErrExit(eD2D);
    }

    if (bHwnd)
        hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 HwndRenderTarget");
        ErrExit(eD2D);
    }

    D2D1_GRADIENT_STOP gStop[2] = { 0 };
    ID2D1GradientStopCollection* gStopCol = nullptr;

    gStop[0].position = 0;
    gStop[0].color = D2D1::ColorF(D2D1::ColorF::LightSeaGreen);
    gStop[1].position = 1.0f;
    gStop[1].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);

    if (Draw)
        hr = Draw->CreateGradientStopCollection(gStop, 2, &gStopCol);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 GradientStopCollection");
        ErrExit(eD2D);
    }

    if (Draw && gStopCol)
        hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
            D2D1::Point2F(0, 0), scr_width / 2, 25.0f), gStopCol, &butBckg);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 RadialGradientBrush");
        ErrExit(eD2D);
    }
    Swipe(&gStopCol);

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver), &TxtBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 TxtBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &HgltBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 HgltBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &InactBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 TxtBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &GreenBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 GreenBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &YellowBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 YellowBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &RedBrush);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 RedBrush");
        ErrExit(eD2D);
    }

    ///////////////////////////////////////////////////

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 WriteFactory");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_EXTRA_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"", &nrmText);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 normal WriteTextFormat");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_EXTRA_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"", &middleText);
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 middle WriteTextFormat");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_EXTRA_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigText);
    
    if (hr != S_OK)
    {
        LogError(L"Error creating D2D1 big WriteTextFormat");
        ErrExit(eD2D);
    }

    //////////////////////////////////////////////////////

    bmpBall = Load(L".\\res\\img\\ball.png", Draw);
    if (!bmpBall)
    {
        LogError(L"Error loading bmpBall");
        ErrExit(eD2D);
    }

    bmpFinal = Load(L".\\res\\img\\final.png", Draw);
    if (!bmpBall)
    {
        LogError(L"Error loading bmpFinal");
        ErrExit(eD2D);
    }

    bmpIsland1 = Load(L".\\res\\img\\island1.png", Draw);
    if (!bmpIsland1)
    {
        LogError(L"Error loading bmpIsland1");
        ErrExit(eD2D);
    }

    bmpIsland2 = Load(L".\\res\\img\\island2.png", Draw);
    if (!bmpIsland2)
    {
        LogError(L"Error loading bmpIsland2");
        ErrExit(eD2D);
    }

    bmpIsland3 = Load(L".\\res\\img\\island3.png", Draw);
    if (!bmpIsland3)
    {
        LogError(L"Error loading bmpIsland3");
        ErrExit(eD2D);
    }

    for (int i = 0; i < 16; i++)
    {
        wchar_t name[100] = L".\\res\\img\\field\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpField[i] = Load(name, Draw);
        if (!bmpField[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }
    for (int i = 0; i < 24; i++)
    {
        wchar_t name[100] = L".\\res\\img\\explosion\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpExplosion[i] = Load(name, Draw);
        if (!bmpExplosion[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 7; i++)
    {
        wchar_t name[100] = L".\\res\\img\\hero\\l\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".gif");

        bmpHeroL[i] = Load(name, Draw);
        if (!bmpHeroL[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }
    for (int i = 0; i < 7; i++)
    {
        wchar_t name[100] = L".\\res\\img\\hero\\r\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".gif");

        bmpHeroR[i] = Load(name, Draw);
        if (!bmpHeroR[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad1\\l\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad1L[i] = Load(name, Draw);
        if (!bmpBad1L[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad1\\r\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad1R[i] = Load(name, Draw);
        if (!bmpBad1R[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad2\\l\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad2L[i] = Load(name, Draw);
        if (!bmpBad2L[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }
    for (int i = 0; i < 5; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad2\\r\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad2R[i] = Load(name, Draw);
        if (!bmpBad2R[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad3\\l\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad3L[i] = Load(name, Draw);
        if (!bmpBad3L[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }
    for (int i = 0; i < 10; i++)
    {
        wchar_t name[100] = L".\\res\\img\\bad3\\r\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpBad3R[i] = Load(name, Draw);
        if (!bmpBad3R[i])
        {
            wchar_t error_txt[150] = L"Error loading ";
            wcscat_s(error_txt, name);
            LogError(error_txt);
            ErrExit(eD2D);
        }
    }

    ///////////////////////////////////////////////
    mciSendString(L"play .\\res\\snd\\intro.wav", NULL, NULL, NULL);

    D2D1_RECT_F up_scr = { 20.0f,-20.0f,scr_width,130.0f };
    D2D1_RECT_F down_scr = { scr_width - 150.0f, scr_height, scr_width + 300.0f,scr_height + 150.0f };

    bool all_set = false;

    while (!all_set)
    {
        if (up_scr.right < 200.0f)
        {
            up_scr.left += 1.5f;
            up_scr.right += 1.5f;
        }
        if (up_scr.bottom < scr_height / 2)
        {
            up_scr.top += 1.5f;
            up_scr.bottom += 1.5f;
        }
        if (down_scr.left > 550.0f)
        {
            down_scr.left -= 1.5f;
            down_scr.right -= 1.5f;
        }
        if (down_scr.top > scr_height / 2 + 100.0f)
        {
            down_scr.top -= 1.5f;
            down_scr.bottom -= 1.5f;
        }
        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkOliveGreen));
        if (bigText && HgltBrush)
        {
            Draw->DrawTextW(L"PIRATE ESCAPE", 14, bigText, up_scr, HgltBrush);
            Draw->DrawTextW(L"dev. Daniel", 12, bigText, down_scr, HgltBrush);
        }
        Draw->EndDraw();
        if (up_scr.right >= 200.f && up_scr.bottom >= scr_height / 2 && down_scr.left <= 550.f
            && down_scr.top <= scr_height / 2 + 100.0f)all_set = true;
    }
    Sleep(1000);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);
    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessageW(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkKhaki));
            Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, 
                scr_height), HgltBrush);
            Draw->EndDraw();
            continue;
        }

        /////////////////////////////////////////////////

        //GAME ENGINE *************************************************

        //MY SHIP **********************

        if (Hero)
        {
            if (Hero->dir != dirs::stop)
                if (Hero->Move((float)(game_speed)) == D_FAIL)
                {
                    int posibility = -1;
                    
                    switch(Hero->dir)
                    {
                    case dirs::right:
                        if (ActiveScreen.right > -1)
                        {
                            posibility = ActiveScreen.right;
                            ActiveScreen = AllGameScreens[posibility];
                            Hero->x = 10.0f;
                            Hero->SetEdges();
                            island1_hts = 5;
                            island2_hts = 5;
                            island3_hts = 5;
                            if (!vPirates.empty())
                            {
                                for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
                                    (*it)->Release();
                                vPirates.clear();
                            }
                        }
                        break;

                    case dirs::left:
                        if (ActiveScreen.left > -1)
                        {
                            posibility = ActiveScreen.left;
                            ActiveScreen = AllGameScreens[posibility];
                            Hero->x = scr_width - 10.0f;
                            Hero->SetEdges();
                            island1_hts = 5;
                            island2_hts = 5;
                            island3_hts = 5;
                            if (!vPirates.empty())
                            {
                                for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
                                    (*it)->Release();
                                vPirates.clear();
                            }
                        }
                        break;

                    case dirs::up:
                        if (ActiveScreen.up > -1)
                        {
                            posibility = ActiveScreen.up;
                            ActiveScreen = AllGameScreens[posibility];
                            Hero->y = scr_height - 10.0f;
                            Hero->SetEdges();
                            island1_hts = 5;
                            island2_hts = 5;
                            island3_hts = 5;
                            if (!vPirates.empty())
                            {
                                for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
                                    (*it)->Release();
                                vPirates.clear();
                            }
                        }
                        break;

                    case dirs::down:
                        if (ActiveScreen.down > -1)
                        {
                            posibility = ActiveScreen.down;
                            ActiveScreen = AllGameScreens[posibility];
                            Hero->y = 60.0f;
                            Hero->SetEdges();
                            island1_hts = 5;
                            island2_hts = 5;
                            island3_hts = 5;
                            if (!vPirates.empty())
                            {
                                for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
                                    (*it)->Release();
                                vPirates.clear();
                            }
                        }
                        break;
                    }

                    Hero->dir = dirs::stop;
                }
           if (Hero)
            {
                if (ActiveScreen.Island1.x >= 0)
                {
                    if (!(Hero->x >= ActiveScreen.Island1.ex || Hero->ex <= ActiveScreen.Island1.x
                        || Hero->y >= ActiveScreen.Island1.ey || Hero->ey <= ActiveScreen.Island1.y))
                        Hero->dir = dirs::stop;
                }
                if (ActiveScreen.Island2.x >= 0)
                {
                    if (!(Hero->x >= ActiveScreen.Island2.ex || Hero->ex <= ActiveScreen.Island2.x
                        || Hero->y >= ActiveScreen.Island2.ey || Hero->ey <= ActiveScreen.Island2.y))
                        Hero->dir = dirs::stop;
                     
                }
                if (ActiveScreen.Island3.x >= 0)
                {
                    if (!(Hero->x >= ActiveScreen.Island3.ex || Hero->ex <= ActiveScreen.Island3.x
                        || Hero->y >= ActiveScreen.Island3.ey || Hero->ey <= ActiveScreen.Island3.y))
                        Hero->dir = dirs::stop;
                }
                if (ActiveScreen.FinalIsland.x >= 0)
                {
                    if (!(Hero->x >= ActiveScreen.FinalIsland.ex || Hero->ex <= ActiveScreen.FinalIsland.x
                        || Hero->y >= ActiveScreen.FinalIsland.ey || Hero->ey <= ActiveScreen.FinalIsland.y))
                    {
                        win_game = true;
                        GameOver();
                    }
                }

            }
        
           if (Hero->now_shooting)
           {
               int shot_engaged = Hero->Shoot();
               if (shot_engaged > 0)
               {
                   dll::ATOM bDims(Hero->x + 70.0f, Hero->y + 70.0f, 30.0f, 30.0f);
                   dll::BOULDER bBoulder(bDims, shot_engaged);
                   vMyBoulders.push_back(SHOTDATA(bBoulder, MyShotData, MyShotDir));
               }
           }
        }

        ////////////////////////////////

        //MY SHOTS ***********************

        if (!vMyBoulders.empty())
        {
            for (std::vector<SHOTDATA>::iterator it = vMyBoulders.begin(); it < vMyBoulders.end(); it++)
            {
                switch (it->dir)
                {
                case dirs::left:
                {
                    float nexty = move::NextY(it->Shot.Dims.x -= game_speed * 1.5f, MyShotData);
                    it->Shot.Dims.x -= game_speed * 1.5f;
                    it->Shot.Dims.y = nexty;
                    it->Shot.Dims.SetEdges();
                    it->Shot.range--;
                }
                    break;

                case dirs::right:
                {
                    float nexty = move::NextY(it->Shot.Dims.x += game_speed * 1.5f, MyShotData);
                    it->Shot.Dims.x += game_speed * 1.5f;
                    it->Shot.Dims.y = nexty;
                    it->Shot.Dims.SetEdges();
                    it->Shot.range--;
                }
                break;

                }

                if (it->Shot.range < 0)
                {
                    vMyBoulders.erase(it);
                    break;
                }

            }
        }

        if (!vMyBoulders.empty())
        {
            for (std::vector<SHOTDATA>::iterator it = vMyBoulders.begin(); it < vMyBoulders.end(); it++)
            {
                if (ActiveScreen.Island1.x > -1)
                {
                    if (!(ActiveScreen.Island1.x > it->Shot.Dims.ex || ActiveScreen.Island1.ex<it->Shot.Dims.x ||
                        ActiveScreen.Island1.y>it->Shot.Dims.ey || ActiveScreen.Island1.ey < it->Shot.Dims.y))
                    {
                        vMyBoulders.erase(it);
                        island1_hts--;
                        if (island1_hts <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            dll::ATOM anExplosion(ActiveScreen.Island1.x, ActiveScreen.Island1.y, 200.0f, 227.0f);
                            vExplosions.push_back(EXPLOSION(anExplosion, -1));
                            ActiveScreen.Island1.x = -1;
                            ActiveScreen.Island1.y = -1;
                            ActiveScreen.Island1.ex = -1;
                            ActiveScreen.Island1.ey = -1;

                        }
                        break;
                    }
                }
                if (ActiveScreen.Island2.x > -1)
                {
                    if (!(ActiveScreen.Island2.x > it->Shot.Dims.ex || ActiveScreen.Island2.ex<it->Shot.Dims.x ||
                        ActiveScreen.Island2.y>it->Shot.Dims.ey || ActiveScreen.Island2.ey < it->Shot.Dims.y))
                    {
                        vMyBoulders.erase(it);
                        island2_hts--;
                        if (island2_hts <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            dll::ATOM anExplosion(ActiveScreen.Island2.x, ActiveScreen.Island2.y, 200.0f, 227.0f);
                            vExplosions.push_back(EXPLOSION(anExplosion, -1));
                            ActiveScreen.Island2.x = -1;
                            ActiveScreen.Island2.y = -1;
                            ActiveScreen.Island2.ex = -1;
                            ActiveScreen.Island2.ey = -1;
                        }
                        break;
                    }
                }
                if (ActiveScreen.Island3.x > -1)
                {
                    if (!(ActiveScreen.Island3.x > it->Shot.Dims.ex || ActiveScreen.Island3.ex<it->Shot.Dims.x ||
                        ActiveScreen.Island3.y>it->Shot.Dims.ey || ActiveScreen.Island3.ey < it->Shot.Dims.y))
                    {
                        vMyBoulders.erase(it);
                        island3_hts--;
                        if (island3_hts <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            dll::ATOM anExplosion(ActiveScreen.Island3.x, ActiveScreen.Island3.y, 200.0f, 227.0f);
                            vExplosions.push_back(EXPLOSION(anExplosion, -1));
                            ActiveScreen.Island3.x = -1;
                            ActiveScreen.Island3.y = -1;
                            ActiveScreen.Island3.ex = -1;
                            ActiveScreen.Island3.ey = -1;
                        }
                        break;
                    }
                }
            }
        }

        //////////////////////////////////

        //ENEMIES ***********************

        if (vPirates.size() < 2 && rand() % 300 == 66)
        {
            int first_dir = rand() % 4;
            int pirate_type = rand() % 3;

            switch (first_dir)
            {
            case 0:
                if (pirate_type == 0)vPirates.push_back(dll::iFactory(types::bad1, scr_width,
                    static_cast<float>(rand() % 600 + 60), dirs::left));
                else if (pirate_type == 1)vPirates.push_back(dll::iFactory(types::bad2, scr_width,
                    static_cast<float>(rand() % 600 + 60), dirs::left));
                else vPirates.push_back(dll::iFactory(types::bad3, scr_width,
                    static_cast<float>(rand() % 600 + 60), dirs::left));
                break;

            case 1:
                if (pirate_type == 0)vPirates.push_back(dll::iFactory(types::bad1, -100.0f,
                    static_cast<float>(rand() % 600 + 60), dirs::right));
                else if (pirate_type == 1)vPirates.push_back(dll::iFactory(types::bad2, -100.0f,
                    static_cast<float>(rand() % 600 + 60), dirs::right));
                else vPirates.push_back(dll::iFactory(types::bad3, -100.0f,
                    static_cast<float>(rand() % 600 + 60), dirs::right));
                break;

            case 2:
                if (pirate_type == 0)vPirates.push_back(dll::iFactory(types::bad1, static_cast<float>(rand() % 800),
                    scr_height, dirs::up));
                else if (pirate_type == 1)vPirates.push_back(dll::iFactory(types::bad2, static_cast<float>(rand() % 800),
                    scr_height, dirs::up));
                else vPirates.push_back(dll::iFactory(types::bad3, static_cast<float>(rand() % 800),
                    scr_height, dirs::up));
                break;

            case 3:
                if (pirate_type == 0)vPirates.push_back(dll::iFactory(types::bad1, static_cast<float>(rand() % 800),
                    -100.0f, dirs::down));
                else if (pirate_type == 1)vPirates.push_back(dll::iFactory(types::bad2, static_cast<float>(rand() % 800),
                    -100.0f, dirs::down));
                else vPirates.push_back(dll::iFactory(types::bad3, static_cast<float>(rand() % 800),
                    -100.0f, dirs::down));
                break;
            }
        }

        if (!vPirates.empty())
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
                (*it)->Move((float)(game_speed));
            
        }

        //ENEMY SHOTS ********************

        if (!vPirates.empty() && Hero)
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                if (abs((*it)->x - Hero->x) <= 150 || abs((*it)->y - Hero->y) <= 150)
                {
                    if (!((*it)->now_shooting))
                    {
                        (*it)->Shoot();
                        EnemyShotData = move::Init((*it)->x + 70.0f, (*it)->y + 70.0f, Hero->x, Hero->y);
                        if (Hero->x >= (*it)->x)EnemyShotDir = dirs::right;
                        else EnemyShotDir = dirs::left;
                    }
                    else
                    {
                        int shot_engaged = (*it)->Shoot();
                        if (shot_engaged > 0)
                        {
                            dll::ATOM bDims((*it)->x + 70.0f, (*it)->y + 70.0f, 30.0f, 30.0f);
                            dll::BOULDER bBoulder(bDims, shot_engaged);
                            vEvilBoulders.push_back(SHOTDATA(bBoulder, EnemyShotData, EnemyShotDir));
                        }
                    }
                }
            }
        }

        if (!vEvilBoulders.empty())
        {
            for (std::vector<SHOTDATA>::iterator it = vEvilBoulders.begin(); it < vEvilBoulders.end(); it++)
            {
                switch (it->dir)
                {
                case dirs::left:
                {
                    float nexty = move::NextY(it->Shot.Dims.x -= game_speed * 1.5f, EnemyShotData);
                    it->Shot.Dims.x -= game_speed * 1.5f;
                    it->Shot.Dims.y = nexty;
                    it->Shot.Dims.SetEdges();
                    it->Shot.range--;
                }
                break;

                case dirs::right:
                {
                    float nexty = move::NextY(it->Shot.Dims.x += game_speed * 1.5f, EnemyShotData);
                    it->Shot.Dims.x += game_speed * 1.5f;
                    it->Shot.Dims.y = nexty;
                    it->Shot.Dims.SetEdges();
                    it->Shot.range--;
                }
                break;
                }

                if (it->Shot.range < 0)
                {
                    vEvilBoulders.erase(it);
                    break;
                }

            }
        }

        if (!vEvilBoulders.empty() && Hero)
        {
            for (std::vector<SHOTDATA>::iterator it = vEvilBoulders.begin(); it < vEvilBoulders.end(); it++)
            {
                if (!(Hero->x > it->Shot.Dims.ex || Hero->ex<it->Shot.Dims.x
                    || Hero->y>it->Shot.Dims.ey || Hero->ey < it->Shot.Dims.y))
                {
                    Hero->lifes -= 30;
                    if (Hero->lifes <= 0)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                        vExplosions.push_back(EXPLOSION(dll::ATOM(Hero->x, Hero->y, Hero->GetWidth(), Hero->GetHeight()), -1));
                        hero_killed = true;
                        Swipe(&Hero);
                    }
                    vEvilBoulders.erase(it);
                    break;
                }
            }

        }

        //DRAW THINGS *************************************
        Draw->BeginDraw();
        Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), butBckg);
        if (nrmText && InactBrush && TxtBrush && HgltBrush)
        {
            if (set_name)
                Draw->DrawTextW(L"ВЪВЕДИ ИМЕТО СИ ТУКА", 21, nrmText, b1TxtRect, InactBrush);
            else
            {
                if(!b1Hglt)
                    Draw->DrawTextW(L"ВЪВЕДИ ИМЕТО СИ ТУКА", 21, nrmText, b1TxtRect, TxtBrush);
                else
                    Draw->DrawTextW(L"ВЪВЕДИ ИМЕТО СИ ТУКА", 21, nrmText, b1TxtRect, HgltBrush);
            }

            if(!b2Hglt)
                Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, TxtBrush);
            else
                Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, HgltBrush);
            if (!b3Hglt)
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, TxtBrush);
            else
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, HgltBrush);
        }

        current_field_frame++;
        if (current_field_frame > 15)current_field_frame = 0;
        Draw->DrawBitmap(bmpField[current_field_frame], D2D1::RectF(0, 50.0f, scr_width, scr_height));

        //ISLANDS *****************************************

        for (int i = 0; i < 3; i++)
        {
            if (ActiveScreen.Island1.x > 0)
                Draw->DrawBitmap(bmpIsland1, D2D1::RectF(ActiveScreen.Island1.x, ActiveScreen.Island1.y,
                    ActiveScreen.Island1.ex, ActiveScreen.Island1.ey));
            if (ActiveScreen.Island2.x > 0)
                Draw->DrawBitmap(bmpIsland2, D2D1::RectF(ActiveScreen.Island2.x, ActiveScreen.Island2.y,
                    ActiveScreen.Island2.ex, ActiveScreen.Island2.ey));
            if (ActiveScreen.Island3.x > 0)
                Draw->DrawBitmap(bmpIsland3, D2D1::RectF(ActiveScreen.Island3.x, ActiveScreen.Island3.y,
                    ActiveScreen.Island3.ex, ActiveScreen.Island3.ey));
            if (ActiveScreen.FinalIsland.x > 0)
                Draw->DrawBitmap(bmpFinal, D2D1::RectF(ActiveScreen.FinalIsland.x, ActiveScreen.FinalIsland.y,
                    ActiveScreen.FinalIsland.ex, ActiveScreen.FinalIsland.ey));
        }

        //MY SHIP ****************************************

        if (Hero)
        {
            switch (Hero->dir)
            {
            case dirs::stop:
                if (Hero->x > scr_width / 2)
                    Draw->DrawBitmap(bmpHeroR[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                else
                    Draw->DrawBitmap(bmpHeroL[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::up:
                if (Hero->x > scr_width / 2)
                    Draw->DrawBitmap(bmpHeroR[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                else
                    Draw->DrawBitmap(bmpHeroL[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::down:
                if (Hero->x > scr_width / 2)
                    Draw->DrawBitmap(bmpHeroR[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                else
                    Draw->DrawBitmap(bmpHeroL[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::right:
                Draw->DrawBitmap(bmpHeroL[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;

            case dirs::left:
                Draw->DrawBitmap(bmpHeroR[Hero->GetFrame()], D2D1::RectF(Hero->x, Hero->y, Hero->ex, Hero->ey));
                break;
            }

            if (GreenBrush && YellowBrush && RedBrush)
            {
                if (Hero->lifes > 80)Draw->DrawLine(D2D1::Point2F(Hero->x, Hero->ey + 10.0f),
                    D2D1::Point2F(Hero->x + Hero->lifes / 1.5f, Hero->ey + 10.0f), GreenBrush, 10.0f);
                else if(Hero->lifes>30)Draw->DrawLine(D2D1::Point2F(Hero->x, Hero->ey + 10.0f),
                    D2D1::Point2F(Hero->x + Hero->lifes / 1.5f, Hero->ey + 10.0f), YellowBrush, 10.0f);
                else Draw->DrawLine(D2D1::Point2F(Hero->x, Hero->ey + 10.0f),
                    D2D1::Point2F(Hero->x + Hero->lifes / 1.5f, Hero->ey + 10.0f), RedBrush, 10.0f);
            }
        }

        if (!vMyBoulders.empty())
        {
            for (int i = 0; i < vMyBoulders.size(); i++)
                Draw->DrawBitmap(bmpBall, D2D1::RectF(vMyBoulders[i].Shot.Dims.x, vMyBoulders[i].Shot.Dims.y,
                    vMyBoulders[i].Shot.Dims.ex, vMyBoulders[i].Shot.Dims.ey));
        }

        if (!vMyBoulders.empty() && !vPirates.empty())
        {
            for (std::vector<dll::obj_ptr>::iterator pirate = vPirates.begin(); pirate < vPirates.end(); pirate++)
            {
                bool killed = false;

                for (std::vector<SHOTDATA>::iterator shot = vMyBoulders.begin(); shot < vMyBoulders.end(); shot++)
                {
                    if (!((*pirate)->x > shot->Shot.Dims.ex || (*pirate)->ex < shot->Shot.Dims.x ||
                        (*pirate)->y>shot->Shot.Dims.ey || (*pirate)->ey < shot->Shot.Dims.y))
                    {
                        (*pirate)->lifes -= 30;
                        vMyBoulders.erase(shot);

                        if ((*pirate)->lifes <= 0)
                        {
                            score += game_speed * 20;
                            dll::ATOM anExplosion((*pirate)->x, (*pirate)->y, 200.0f, 227.0f);
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            vExplosions.push_back(EXPLOSION(anExplosion, -1));
                            (*pirate)->Release();
                            vPirates.erase(pirate);
                            killed = true;
                        }
                        break;
                    }
                }

                if (killed)break;
            }
        }


        //PIRATES *****************************************

        if (!vPirates.empty())
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                switch ((*it)->type)
                {
                case types::bad1:
                    if ((*it)->dir == dirs::left)
                        Draw->DrawBitmap(bmpBad1R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else if ((*it)->dir == dirs::right)
                        Draw->DrawBitmap(bmpBad1L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else
                    {
                        if ((*it)->x > scr_width / 2)
                            Draw->DrawBitmap(bmpBad1R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                        else
                            Draw->DrawBitmap(bmpBad1L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    }
                    break;

                case types::bad2:
                    if ((*it)->dir == dirs::left)
                        Draw->DrawBitmap(bmpBad2R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else if ((*it)->dir == dirs::right)
                        Draw->DrawBitmap(bmpBad2L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else
                    {
                        if ((*it)->x > scr_width / 2)
                            Draw->DrawBitmap(bmpBad2R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                        else
                            Draw->DrawBitmap(bmpBad2L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    }
                    break;

                case types::bad3:
                    if ((*it)->dir == dirs::left)
                        Draw->DrawBitmap(bmpBad3R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else if ((*it)->dir == dirs::right)
                        Draw->DrawBitmap(bmpBad3L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    else
                    {
                        if ((*it)->x > scr_width / 2)
                            Draw->DrawBitmap(bmpBad3R[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                        else
                            Draw->DrawBitmap(bmpBad3L[(*it)->GetFrame()], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                    }
                    break;
                }

                if (GreenBrush && YellowBrush && RedBrush)
                {
                    if ((*it)->lifes > 80)Draw->DrawLine(D2D1::Point2F((*it)->x, (*it)->ey + 10.0f),
                        D2D1::Point2F((*it)->x + (*it)->lifes / 1.5f, (*it)->ey + 10.0f), GreenBrush, 10.0f);
                    else if ((*it)->lifes > 30)Draw->DrawLine(D2D1::Point2F((*it)->x, (*it)->ey + 10.0f),
                        D2D1::Point2F((*it)->x + (*it)->lifes / 1.5f, (*it)->ey + 10.0f), YellowBrush, 10.0f);
                    else Draw->DrawLine(D2D1::Point2F((*it)->x, (*it)->ey + 10.0f),
                        D2D1::Point2F((*it)->x + (*it)->lifes / 1.5f, (*it)->ey + 10.0f), RedBrush, 10.0f);
                }

            }
        }

        if (!vPirates.empty() && ActiveScreen.Island1.x > -1)
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                if (!((*it)->x > ActiveScreen.Island1.ex || (*it)->ex<ActiveScreen.Island1.x ||
                    (*it)->y>ActiveScreen.Island1.ey || (*it)->ey < ActiveScreen.Island1.y))
                {
                    int new_dir = rand() % 3;
                    switch ((*it)->dir)
                    {
                    case dirs::left:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::right:
                        if (new_dir == 0)(*it)->dir = dirs::left;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::up:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::left;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::down:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::up;
                        break;

                    }
                    break;
                }
            }
        }
        if (!vPirates.empty() && ActiveScreen.Island2.x > -1)
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                if (!((*it)->x > ActiveScreen.Island2.ex || (*it)->ex<ActiveScreen.Island2.x ||
                    (*it)->y>ActiveScreen.Island2.ey || (*it)->ey < ActiveScreen.Island2.y))
                {
                    int new_dir = rand() % 3;
                    switch ((*it)->dir)
                    {
                    case dirs::left:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::right:
                        if (new_dir == 0)(*it)->dir = dirs::left;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::up:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::left;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::down:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::up;
                        break;

                    }
                    break;
                }
            }
        }
        if (!vPirates.empty() && ActiveScreen.Island3.x > -1)
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                if (!((*it)->x > ActiveScreen.Island3.ex || (*it)->ex<ActiveScreen.Island3.x ||
                    (*it)->y>ActiveScreen.Island3.ey || (*it)->ey < ActiveScreen.Island3.y))
                {
                    int new_dir = rand() % 3;
                    switch ((*it)->dir)
                    {
                    case dirs::left:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::right:
                        if (new_dir == 0)(*it)->dir = dirs::left;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::up:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::left;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::down:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::up;
                        break;

                    }
                    break;
                }
            }
        }
        if (!vPirates.empty() && ActiveScreen.FinalIsland.x > -1)
        {
            for (std::vector<dll::obj_ptr>::iterator it = vPirates.begin(); it < vPirates.end(); it++)
            {
                if (!((*it)->x > ActiveScreen.FinalIsland.ex || (*it)->ex<ActiveScreen.FinalIsland.x ||
                    (*it)->y>ActiveScreen.FinalIsland.ey || (*it)->ey < ActiveScreen.FinalIsland.y))
                {
                    int new_dir = rand() % 3;
                    switch ((*it)->dir)
                    {
                    case dirs::left:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::right:
                        if (new_dir == 0)(*it)->dir = dirs::left;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::up:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::left;
                        else (*it)->dir = dirs::down;
                        break;

                    case dirs::down:
                        if (new_dir == 0)(*it)->dir = dirs::right;
                        else if (new_dir == 1)(*it)->dir = dirs::up;
                        else (*it)->dir = dirs::up;
                        break;

                    }
                    break;
                }
            }
        }

        if (!vEvilBoulders.empty())
        {
            for (int i = 0; i < vEvilBoulders.size(); i++)
                Draw->DrawBitmap(bmpBall, D2D1::RectF(vEvilBoulders[i].Shot.Dims.x, vEvilBoulders[i].Shot.Dims.y,
                    vEvilBoulders[i].Shot.Dims.ex, vEvilBoulders[i].Shot.Dims.ey));
        }
        //////////////////////////////////////////////////

        //EXPLOSIONS *************************************

        if (!vExplosions.empty())
        {
            for (std::vector<EXPLOSION>::iterator it = vExplosions.begin(); it < vExplosions.end(); it++)
            {
                it->frame++;
                if (it->frame > 23)
                {
                    vExplosions.erase(it);
                    if (hero_killed)GameOver();
                    break;
                }
                Draw->DrawBitmap(bmpExplosion[it->frame], D2D1::RectF(it->Dims.x, it->Dims.y, it->Dims.ex, it->Dims.ey));
                
            }
        }

        ////////////////////////////////////////////////////
        Draw->EndDraw();
    }

    ClearRes();
    std::remove(tmp_file);
    return (int) bMsg.wParam;
}