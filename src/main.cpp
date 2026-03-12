#include <windows.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "phatom.hpp"
#include "phatom_utils.hpp"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define EDIT_WIDTH 80
#define EDIT_HEIGHT 25
#define LABEL_WIDTH 120
#define LABEL_HEIGHT 25

HWND hwndImage1 = nullptr;
HWND hwndImage2 = nullptr;
HWND hwndBtn1 = nullptr;
Phantom* g_phantom = nullptr;

// Поля ввода параметров
HWND hwndEditDelta = nullptr;
HWND hwndEditSigma = nullptr;
HWND hwndEditMinR = nullptr;
HWND hwndEditMaxR = nullptr;
HWND hwndEditCount = nullptr;
void drawPhantom(HWND hwnd, Phantom* phantom) {
    HDC hdc = GetDC(hwnd);
    int w = phantom->getW();
    int h = phantom->getH();
    double** data = phantom->get_phantom_data();

    // BGRA формат (4 канала)
    BYTE* pixels = new BYTE[w * h * 4];
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            BYTE alpha = (BYTE)(data[y][x] * 255);
            // Цвет фона - белый (255), смешиваем с альфой
            pixels[idx] = alpha;     // B
            pixels[idx+1] = alpha;   // G
            pixels[idx+2] = alpha;   // R
            pixels[idx+3] = 255;     // A (всегда непрозрачный)
        }
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc, 0, 0, w, h, 0, 0, 0, h, pixels, &bmi, DIB_RGB_COLORS);

    delete[] pixels;
    ReleaseDC(hwnd, hdc);
}

void drawGraph(HWND hwnd, Phantom* phantom) {
    HDC hdc = GetDC(hwnd);
    int w = phantom->getW();
    int h = phantom->getH();
    double* dataline = phantom->get_dataline();

    // Очистка фона (белый)
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT rect = {0, 0, w, h};
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // Находим мин/макс для масштабирования
    double minY = dataline[0], maxY = dataline[0];
    for (int x = 1; x < w; x++) {
        if (dataline[x] < minY) minY = dataline[x];
        if (dataline[x] > maxY) maxY = dataline[x];
    }
    double range = maxY - minY;
    if (range < 0.001) range = 1.0;

    // Рисуем график (синяя линия)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(hdc, hPen);

    MoveToEx(hdc, 0, h - (int)((dataline[0] - minY) / range * h), nullptr);
    for (int x = 1; x < w; x++) {
        int y = h - (int)((dataline[x] - minY) / range * h);
        LineTo(hdc, x, y);
    }

    DeleteObject(hPen);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    srand(time(0));
    switch (msg) {
        case WM_CREATE: {
            // Первая область для картинки
            hwndImage1 = CreateWindowW(
                L"STATIC", L"Phatom",
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
                (WINDOW_WIDTH - IMAGE_WIDTH * 2 - 100) / 2,
                50,
                IMAGE_WIDTH, IMAGE_HEIGHT,
                hwnd, nullptr, nullptr, nullptr
            );

            // Вторая область для картинки
            hwndImage2 = CreateWindowW(
                L"STATIC", L"Graph",
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
                (WINDOW_WIDTH - IMAGE_WIDTH * 2 - 100) / 2 + IMAGE_WIDTH + 100,
                50,
                IMAGE_WIDTH, IMAGE_HEIGHT,
                hwnd, nullptr, nullptr, nullptr
            );

            // Кнопка под первой картинкой
            hwndBtn1 = CreateWindowW(
                L"BUTTON", L"Generate",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                (WINDOW_WIDTH - IMAGE_WIDTH * 2 - 100) / 2 + IMAGE_WIDTH / 2 - BUTTON_WIDTH / 2,
                50 + IMAGE_HEIGHT + 20,
                BUTTON_WIDTH, BUTTON_HEIGHT,
                hwnd, (HMENU)1, nullptr, nullptr
            );

            // Параметры (справа от картинки)
            int paramX = (WINDOW_WIDTH - IMAGE_WIDTH * 2 - 100) / 2 + IMAGE_WIDTH + 100;
            int paramY = 50 + IMAGE_HEIGHT + 20;
            int labelX = paramX;
            int editX = paramX + LABEL_WIDTH + 10;

            // Delta
            CreateWindowW(L"STATIC", L"Delta:",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                labelX, paramY, LABEL_WIDTH, LABEL_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            hwndEditDelta = CreateWindowW(L"EDIT", L"0.1",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                editX, paramY, EDIT_WIDTH, EDIT_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            paramY += LABEL_HEIGHT + 10;

            // Sigma
            CreateWindowW(L"STATIC", L"Sigma:",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                labelX, paramY, LABEL_WIDTH, LABEL_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            hwndEditSigma = CreateWindowW(L"EDIT", L"0",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                editX, paramY, EDIT_WIDTH, EDIT_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            paramY += LABEL_HEIGHT + 10;

            // MinR
            CreateWindowW(L"STATIC", L"Min Radius:",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                labelX, paramY, LABEL_WIDTH, LABEL_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            hwndEditMinR = CreateWindowW(L"EDIT", L"50",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                editX, paramY, EDIT_WIDTH, EDIT_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            paramY += LABEL_HEIGHT + 10;

            // MaxR
            CreateWindowW(L"STATIC", L"Max Radius:",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                labelX, paramY, LABEL_WIDTH, LABEL_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            hwndEditMaxR = CreateWindowW(L"EDIT", L"100",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                editX, paramY, EDIT_WIDTH, EDIT_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            paramY += LABEL_HEIGHT + 10;

            // Count
            CreateWindowW(L"STATIC", L"Count:",
                WS_CHILD | WS_VISIBLE | SS_RIGHT,
                labelX, paramY, LABEL_WIDTH, LABEL_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);
            hwndEditCount = CreateWindowW(L"EDIT", L"100",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                editX, paramY, EDIT_WIDTH, EDIT_HEIGHT,
                hwnd, nullptr, nullptr, nullptr);

            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) {
                // Чтение параметров из полей ввода
                wchar_t buffer[32];
                
                GetWindowTextW(hwndEditDelta, buffer, 32);
                double delta = _wtof(buffer);
                
                GetWindowTextW(hwndEditSigma, buffer, 32);
                double sigma = _wtof(buffer);
                
                GetWindowTextW(hwndEditMinR, buffer, 32);
                int minR = _wtoi(buffer);
                
                GetWindowTextW(hwndEditMaxR, buffer, 32);
                int maxR = _wtoi(buffer);
                
                GetWindowTextW(hwndEditCount, buffer, 32);
                int count = _wtoi(buffer);

                if (g_phantom) delete g_phantom;
                g_phantom = new Phantom(IMAGE_WIDTH, IMAGE_HEIGHT, delta, sigma, minR, maxR, count);
                g_phantom->generate_phantom();
                save_data(g_phantom);
                save_image(g_phantom);
                drawPhantom(hwndImage1, g_phantom);
                drawGraph(hwndImage2, g_phantom);
            }
            break;
        }

        case WM_DESTROY:
            if (g_phantom) delete g_phantom;
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"ImageGeneratorWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    int posX = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
    int posY = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Phatoms generator",
        WS_OVERLAPPEDWINDOW,
        posX, posY,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
