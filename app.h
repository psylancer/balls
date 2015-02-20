#include "windows.h"
#include "tchar.h"
#include "engine.h"

#define WINDOW_CLASSNAME _T("BallsApp")
#define WINDOW_TITLE _T("Balls")

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
