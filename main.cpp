#include "app.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, INT cmdShow)
{
    srand(GetTickCount());
    TApplication* app=new TApplication(hInstance,cmdShow);
    return app->MainLoop();
}
