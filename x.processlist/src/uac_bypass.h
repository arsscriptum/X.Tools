

//==============================================================================
//
//  ps_enum.h: CEnumProcess class.
//
//==============================================================================
//  Copyright (C) Guilaume Plante 2020 <cybercastor@icloud.com>
//==============================================================================


#ifndef ___BYPASS_UAC__H__
#define ___BYPASS_UAC__H__

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include "psapi.h"
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <Psapi.h>
#include <sddl.h>
#include <AclAPI.h>
#include <winternl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace C
{
    namespace UAC
    {

        /*++
        Routine Description: This routine returns TRUE if the caller's
        process is a member of the Administrators local group. Caller is NOT
        expected to be impersonating anyone and is expected to be able to
        open its own process and process token.
        Arguments: None.
        Return Value:
           TRUE - Caller has Administrators local group.
           FALSE - Caller does not have Administrators local group. --
        */
        BOOL IsUserAdmin();

        // Return error code
        DWORD MakeEveryoneAbleToWaitForProcess(HANDLE hProcess);
        void FillParentProcessID(LPTSTR lpBuffer, SIZE_T BufferCount);

        void FillSessionID(LPTSTR lpBuffer, SIZE_T BufferCount);

        LPCWSTR GetCurrentProcessPath();
        // --------------------
        // When Run As Normal User
        // --------------------

        void BypassUAC(HANDLE hToken);
        // --------------------
        // When Run As Administrator
        // --------------------

        DWORD LaunchService();
        // --------------------
        // When Run As Service
        // --------------------


        DWORD WINAPI HandleEx(DWORD dwControl, DWORD dwEventType,
            LPVOID lpEventData, LPVOID lpContext);
        void WINAPI ServiceMain(_In_ DWORD dwArgc, _In_ LPTSTR lpszArgv[]);
        BOOL FindElevatedProcess(LPDWORD lpProcessID, PHANDLE lpProcessHandle, PHANDLE lpProcessToken);
        int UAC_Init(int argc, TCHAR argv[], TCHAR envp);
    }
}

#endif //___BYPASS_UAC__H__