// ImageCompressor.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "ImageCompressor.h"
#include "ICompressor.h"
#include "SimpleCompressor.h"
#include "ParallelCompressor.h"
#include "Stopwatch.h"
#include <string>
#include "OpenMpCompressor.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HBITMAP hCompressedBmp = NULL;
HWND hMainWindow;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR szBmpFilePath[] = L"C:\\Users\\Rukin\\source\\repos\\Cpp\\ImageCompressor\\ImageCompressor\\sample_1.bmp";
Stopwatch _stopwatch;
double _compressWorkingTime = 0;
double _decompressWorkingTime = 0;


// кнопки
HWND hCompressButton = NULL;
HWND hUseParallelCompressorButton = NULL;
HWND hUseOpenMpCompressorButton = NULL;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap);
HWND CreateButton(const wchar_t* buttonText, int x, int y, int width, int height, HWND parentWindow);
HBITMAP CompressAndDecompressImage(HBITMAP hBitmap, ICompressor* compressor);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IMAGECOMPRESSOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGECOMPRESSOR));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGECOMPRESSOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IMAGECOMPRESSOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hMainWindow)
   {
      return FALSE;
   }

   ShowWindow(hMainWindow, nCmdShow);
   UpdateWindow(hMainWindow);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hBmp;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // ловим нажатие на кнопку
            if (HIWORD(wParam) == BN_CLICKED)
            {
                int threadsCount = 50;
                if ((HWND)lParam == hCompressButton) // если нажата кнопка для сжатия изображения с использованием простого компрессора
                {
                    // сжимаем изображение
                    hCompressedBmp = CompressAndDecompressImage(hBmp, new SimpleCompressor());

                    // объявляем все окно недействительным, чтобы оно было перерисовано
                    InvalidateRect(hMainWindow, NULL, TRUE);
                }
                else if ((HWND)lParam == hUseParallelCompressorButton) // если нажата кнопка для сжатия изображения с использованием параллельного компрессора
                {
                    // сжимаем изображение
                    hCompressedBmp = CompressAndDecompressImage(hBmp, new ParallelCompressor(threadsCount));

                    // объявляем все окно недействительным, чтобы оно было перерисовано
                    InvalidateRect(hMainWindow, NULL, TRUE);
                }
                else if ((HWND)lParam == hUseOpenMpCompressorButton) // если нажата кнопка для сжатия изображения с использованием параллельного компрессора OpenMp
                {
                    // сжимаем изображение
                    hCompressedBmp = CompressAndDecompressImage(hBmp, new OpenMpCompressor(threadsCount));

                    // объявляем все окно недействительным, чтобы оно было перерисовано
                    InvalidateRect(hMainWindow, NULL, TRUE);
                }
            }
            // Разобрать выбор в меню:
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
    case WM_CREATE:
        {
            int error;
            // создаем кнопку
            hCompressButton = CreateButton(L"Use Simple Compressor", 50, 50, 200, 40, hWnd);
            hUseParallelCompressorButton = CreateButton(L"Use Parallel Compressor", 300, 50, 200, 40, hWnd);
            hUseOpenMpCompressorButton = CreateButton(L"Use OpenMp Compressor", 550, 50, 200, 40, hWnd);
            ShowWindow(hCompressButton, SW_SHOWNORMAL);
            ShowWindow(hUseParallelCompressorButton, SW_SHOWNORMAL);
            ShowWindow(hUseOpenMpCompressorButton, SW_SHOWNORMAL);
            // загружаем картинку
            hBmp = (HBITMAP)LoadImageW(NULL, szBmpFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            break;
        }
    case WM_PAINT:
        {
            std::string messageCompress = "Current compress time: " + std::to_string(_compressWorkingTime * 1000) + " ms";
            std::string messageDecompress = "Current decompress time: " + std::to_string(_decompressWorkingTime * 1000) + " ms";
            PAINTSTRUCT ps;
            BITMAP bm;
            HDC hdc = BeginPaint(hWnd, &ps);
            int imageY = 100, imageX = 100;
            DrawBitmap(hdc, imageX, imageY, hBmp);
            if (hCompressedBmp != NULL)
            {
                GetObject(hCompressedBmp, sizeof(BITMAP), &bm);
                DrawBitmap(hdc, imageX + bm.bmWidth + imageX, imageY, hCompressedBmp);
                TextOutA(hdc, imageX, imageY + bm.bmHeight + 50, messageCompress.c_str(), messageCompress.size());
                TextOutA(hdc, imageX, imageY + bm.bmHeight + 100, messageDecompress.c_str(), messageDecompress.size());
            }
            EndPaint(hWnd, &ps);
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

// Обработчик сообщений для окна "О программе".
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


/// <summary>
/// Отображает картинку
/// </summary>
/// <param name="hDC">контекст устройства</param>
/// <param name="x">координата х верхнего левого угла</param>
/// <param name="y">координата у верхнего левого угла</param>
/// <param name="hBitmap">картинка</param>
void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;
    POINT ptSize, ptOrg;

    // Создаем контекст памяти, совместимый
    // с контекстом отображения
    hMemDC = CreateCompatibleDC(hDC);

    // Выбираем изображение bitmap в контекст памяти
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // Если не было ошибок, продолжаем работу
    if (hOldbm)
    {
        // Для контекста памяти устанавливаем тот же
        // режим отображения, что используется в
        // контексте отображения
        SetMapMode(hMemDC, GetMapMode(hDC));

        // Определяем размеры изображения
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

        ptSize.x = bm.bmWidth;  // ширина
        ptSize.y = bm.bmHeight; // высота

        // Преобразуем координаты устройства в логические
        // для устройства вывода
        DPtoLP(hDC, &ptSize, 1);

        ptOrg.x = 0;
        ptOrg.y = 0;

        // Преобразуем координаты устройства в логические
        // для контекста памяти
        DPtoLP(hMemDC, &ptOrg, 1);

        // Рисуем изображение bitmap
        BitBlt(hDC, x, y, ptSize.x, ptSize.y,
            hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);

        // Восстанавливаем контекст памяти
        SelectObject(hMemDC, hOldbm);
    }

    // Удаляем контекст памяти
    DeleteDC(hMemDC);
}


HBITMAP CompressAndDecompressImage(HBITMAP hBitmap, ICompressor* compressor)
{
    // получаем пиксельные данные изображения
    HDC hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBitmap);
    BITMAPINFO bmpInfo;
    BITMAP bm;
    COLORREF* pixels;
    HBITMAP restoredBitMap = NULL;

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmpInfo.bmiHeader.biWidth = bm.bmWidth;
    bmpInfo.bmiHeader.biHeight = -bm.bmHeight;
    bmpInfo.bmiHeader.biPlanes = bm.bmPlanes;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biSizeImage = 0;

    pixels = new COLORREF[bm.bmWidth * bm.bmHeight];

    GetDIBits(hdc, hBitmap, 0, bm.bmHeight, pixels, &bmpInfo, DIB_RGB_COLORS);

    // выполняем сжатие
    _stopwatch.Start();
    unsigned short* compressedPixels = compressor->Compress((unsigned int*)pixels, bm.bmWidth, bm.bmHeight);
    _stopwatch.Stop();
    _compressWorkingTime = _stopwatch.Get();
    _stopwatch.Reset();

    // выполняем расшифровку, чтобы затем отобразить
    _stopwatch.Start();
    unsigned int* restoredPixels = compressor->Decompress(compressedPixels, bm.bmWidth, bm.bmHeight);
    _stopwatch.Stop();
    _decompressWorkingTime = _stopwatch.Get();
    _stopwatch.Reset();

    // запихиваем байты в битмап
    restoredBitMap = CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);
    SetDIBits(hdc, restoredBitMap, 0, bm.bmHeight, restoredPixels, &bmpInfo, DIB_RGB_COLORS);

    ReleaseDC(NULL, hdc);
    DeleteDC(hdc);
    // DeleteObject(hBitmap);
    delete[] pixels;

    return restoredBitMap;
}

HWND CreateButton(const wchar_t* buttonText, int x, int y, int width, int height, HWND parentWindow)
{
    HWND hwndButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        buttonText,      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        y,         // y position 
        width,        // Button width
        height,        // Button height
        parentWindow,     // Parent window
        NULL,       // No menu.
        hInst,
        NULL);      // Pointer not needed.
    return hwndButton;
}

