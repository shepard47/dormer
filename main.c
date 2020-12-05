#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int
main()
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = DefWindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(0);
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "window";
    wc.hIconSm       = LoadIcon(0, IDI_APPLICATION);

    if(RegisterClassEx(&wc) == 0){
        perror("window class");
        exit(-1);
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "window", "The title of my window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 240, 120, 0, 0, GetModuleHandle(0), 0);
    if(hwnd == 0){
        perror("window creation");
        exit(-1);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, 0, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        switch(msg.message){
        case WM_KEYDOWN:
            puts("helo rofik");
        }
    }

    return 0;
}
