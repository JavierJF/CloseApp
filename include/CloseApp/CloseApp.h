#ifndef __CLOSE_APP_UI_H
#define __CLOSE_APP_UI_H

#include <string>
#include <UIHandler/Error.h>
#include <UIHandler/SysUtils/ProcInspection.h>

using WString = std::wstring;

enum ClsAppErr {
    E_CL_MULTIPLE_DIALOGS = -0x40,
    E_CL_INVAL_N_BUTTONS = -0x60
};

/**
 * @brief
 *  Check main function arguments.
 * @param argc
 *  Number of arguments.
 * @param argv
 *  Arguments.
 * @return
 *  E_FN_SUCCESS if arguments are valid, E_FN_INVAL otherwise.
 */
error_t checkArguments(int argc, wchar_t* argv[]);
/**
 * @brief
 *  Find the process pid form the application path.
 * @param path
 *  The path to the application executable.
 * @param pid
 *  Pointer to process pid.
 * @return
 *  E_FN_SUCCESS if pid was successfully retrieved, or one of the following
 *  errors:
 *      - TODO.
 */
error_t findAppByPath(const WString& path, ProcId* pid);
/**
 * @brief
 *  Trigger the supplied button number from the exit dialog of an application.
 * @param windowHWND
 *  The window handle of the application window.
 * @param n
 *  The button from the exit dialog to be invoked.
 * @return
 *  E_FN_SUCCESS if the button could be invoked successfully, or one of the
 *  following errors:
 *      - TODO.
 */
error_t triggerExitButton(const HWND* windowHWND, uint32_t n);

#endif
