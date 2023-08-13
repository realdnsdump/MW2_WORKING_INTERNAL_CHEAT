#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <random>
#include <dxgi1_4.h>
#include <limits>
#include <algorithm>
#include <cmath>
#include <d2d1.h>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include "lazyimporter.h"

extern "C" { NTSTATUS _NtUserSendInput(UINT a1, LPINPUT Input, int Size); };

namespace sysingthecall
{
    auto GetSyscallIndex(std::string ModuleName, std::string SyscallFunctionName, void* Function) -> bool
    {
        auto ModuleBaseAddress = iat(GetModuleHandleA)(ModuleName.c_str());
        if (!ModuleBaseAddress)
            ModuleBaseAddress = iat(LoadLibraryA)(ModuleName.c_str());
        if (!ModuleBaseAddress)
            return false;

        auto GetFunctionAddress = iat(GetProcAddress)(ModuleBaseAddress, SyscallFunctionName.c_str());
        if (!GetFunctionAddress)
            return false;

        auto SyscallIndex = *(DWORD*)((PBYTE)GetFunctionAddress + 4);

        *(DWORD*)((PBYTE)Function + 4) = SyscallIndex;

        return true;
    }



    VOID send_mouse_input(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo)
    {
        static bool doneonce;
        if (!doneonce)
        {
            if (!GetSyscallIndex("win32u.dll", ("NtUserSendInput"), _NtUserSendInput))
                return;
            doneonce = true;
        }

        INPUT Input[3] = { 0 };
        Input[0].type = INPUT_MOUSE;
        Input[0].mi.dx = dx;
        Input[0].mi.dy = dy;
        Input[0].mi.mouseData = dwData;
        Input[0].mi.dwFlags = dwFlags;
        Input[0].mi.time = 0;
        Input[0].mi.dwExtraInfo = dwExtraInfo;

        _NtUserSendInput((UINT)1, (LPINPUT)&Input, (INT)sizeof(INPUT));
    }
}