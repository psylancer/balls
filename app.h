
#ifndef _APP_H_
#define _APP_H_

#include <windows.h>

class TEngine;
class TGame;

class TApplication {
private:
    HINSTANCE hInstance;
    HWND hWindow;
    POINT mouseCoords;
    TEngine* engine;
    TGame* game;
    BOOL moveStarted;
    BOOL detonateStarted;
    BOOL appearStarted;
    void RegWindow();
    static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ProcessGame();
public:
    TApplication(HINSTANCE hInstance, INT cmdShow);
    ~TApplication();
    TEngine* GetEngine();
    TGame* GetGame();
    INT MainLoop();
};

#endif
