#include <CloseApp/CloseApp.h>
#include <UIHandler/UI/Handler.h>

#include <Shlwapi.h>
#include <UIAnimation.h>

#pragma comment(lib, "Shlwapi.lib")

#include <iostream>

error_t checkArguments(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        return E_FN_INVAL;
    }

    LPTSTR _appPath = NULL;
    LPTSTR _buttonNum = NULL;

    _appPath = argv[1];
    _buttonNum = argv[2];

    if (_appPath == NULL || _buttonNum == NULL) {
        return E_FN_INVAL;
    }

    bool pathExists = PathFileExists(_appPath);
    if (!pathExists) {
        return E_FN_INVAL;
    }

    return E_FN_SUCCESS;
}

error_t findAppByPath(const WString &path, ProcId *pid) {
    if (pid == NULL) {
        return E_FN_INVAL;
    }

    error_t resCode = E_FN_SUCCESS;

    ProcId* pIds     = (ProcId*)malloc(sizeof(ProcId)*ST_RUNNING_PROCS);
    ProcId_A pIdsArr = { pIds, ST_RUNNING_PROCS, 0 };
    TChar_A pPathArr = { (LPTSTR)path.c_str(), path.length()+1, 0 };

    auto fProcErr = findProcByPath(&pPathArr, &pIdsArr);
    if (fProcErr != E_FN_SUCCESS) {
        resCode = E_FN_INVAL;
        goto Cleanup;
    }

    // Get the first result from the array of pIds
    *pid = *pIds;

Cleanup:
    if (pIds != NULL) {
        free(pIds);
    }

    return resCode;
}

error_t tryTriggerExitButton(const HWND* windowHWND, int32_t n) {
    if (windowHWND == NULL) {
        return E_FN_INVAL;
    }

    error_t errCode = E_FN_SUCCESS;
    error_t dialogNumErr = E_FN_SUCCESS;

    // UIA API Access
    IUIAutomationElement* windowUIE = NULL;
    IUIAutomation*  UIAutomation    = NULL;

    std::cout << "-> :: Finding AppWindow Element." << "\r\n";
    errCode = findWindowUIElemByHwnd(*windowHWND, &windowUIE);
    if (errCode != E_FN_SUCCESS) {
        std::cout << "-> :: Err :: Unable to get AppWindow element." << "\r\n";
        return E_UI_GET_ELEM;
    }

    std::cout << "-> :: Getting UIAutomation interface." << "\r\n";
    errCode = getInterface(&UIAutomation);
    if (errCode != E_FN_SUCCESS || UIAutomation == NULL) {
        std::cout << "-> :: Err:: Unable to get window UIA interface.";
        return E_UI_RTS_INVAL_STATE;
    }

    VARIANT classNameProp;
    classNameProp.vt = VT_BSTR;
    classNameProp.bstrVal= SysAllocString(L"\x0023""32770");
    IUIAutomationCondition* classNameCond = NULL;

    errCode =
        UIAutomation->CreatePropertyCondition(
            UIA_ClassNamePropertyId,
            classNameProp,
            &classNameCond
        );
    if (FAILED(errCode) || classNameCond == NULL) {
        std::cout << "-> :: Err:: Failed to create dialog property condition."  << "\r\n";
        return errCode;
    }

    IUIAutomationElementArray* dialogs = NULL;
    IUIAutomationElement* dialog = NULL;

    std::cout << "-> :: Trying to find dialog window." << "\r\n";
    errCode = windowUIE->FindAll(TreeScope_Descendants, classNameCond, &dialogs);
    if (FAILED(errCode)) {
        std::cout << "-> :: Err:: Unable to find window UIA interface." << "\r\n";
        return errCode;
    }

    int numDialogs = 0;
    errCode = dialogs->get_Length(&numDialogs);
    if (FAILED(errCode) || numDialogs == 0) {
        std::cout << "-> :: Err :: Unable to find dialog window." << "\r\n";
        return  E_UI_GET_ELEM;
    }

    if (numDialogs > 1) {
        std::cout << "-> :: WARN :: Found multiple dialogs, choosing first" << "\r\n";
        dialogNumErr = E_CL_MULTIPLE_DIALOGS;
    }

    errCode = dialogs->GetElement(0, &dialog);
    if (FAILED(errCode)) {
        std::cout << "-> :: Err :: Can't return dialog UIElement" << "\r\n";
        return errCode;
    }

    VARIANT buttonTypeProp;
    buttonTypeProp.vt = VT_I4;
    buttonTypeProp.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* buttonTypeCond = NULL;

    errCode =
        UIAutomation->CreatePropertyCondition(
            UIA_ControlTypePropertyId,
            buttonTypeProp,
            &buttonTypeCond
        );
    if (FAILED(errCode) || buttonTypeCond == NULL) {
        std::cout << "-> :: Err:: Failed to create button property condition."  << "\r\n";
        return errCode;
    }

    std::cout << "-> :: Trying to find dialog buttons." << "\r\n";
    IUIAutomationElementArray* buttons = NULL;
    IUIAutomationElement* button = NULL;
    errCode = dialog->FindAll(TreeScope_Descendants, buttonTypeCond, &buttons);
    if (FAILED(errCode) || buttons == NULL) {
        std::cout << "-> :: Err:: Unable to find dialog buttons."  << "\r\n";
        return errCode;
    }

    int nButtons = 0;
    errCode = buttons->get_Length(&nButtons);
    if (FAILED(errCode)) {
        std::cout << "-> :: Err:: Failed to get number of buttons."  << "\r\n";
        return errCode;
    }

    if (n > nButtons) {
        std::cout << "-> :: Err:: Supplied button number is bigger than number of buttons."  << "\r\n";
        return E_CL_INVAL_N_BUTTONS;
    }
    errCode = buttons->GetElement(n, &button);
    if (FAILED(errCode)) {
        std::cout << "-> :: Err:: Failed to get button UIElement."  << "\r\n";
        return errCode;
    }

    IUIAutomationInvokePattern* invokePattern = NULL;
    errCode = button->GetCurrentPattern(UIA_InvokePatternId, (IUnknown**)&invokePattern);
    if (FAILED(errCode)) {
        std::cout << "-> :: Err:: Failed to get Invokable pattern for button."  << "\r\n";
        return errCode;
    }

    std::cout << "-> :: Invoking exit button." << "\r\n";
    errCode = invokePattern->Invoke();
    if (FAILED(errCode)) {
        std::cout << "-> :: Err:: Failed to invoke button."  << "\r\n";
    }

    SysFreeString(classNameProp.bstrVal);

    return errCode;
}

error_t triggerExitButton(const HWND* windowHWND, uint32_t n) {
    if (windowHWND == NULL) {
        return E_FN_INVAL;
    }

    error_t resCode = E_FN_SUCCESS;

    //                               RTS Init
    // =========================================================================

    // Initialize MTA thread, requierement for COM call to UIA
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    std::cout << "-> :: Initializing UIA RTS" << "\r\n";
    char* _argv = "init";

    error_t rtsInitErr = initHRTS(1, &_argv);
    if (rtsInitErr != E_FN_SUCCESS) {
        std::cout << "-> :: Err :: RTS initilization failed." << "\r\n";
        resCode = E_GV_SYS_WIN32ERROR;
        goto Cleanup;
    }
    // =========================================================================

    int retries = 5;

    for (int i = 0; i < retries; i++) {
        Sleep(1000);

        resCode = tryTriggerExitButton(windowHWND, n);

        if (resCode == E_FN_SUCCESS) {
            break;
        }
    }

Cleanup:

    //                              RTS Shutdown
    // =========================================================================
    std::cout << "-> :: Closing UIA RTS" << "\r\n";

    errno_t rtsClosingErr = closeHRTS();
    // Note: This error is being ignored for return because it will fade the
    // internal returned one, and it isn't so relevant one the operation is
    // completed.
    if (rtsClosingErr != E_FN_SUCCESS) {
        std::cout << "-> :: RTS initilization failed." << "\r\n";
    }

    CoUninitialize();
    // =========================================================================

    return resCode;
}

