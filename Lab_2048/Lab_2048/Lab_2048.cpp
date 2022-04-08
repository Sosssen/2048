// Lab_2048.cpp : Defines the entry point for the application.
//
#pragma comment(lib, "Msimg32.lib")

#include "framework.h"
#include "Lab_2048.h"
#include <string>
#include <iostream>
#include <fstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szChildClass[MAX_LOADSTRING];

int numbers[12] = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
COLORREF colors[12] = { RGB(204, 192, 174), RGB(238,228,198), RGB(239,225,218), RGB(243, 179, 124), RGB(246, 153, 100), RGB(246, 125,98), RGB(247, 93, 60), RGB(237, 206, 116), RGB(239, 204, 98), RGB(243, 201, 85), RGB(238, 200, 72), RGB(239, 192, 47) };
int blocks[4][4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} };
int score = 0;
int goal = 8;
bool DrawGreen = false;
bool DrawRed = false;
bool KeyboardLocked = false;

RECT RC;
HWND hWndMain;
HWND hWndSecond;
bool pause;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                RegisterChildClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void CountScore();
void CheckIfWon(HWND hWnd);
void CheckIfLost(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2048, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHILD, szChildClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    RegisterChildClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2048));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(250, 247, 238)));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2048);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

ATOM RegisterChildClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndChildProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(250, 247, 238)));
    wcex.lpszClassName = szChildClass;
    wcex.hIconSm = nullptr;
    wcex.lpszMenuName = nullptr;

    return RegisterClassExW(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT rc = RECT();


   int i = rand() % 4;
   int j = rand() % 4;
   blocks[i][j] = 1;

   std::ifstream stream;
   stream.open("save.txt");
   std::string s;

   for (int i = 0; i < 4; i++) {
       for (int j = 0; j < 4; j++) {
           getline(stream, s);
           blocks[i][j] = std::stoi(s);
       }
   }

   getline(stream, s);
   score = std::stoi(s);

   getline(stream, s);
   goal = std::stoi(s);
   



   rc.left = 0;
   rc.right = 290;
   rc.top = 0;
   rc.bottom = 360;


   AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, TRUE);

   RC = rc;
   int width = rc.right - rc.left;
   int height = rc.bottom - rc.top;
   pause = false;

   hWndMain = CreateWindowW(
       szWindowClass,
       szTitle,
       WS_OVERLAPPEDWINDOW | WS_VISIBLE,
       100,
       100,
       width,
       height,
       nullptr,
       nullptr,
       hInstance,
       nullptr
   );

   int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
   int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);


   GetWindowRect(hWndMain, &rc);

   int newX = ScreenWidth - rc.right;
   int newY = ScreenHeight - rc.bottom;

   hWndSecond = CreateWindowExW(
       WS_EX_TOOLWINDOW,
       szWindowClass,
       szTitle,
       WS_CAPTION | WS_OVERLAPPED | WS_VISIBLE,
       newX,
       newY,
       width,
       height,
       nullptr,
       nullptr,
       hInstance,
       nullptr
   );

   


 
   if (!hWndMain)
   {
      return FALSE;
   }



   ShowWindow(hWndMain, nCmdShow);
   ShowWindow(hWndSecond, nCmdShow);
   UpdateWindow(hWndMain);

   if (goal == 8) {
       HMENU hMenu1 = GetMenu(hWndMain);
       HMENU hMenu2 = GetMenu(hWndSecond);
       CheckMenuItem(hMenu1, IDM_8, MF_CHECKED);
       CheckMenuItem(hMenu2, IDM_8, MF_CHECKED);
       CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);
   }
   else if (goal == 16) {
       HMENU hMenu1 = GetMenu(hWndMain);
       HMENU hMenu2 = GetMenu(hWndSecond);
       CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_16, MF_CHECKED);
       CheckMenuItem(hMenu2, IDM_16, MF_CHECKED);
       CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);
   }
   else if (goal == 64) {
       HMENU hMenu1 = GetMenu(hWndMain);
       HMENU hMenu2 = GetMenu(hWndSecond);
       CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_64, MF_CHECKED);
       CheckMenuItem(hMenu2, IDM_64, MF_CHECKED);
       CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);
   }
   else if (goal == 2048) {
       HMENU hMenu1 = GetMenu(hWndMain);
       HMENU hMenu2 = GetMenu(hWndSecond);
       CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
       CheckMenuItem(hMenu1, IDM_2048, MF_CHECKED);
       CheckMenuItem(hMenu2, IDM_2048, MF_CHECKED);
   }

   int x;
   getline(stream, s);
   x = std::stoi(s);
   if (x == 1) KeyboardLocked = true;
   else KeyboardLocked = false;

   getline(stream, s);
   x = std::stoi(s);
   if (x == 1) DrawGreen = true;
   else DrawGreen = false;

   getline(stream, s);
   x = std::stoi(s);
   if (x == 1) DrawRed = true;
   else DrawRed = false;

   RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
   RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {

    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_NG:
            {
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        blocks[i][j] = 0;
                    }
                }

                int i = rand() % 4;
                int j = rand() % 4;
                blocks[i][j] = 1;

                score = 0;
                DrawGreen = false;
                DrawRed = false;
                KeyboardLocked = false;

                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                break;
            }
            case IDM_8:
            {

                HMENU hMenu1 = GetMenu(hWndMain);
                HMENU hMenu2 = GetMenu(hWndSecond);
                CheckMenuItem(hMenu1, IDM_8, MF_CHECKED);
                CheckMenuItem(hMenu2, IDM_8, MF_CHECKED);
                CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);

                goal = 8;

                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                break;
            }
            case IDM_16:
            {
                HMENU hMenu1 = GetMenu(hWndMain);
                HMENU hMenu2 = GetMenu(hWndSecond);
                CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_16, MF_CHECKED);
                CheckMenuItem(hMenu2, IDM_16, MF_CHECKED);
                CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);

                goal = 16;

                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                break;
            }
            case IDM_64:
            {

                HMENU hMenu1 = GetMenu(hWndMain);
                HMENU hMenu2 = GetMenu(hWndSecond);
                CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_64, MF_CHECKED);
                CheckMenuItem(hMenu2, IDM_64, MF_CHECKED);
                CheckMenuItem(hMenu1, IDM_2048, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_2048, MF_UNCHECKED);

                goal = 64;

                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                break;
            }
            case IDM_2048:
            {
                HMENU hMenu1 = GetMenu(hWndMain);
                HMENU hMenu2 = GetMenu(hWndSecond);
                CheckMenuItem(hMenu1, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_8, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_16, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu2, IDM_64, MF_UNCHECKED);
                CheckMenuItem(hMenu1, IDM_2048, MF_CHECKED);
                CheckMenuItem(hMenu2, IDM_2048, MF_CHECKED);

                goal = 2048;

                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                break;
            }
            case IDM_D:
            {
                if (KeyboardLocked) {
                    break;
                }

                bool createNew = false;

                int counter = 0;

                for (int i = 0; i < 4; i++) {

                    for (int j = 0; j < 3; j++) {
                        if (blocks[i][j] != 0) {
                            if (blocks[i][j + 1] == blocks[i][j])
                            {
                                blocks[i][j + 1] += 1;
                                score += numbers[blocks[i][j]] * 2;
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                            else if (blocks[i][j + 1] == 0)
                            {
                                blocks[i][j + 1] = blocks[i][j];
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                        }
                    }
                }

                CountScore();

                bool Is2Inserted = false;
                int isFull = true;

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] == 0) isFull = false;
                    }
                }

                if (createNew) {
                    while (!Is2Inserted && !isFull) {
                        int i = rand() % 4;
                        int j = rand() % 4;
                        if (blocks[i][j] == 0) {
                            blocks[i][j] = 1;
                            Is2Inserted = true;
                        }
                    }
                }


                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                CheckIfWon(hWnd);
               
                CheckIfLost(hWnd);
                

                break;
            }
            case IDM_S:
            {
                if (KeyboardLocked) {
                    break;
                }

                bool createNew = false;

                int counter = 0;

                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] != 0) {
                            if (blocks[i + 1][j] == blocks[i][j])
                            {
                                blocks[i + 1][j] += 1;
                                score += numbers[blocks[i][j]] * 2;
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                            else if (blocks[i + 1][j] == 0) {
                                blocks[i + 1][j] = blocks[i][j];
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                        }
                    }
                }

                CountScore();

                bool Is2Inserted = false;
                int isFull = true;

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] == 0) isFull = false;
                    }
                }

                if (createNew) {
                    while (!Is2Inserted && !isFull) {
                        int i = rand() % 4;
                        int j = rand() % 4;
                        if (blocks[i][j] == 0) {
                            blocks[i][j] = 1;
                            Is2Inserted = true;
                        }
                    }
                }

                
                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                CheckIfWon(hWnd);
             
                CheckIfLost(hWnd);
               

                break;
            }
            case IDM_A:
            {
                if (KeyboardLocked) {
                    break;
                }

                bool createNew = false;

                int counter = 0;

                for (int i = 0; i < 4; i++)
                {
                    for (int j = 3; j > 0; j--) {
                        if (blocks[i][j] != 0) {
                            if (blocks[i][j - 1] == blocks[i][j]) {
                                blocks[i][j - 1] += 1;
                                score += numbers[blocks[i][j]] * 2;
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                            else if (blocks[i][j - 1] == 0) {
                                blocks[i][j - 1] = blocks[i][j];
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                        }
                    }
                }

                CountScore();

                bool Is2Inserted = false;
                int isFull = true;

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] == 0) isFull = false;
                    }
                }

                if (createNew) {
                    while (!Is2Inserted && !isFull) {
                        int i = rand() % 4;
                        int j = rand() % 4;
                        if (blocks[i][j] == 0) {
                            blocks[i][j] = 1;
                            Is2Inserted = true;
                        }
                    }
                }


                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                CheckIfWon(hWnd);
               
                CheckIfLost(hWnd);
                

                break;
            }
            case IDM_W:
            {
                if (KeyboardLocked) {
                    break;
                }

                bool createNew = false;

                int counter = 0;

                for (int i = 3; i > 0; i--) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] != 0) {
                            if (blocks[i - 1][j] == blocks[i][j]) {
                                blocks[i - 1][j] += 1;
                                score += numbers[blocks[i][j]] * 2;
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                            else if (blocks[i - 1][j] == 0) {
                                blocks[i - 1][j] = blocks[i][j];
                                blocks[i][j] = 0;
                                counter++;
                                createNew = true;
                            }
                        }
                    }
                }

                CountScore();

                bool Is2Inserted = false;
                int isFull = true;

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (blocks[i][j] == 0) isFull = false;
                    }
                }

                if (createNew) {
                    while (!Is2Inserted && !isFull) {
                        int i = rand() % 4;
                        int j = rand() % 4;
                        if (blocks[i][j] == 0) {
                            blocks[i][j] = 1;
                            Is2Inserted = true;
                        }
                    }
                }


                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

                CheckIfWon(hWnd);
                
                CheckIfLost(hWnd);
                

                break;
            }
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;

        minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x = RC.right - RC.left;
        minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y = RC.bottom - RC.top;
        minMaxInfo->ptMinTrackSize.x = RC.right - RC.left;
        minMaxInfo->ptMinTrackSize.y = RC.bottom - RC.top;

        break;
    }
    case WM_MOVE:
    {

        if (pause) break;

        pause = true;

        HWND WindowToMove = nullptr;
        if (hWnd == hWndMain) WindowToMove = hWndSecond;
        else if (hWnd == hWndSecond) WindowToMove = hWndMain;

        int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        RECT rc;

        GetWindowRect(hWnd, &rc);

        int newX = ScreenWidth - rc.right;
        int newY = ScreenHeight - rc.bottom;

        MoveWindow(WindowToMove, newX, newY, RC.right - RC.left, RC.bottom - RC.top, FALSE);

        // TU SIĘ COŚ PSUJE, CZASAMI GŁÓWNE OKNO DOSTAJE TEMPLATKE DRUGIEGO

        int MiddleX = ScreenWidth / 2;
        int MiddleY = ScreenHeight / 2;

        if (rc.bottom > MiddleY && rc.top < MiddleY && rc.right > MiddleX && rc.left < MiddleX)
        {
            if (hWnd == hWndSecond)
            {
                SetWindowLong(hWndSecond, GWL_EXSTYLE, GetWindowLong(hWndSecond, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWndSecond, 0, (255 * 50) / 100, LWA_ALPHA);
                SetWindowLong(hWndMain, GWL_EXSTYLE, GetWindowLong(hWndMain, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWndMain, 0, 255, LWA_ALPHA);
            }
            else if (hWnd == hWndMain)
            {
                SetWindowLong(hWndMain, GWL_EXSTYLE, GetWindowLong(hWndMain, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWndMain, 0, (255 * 50) / 100, LWA_ALPHA);
                SetWindowLong(hWndSecond, GWL_EXSTYLE, GetWindowLong(hWndSecond, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWndSecond, 0, 255, LWA_ALPHA);
            }
        }
        else
        {
            SetWindowLong(hWndMain, GWL_EXSTYLE, GetWindowLong(hWndMain, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWndMain, 0, 255, LWA_ALPHA);
            SetWindowLong(hWndSecond, GWL_EXSTYLE, GetWindowLong(hWndSecond, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWndSecond, 0, 255, LWA_ALPHA);
        }

        

        pause = false;

        break;
    }
    case WM_PAINT:
        {
           
        
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // TUTAJ CHYBA JUŻ NIC ZMIENIAĆ NIE TRZEBA - JEST BIAŁE, DUŻE I NA PRZEZROCZYSTYM TLE
            HFONT font = CreateFont(
                -MulDiv(15, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                0,
                0,
                0,
                1000,
                false,
                FALSE,
                0,
                EASTEUROPE_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS,
                _T("Verdana"));
            HFONT oldFont = (HFONT)SelectObject(hdc, font);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, 0x00FFFFFF);
            
            // TODO: Add any drawing code that uses hdc here...
            HPEN pen = CreatePen(PS_NULL, 3, RGB(0, 0, 0));
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            HBRUSH brush = CreateSolidBrush(RGB(204, 192, 174));
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
            RoundRect(hdc, 10, 10, 280, 70, 15, 15);

            std::string temp;
            std::wstring temp2;
            LPCWSTR s = nullptr;
            RECT rc;


            rc.left = 10;
            rc.top = 10;
            rc.right = 280;
            rc.bottom = 70;
            temp = std::to_string(score);
            temp2 = std::wstring(temp.begin(), temp.end());
            s = temp2.c_str();
            DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    DeleteObject(brush);
                    brush = CreateSolidBrush(colors[blocks[i][j]]);
                    oldBrush = (HBRUSH)SelectObject(hdc, brush);
                    RoundRect(hdc, 10 + j * (10 + 60), 80 + i * (10 + 60), (j + 1)* (10 + 60), 70 + (i + 1) * (10 + 60), 15, 15);
                    rc.left = 10 + j * (10 + 60);
                    rc.top = 80 + i * (10 + 60);
                    rc.right = (j + 1) * (10 + 60);
                    rc.bottom = 70 + (i + 1) * (10 + 60);
                    if (numbers[blocks[i][j]] == 0) temp = "";
                    else temp = std::to_string(numbers[blocks[i][j]]);
                    temp2 = std::wstring(temp.begin(), temp.end());
                    s = temp2.c_str();
                    DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


                }
            }

            if (DrawGreen) {
                KeyboardLocked = true;
                HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 128;
                bf.AlphaFormat = 0;
                AlphaBlend(hdc, 0, 0, 290, 360, memDC, 0, 0, 2, 2, bf);

                SelectObject(memDC, oldBitmap);
                DeleteObject(bitmap);
                DeleteDC(memDC);
                
                rc.left = 0;
                rc.right = 290;
                rc.top = 0;
                rc.bottom = 360;


                HFONT fontGreen = CreateFont(
                    -MulDiv(50, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                    0,
                    0,
                    0,
                    1000,
                    false,
                    FALSE,
                    0,
                    EASTEUROPE_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_SWISS,
                    _T("Verdana"));
                HFONT oldFontGreen = (HFONT)SelectObject(hdc, fontGreen);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, 0x00FFFFFF);

                TCHAR greenS[] = _T("WIN!");
                DrawText(hdc, greenS, (int)_tcslen(greenS), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hdc, oldFontGreen);
                DeleteObject(fontGreen);
            }


            if (DrawRed) {
                KeyboardLocked = true;
                HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 128;
                bf.AlphaFormat = 0;
                AlphaBlend(hdc, 0, 0, 290, 360, memDC, 0, 0, 2, 2, bf);

                SelectObject(memDC, oldBitmap);
                DeleteObject(bitmap);
                DeleteDC(memDC);

                rc.left = 0;
                rc.right = 290;
                rc.top = 0;
                rc.bottom = 360;


                HFONT fontRed = CreateFont(
                    -MulDiv(30, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                    0,
                    0,
                    0,
                    1000,
                    false,
                    FALSE,
                    0,
                    EASTEUROPE_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_SWISS,
                    _T("Verdana"));
                HFONT oldFontRed = (HFONT)SelectObject(hdc, fontRed);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, 0x00FFFFFF);

                TCHAR redS[] = _T("GAME OVER!");
                DrawText(hdc, redS, (int)_tcslen(redS), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hdc, oldFontRed);
                DeleteObject(fontRed);
            }

            
            DeleteObject(brush);
            SelectObject(hdc, oldFont);
            DeleteObject(font);
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
            
            
            EndPaint(hWnd, &ps);

            std::ofstream stream;
            stream.open("save.txt");

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    stream << blocks[i][j] << "\n";
                }
            }
            stream << score << "\n";
            stream << goal << "\n";
            if (KeyboardLocked) stream << 1;
            else stream << 0;
            stream << "\n";
            if (DrawGreen) stream << 1;
            else stream << 0;
            stream << "\n";
            if (DrawRed) stream << 1;
            else stream << 0;
            stream << "\n";

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
    case WM_TIMER:
    {

        break;

    }
    case WM_CREATE:
    {
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void CountScore() {

 /*   score = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if(numbers[blocks[i][j]] != 2)
                score += numbers[blocks[i][j]];
        }
    }*/

    return;
}

void CheckIfWon(HWND hWnd) {

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (numbers[blocks[i][j]] >= goal) {
                DrawGreen = true;
                RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
                RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
            }
        }
    }
}

void CheckIfLost(HWND hWnd) {


    DrawRed = true;


    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i - 1 >= 0 && blocks[i][j] == blocks[i - 1][j]) DrawRed = false;
            else if (i + 1 < 4 && blocks[i][j] == blocks[i + 1][j]) DrawRed = false;
            else if (j - 1 >= 0 && blocks[i][j] == blocks[i][j - 1]) DrawRed = false;
            else if (j + 1 < 4 && blocks[i][j] == blocks[i][j + 1]) DrawRed = false;
            if (blocks[i][j] == 0) DrawRed = false;
        }
    }

    RedrawWindow(hWndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
    RedrawWindow(hWndSecond, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}
