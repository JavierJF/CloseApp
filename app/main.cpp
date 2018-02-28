#include <CloseApp/CloseApp.h>

#include <UIHandler/SysUtils/ProcInspection.h>
#include <UIHandler/UI/Handler.h>

#include <UIAutomation.h>
#include <Windows.h>

#include <iostream>
#include <string>

using WString = std::wstring;

int wmain(int argc, wchar_t* argv[]) {
    //                          Arguments Check
    // =========================================================================
    std::cout << ":: Checking arguments." << "\r\n";
    error_t argsCheckErr = checkArguments(argc, argv);
    if (argsCheckErr != E_FN_SUCCESS) {
        std::cout << ":: Err :: Bad arguments." << "\r\n";
        return argsCheckErr;
    }
    // =========================================================================

    //                          Variables declaration
    // =========================================================================
    LPTSTR _appPath     = argv[1];
    int n               = _wtoi(argv[2]);

    // RAII strings
    WString appPath { _appPath };

    error_t resCode     = E_FN_SUCCESS;
    ProcId  pId         = 0;

    // Window Handles
    HWND windowHWND     = NULL;

    //                                  Logic
    // =========================================================================
    std::cout << ":: Looking for App process Id." << "\r\n";
    resCode = findAppByPath(appPath, &pId);
    if (resCode != E_FN_SUCCESS) {
        std::cout << ":: Err :: Unable to find App by path." << "\r\n";
        return resCode;
    }

    // Find the window by the obtained process id
    std::cout << ":: Looking for windows Handle." << "\r\n";
    windowHWND = findWindowByProcessId(pId);
    if (windowHWND == NULL) {
        std::cout << ":: Err :: Unable to find window HWND by procid." << "\r\n";
        resCode = E_FN_INVAL;

        return resCode;
    }

    // Send close message to window
    std::cout << ":: Looking for windows Handle." << "\r\n";
    PostMessage(windowHWND, WM_CLOSE, NULL, NULL);

    // Trigger application exit button
    std::cout << ":: Triggering application exit button." << "\r\n";
    resCode = triggerExitButton(&windowHWND, n);

    // Check flags from dialogs
    if (resCode != E_FN_SUCCESS) {
        std::cout << ":: Err :: Unable to trigger button" << "\r\n";
        std::cout << ":: Err :: Final ResCode: " << resCode << "\r\n";
    }

    return resCode;
}
