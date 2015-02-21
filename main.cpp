
#include "app.h"
#include <tchar.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow)
{
    srand(GetTickCount());
    TApplication* app=new TApplication(hInstance,nCmdShow);
    return app->MainLoop();
}
