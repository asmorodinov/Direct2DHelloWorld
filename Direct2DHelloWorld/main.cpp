#include <iostream>

#include "DemoApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL))) {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize())) {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}
