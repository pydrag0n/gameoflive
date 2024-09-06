
#include <Windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WW 1040
#define WH 540

#define PointSize 5
#define mapW 200
#define mapH 100

int epoh = 0;
HWND window;
HDC hdc;
HGLRC hrc;

int paused = 1;
char MAP[mapH][mapW];
char NewMAP[mapH][mapW];

void drawSQ(int x, int y) {
    glColor3f(0.1, 0.8, 0.1);
    glPointSize(PointSize);
    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
}

short getRandomNum() {
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return (short)(rand() % 2);
}

short CheckColNightborn(int coordY, int coordX) {
    int nightborn = 0;
    for (int y = coordY - 1; y <= coordY + 1; y++) {
        for (int x = coordX - 1; x <= coordX + 1; x++) {
            if (x >= 0 && y >= 0 && x < mapW && y < mapH && !(x == coordX && y == coordY)) {
                nightborn += MAP[y][x];
            }
        }
    }
    return nightborn;
}

void GenerateMap(void) {
    for (int jh = 0; jh < mapH; jh++) {
        for (int iw = 0; iw < mapW; iw++) {
            MAP[jh][iw] = getRandomNum();
        }
    }
}

void draw(void) {
    int x = 0;
    int y = 0;

    for (int jh = 0; jh < mapH; jh++) {
        for (int iw = 0; iw < mapW; iw++) {
            short colnighborn = CheckColNightborn(jh, iw);
            printf("%i", epoh);
            if (MAP[jh][iw] > 0) {
                drawSQ(x, y);
            }

            if (colnighborn == 3 || (colnighborn == 2 && MAP[jh][iw] > 0)) {
                NewMAP[jh][iw] = 1;
            } else {
                NewMAP[jh][iw] = 0;
            }

            x += PointSize;
        }
        y += PointSize;
        x = 0;
    }

    for (int jh = 0; jh < mapH; jh++) {
        for (int iw = 0; iw < mapW; iw++) {
            epoh+=1;
            MAP[jh][iw] = NewMAP[jh][iw];
            printf("%i", epoh);
        }
    }
}

void R_Render(void)
{
    if (paused<1) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        Sleep(1000/60);
        draw();
        SwapBuffers(hdc);
    }
}

void GL_Init(void)
{

    GenerateMap();
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    hdc = GetDC(window);
    memset(&pfd, 0, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(hdc, &pfd);

    SetPixelFormat(hdc, iFormat, &pfd);

    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);
    glOrtho(0, WW, WH, 0, -1, 1);
}

void GL_Shutdown(void)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(window, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_DESTROY:
            return 0;
        case WM_SIZE:
            {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                glViewport(0, 0, width, height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, width, height, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
            break;
        case WM_KEYDOWN:
            if (wParam == 'P' || wParam == 'p') {
                paused = !paused; // Toggle pause state
            }
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wcl;
    memset(&wcl, 0, sizeof(wcl));

    wcl.lpfnWndProc = WndProc;
    wcl.lpszClassName = "ABC_CLASS";

    RegisterClassA(&wcl);

    window = CreateWindowExA(0, "ABC_CLASS", "name", WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 
    0, 0, WW, WH, 0, 0, 0, 0);

    GL_Init();
    ShowWindow(window, SW_SHOWNORMAL);

    MSG msg;

    while(1) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            R_Render();
        }
    }

    GL_Shutdown();
    DestroyWindow(window);

    return 0;
}