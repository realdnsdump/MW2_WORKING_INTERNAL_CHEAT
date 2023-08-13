#include "stdafx.h"
#include "sdk.h"
#include "game.h"
#include "lazyimporter.h"
#include "memory.h"
#include <map>
#include "defs.h"
#include "globals.h"
#include <cstdint>
#pragma comment(lib, "user32.lib")
#define DEBASE(a) ((size_t)a - (size_t)(unsigned long long)GetModuleHandleA(NULL))

namespace process
{
    HWND hwnd;

    BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
    {
        DWORD dwPid = 0;
        GetWindowThreadProcessId(hWnd, &dwPid);
        if (dwPid == lParam)
        {
            hwnd = hWnd;
            return FALSE;
        }
        return TRUE;
    }

    HWND get_process_window()
    {
        if (hwnd)
            return hwnd;

        EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

        if (hwnd == NULL)
            Exit();

        return hwnd;
    }
}

namespace g_data
{
    uintptr_t base;
    HWND hWind = NULL;

    void init()
    {
        base = (uint64_t)(iat(GetModuleHandleA).get()("cod.exe"));

        hWind = process::get_process_window();
        //SetWindowDisplayAffinity(hWind, WDA_EXCLUDEFROMCAPTURE);
    }

}

/*
auto mb = g_data::base;
auto Peb = __readgsqword(0x60);
*/
namespace bnet
{
    uintptr_t get_client_info()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rbx = *(uintptr_t*)(mb + 0x122B0CF8);
        if (!rbx)
            return rbx;
        rdx = Peb;              //mov rdx, gs:[rax]
        //r9 = 000000E1026FEE10//failed to trace. base: 00007FF644580000 It's possibly wrong
        rax = rbx;              //mov rax, rbx
        rax >>= 0xB;            //shr rax, 0x0B
        r8 = 0x1E53D8998E54A23D;                //mov r8, 0x1E53D8998E54A23D
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x16;           //shr rax, 0x16
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x2C;           //shr rax, 0x2C
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rbx ^= rax;             //xor rbx, rax
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= *(uintptr_t*)(mb + 0x9ABC0DD);           //xor rcx, [0x000000000739CF14]
        rax = mb + 0x266259AD;          //lea rax, [0x0000000023F067DD]
        rbx *= r8;              //imul rbx, r8
        rax = ~rax;             //not rax
        rbx -= r9;              //sub rbx, r9
        rax -= rdx;             //sub rax, rdx
        rcx = _byteswap_uint64(rcx);            //bswap rcx
        rbx *= *(uintptr_t*)(rcx + 0xb);                //imul rbx, [rcx+0x0B]
        rbx ^= rax;             //xor rbx, rax
        rax = 0xC338B9C132E5301E;               //mov rax, 0xC338B9C132E5301E
        rbx ^= rax;             //xor rbx, rax
        return rbx;
    }
    uint64_t get_client_info_base()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        r8 = *(uintptr_t*)(get_client_info() + 0x162bb0);
        if (!r8)
            return r8;
        r11 = ~Peb;              //mov r11, gs:[rax]
        rax = r11;              //mov rax, r11
        rax = _rotl64(rax, 0x21);               //rol rax, 0x21
        rax &= 0xF;
        switch (rax) {
        case 0:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E72113]
            rax = 0x9141C45BFD5B39F7;               //mov rax, 0x9141C45BFD5B39F7
            r8 ^= rax;              //xor r8, rax
            rcx = mb;               //lea rcx, [0xFFFFFFFFF93B5BE8]
            rcx += r8;              //add rcx, r8
            r8 = mb + 0x6E33AF72;           //lea r8, [0x00000000676F0B41]
            rax = 0xF605A67470E7C53D;               //mov rax, 0xF605A67470E7C53D
            rcx *= rax;             //imul rcx, rax
            rax = r11;              //mov rax, r11
            rax ^= r8;              //xor rax, r8
            rcx -= rax;             //sub rcx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B5BA1]
            rcx -= rax;             //sub rcx, rax
            r8 = rcx + 0xffffffffd10685d8;          //lea r8, [rcx-0x2EF97A28]
            r8 += r11;              //add r8, r11
            rax = mb + 0x526CB4F4;          //lea rax, [0x000000004BA8141A]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r8 += rax;              //add r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1A;           //shr rax, 0x1A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x34;           //shr rax, 0x34
            r8 ^= rax;              //xor r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            return r8;
        }
        case 1:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E71C6D]
            rax = r8;               //mov rax, r8
            rax >>= 0x24;           //shr rax, 0x24
            r8 ^= rax;              //xor r8, rax
            rax = 0xE570A6F93EC9464F;               //mov rax, 0xE570A6F93EC9464F
            r8 *= rax;              //imul r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = r8;               //mov rax, r8
            rax >>= 0x24;           //shr rax, 0x24
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x11;           //shr rax, 0x11
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x22;           //shr rax, 0x22
            r8 ^= rax;              //xor r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B59B6]
            r8 -= rax;              //sub r8, rax
            r8 += r11;              //add r8, r11
            rax = 0x14F095F380F9EB43;               //mov rax, 0x14F095F380F9EB43
            r8 += rax;              //add r8, rax
            return r8;
        }
        case 2:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E717BA]
            rax = 0x8ADB88DACDCF2087;               //mov rax, 0x8ADB88DACDCF2087
            r8 *= rax;              //imul r8, rax
            rax = 0x7962CBE13BD24CEA;               //mov rax, 0x7962CBE13BD24CEA
            r8 += rax;              //add r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x10;           //shr rax, 0x10
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x20;           //shr rax, 0x20
            r8 ^= rax;              //xor r8, rax
            r8 += r11;              //add r8, r11
            r8 += r11;              //add r8, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            r8 -= r11;              //sub r8, r11
            rax = r8;               //mov rax, r8
            rax >>= 0x1D;           //shr rax, 0x1D
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x3A;           //shr rax, 0x3A
            r8 ^= rax;              //xor r8, rax
            return r8;
        }
        case 3:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E713EF]
            rax = r8;               //mov rax, r8
            rax >>= 0x25;           //shr rax, 0x25
            rax ^= r8;              //xor rax, r8
            r8 = 0x99B04B837FD2242B;                //mov r8, 0x99B04B837FD2242B
            rax += r8;              //add rax, r8
            r8 = mb;                //lea r8, [0xFFFFFFFFF93B5200]
            r8 += rax;              //add r8, rax
            r8 ^= r11;              //xor r8, r11
            rax = mb;               //lea rax, [0xFFFFFFFFF93B5019]
            r8 -= rax;              //sub r8, rax
            rax = mb + 0x11751E8B;          //lea rax, [0x000000000AB06DD0]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r8 += rax;              //add r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = 0xDA47FAB853EFDBF7;               //mov rax, 0xDA47FAB853EFDBF7
            r8 *= rax;              //imul r8, rax
            return r8;
        }
        case 4:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E70F34]
            rax = r8;               //mov rax, r8
            rax >>= 0x2;            //shr rax, 0x02
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x4;            //shr rax, 0x04
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x8;            //shr rax, 0x08
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x10;           //shr rax, 0x10
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x20;           //shr rax, 0x20
            r8 ^= rax;              //xor r8, rax
            rax = 0x718CFE6D52D76081;               //mov rax, 0x718CFE6D52D76081
            r8 *= rax;              //imul r8, rax
            rax = r11;              //mov rax, r11
            rax ^= r8;              //xor rax, r8
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            r8 = mb + 0x746D9936;           //lea r8, [0x000000006DA8E4D6]
            r8 ^= rax;              //xor r8, rax
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rax = mb;               //lea rax, [0xFFFFFFFFF93B4B8F]
            rcx = ~rcx;             //not rcx
            r8 *= *(uintptr_t*)(rcx + 0x19);                //imul r8, [rcx+0x19]
            rcx = 0x6A0BA8494B6820F5;               //mov rcx, 0x6A0BA8494B6820F5
            r8 += rcx;              //add r8, rcx
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x15;           //shr rax, 0x15
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x2A;           //shr rax, 0x2A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1C;           //shr rax, 0x1C
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x38;           //shr rax, 0x38
            r8 ^= rax;              //xor r8, rax
            return r8;
        }
        case 5:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC10E);             //mov r9, [0x0000000002E709B5]
            rax = r8;               //mov rax, r8
            rax >>= 0x15;           //shr rax, 0x15
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x2A;           //shr rax, 0x2A
            r8 ^= rax;              //xor r8, rax
            rax = 0x6B4D10E3FCFC0235;               //mov rax, 0x6B4D10E3FCFC0235
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x3;            //shr rax, 0x03
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x6;            //shr rax, 0x06
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0xC;            //shr rax, 0x0C
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x18;           //shr rax, 0x18
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x30;           //shr rax, 0x30
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1A;           //shr rax, 0x1A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x34;           //shr rax, 0x34
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1C;           //shr rax, 0x1C
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x38;           //shr rax, 0x38
            r8 ^= rax;              //xor r8, rax
            uintptr_t RSP_0x58;
            RSP_0x58 = 0x9FE7D7D2C91086EF;          //mov rax, 0x9FE7D7D2C91086EF : RSP+0x58
            r8 *= RSP_0x58;                 //imul r8, [rsp+0x58]
            uintptr_t RSP_0x50;
            RSP_0x50 = 0x3B044E06AA0DC65D;          //mov rax, 0x3B044E06AA0DC65D : RSP+0x50
            r8 += RSP_0x50;                 //add r8, [rsp+0x50]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            return r8;
        }
        case 6:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC10E);             //mov r9, [0x0000000002E70466]
            rax = r8;               //mov rax, r8
            rax >>= 0xF;            //shr rax, 0x0F
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1E;           //shr rax, 0x1E
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x3C;           //shr rax, 0x3C
            r8 ^= rax;              //xor r8, rax
            r8 -= r11;              //sub r8, r11
            r8 ^= r11;              //xor r8, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            rax = *(uintptr_t*)(rax + 0x19);                //mov rax, [rax+0x19]
            uintptr_t RSP_0x48;
            RSP_0x48 = 0xB64C05FA8BB41ED5;          //mov rax, 0xB64C05FA8BB41ED5 : RSP+0x48
            rax *= RSP_0x48;                //imul rax, [rsp+0x48]
            r8 *= rax;              //imul r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B4279]
            r8 -= rax;              //sub r8, rax
            rax = 0xA1839DE961442277;               //mov rax, 0xA1839DE961442277
            r8 *= rax;              //imul r8, rax
            return r8;
        }
        case 7:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6FFE9]
            rax = mb + 0x7E9B;              //lea rax, [0xFFFFFFFFF93BB965]
            rcx = r11;              //mov rcx, r11
            r8 += rax;              //add r8, rax
            rcx = ~rcx;             //not rcx
            r8 += rcx;              //add r8, rcx
            rax = r8;               //mov rax, r8
            rax >>= 0xD;            //shr rax, 0x0D
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1A;           //shr rax, 0x1A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x34;           //shr rax, 0x34
            r8 ^= rax;              //xor r8, rax
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0x70;
            RSP_0x70 = mb + 0x2B12;                 //lea rax, [0xFFFFFFFFF93B69C1] : RSP+0x70
            rax ^= RSP_0x70;                //xor rax, [rsp+0x70]
            r8 -= rax;              //sub r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x27;           //shr rax, 0x27
            r8 ^= rax;              //xor r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = 0x6B0468CD6B4B36F5;               //mov rax, 0x6B0468CD6B4B36F5
            r8 *= rax;              //imul r8, rax
            rax = 0x67D32343BA582459;               //mov rax, 0x67D32343BA582459
            r8 -= rax;              //sub r8, rax
            rax = 0x2AB381DC49040AEF;               //mov rax, 0x2AB381DC49040AEF
            r8 ^= rax;              //xor r8, rax
            return r8;
        }
        case 8:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6FAE6]
            rax = 0xF27764D7BCC134E1;               //mov rax, 0xF27764D7BCC134E1
            r8 *= rax;              //imul r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x7;            //shr rax, 0x07
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0xE;            //shr rax, 0x0E
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1C;           //shr rax, 0x1C
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x38;           //shr rax, 0x38
            r8 ^= rax;              //xor r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B38E6]
            r8 ^= rax;              //xor r8, rax
            rax = 0xC3107C6F6CB6AAB7;               //mov rax, 0xC3107C6F6CB6AAB7
            r8 *= rax;              //imul r8, rax
            rax = 0xD1B5E7C8461A7E03;               //mov rax, 0xD1B5E7C8461A7E03
            r8 ^= rax;              //xor r8, rax
            rax = mb + 0x4977696C;          //lea rax, [0x0000000042B2A179]
            rax = ~rax;             //not rax
            rax += r11;             //add rax, r11
            r8 += rax;              //add r8, rax
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0x60] -- didn't find trace -> use base
            rax += 0xFFFFFFFFFFFF4E09;              //add rax, 0xFFFFFFFFFFFF4E09
            r8 += rax;              //add r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            return r8;
        }
        case 9:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6F64C]
            rax = r8;               //mov rax, r8
            rax >>= 0x23;           //shr rax, 0x23
            r8 ^= rax;              //xor r8, rax
            rax = 0xA7B0F0AA378850A7;               //mov rax, 0xA7B0F0AA378850A7
            r8 *= rax;              //imul r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B324E]
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1B;           //shr rax, 0x1B
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x36;           //shr rax, 0x36
            r8 ^= rax;              //xor r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B31A9]
            rax += 0xD85;           //add rax, 0xD85
            rax += r11;             //add rax, r11
            r8 += rax;              //add r8, rax
            r8 -= mb;               //sub r8, [rsp+0x60] -- didn't find trace -> use base
            uintptr_t RSP_0xFFFFFFFFFFFFFF90;
            RSP_0xFFFFFFFFFFFFFF90 = 0x4E5E1AE762C3863A;            //mov rax, 0x4E5E1AE762C3863A : RBP+0xFFFFFFFFFFFFFF90
            r8 ^= RSP_0xFFFFFFFFFFFFFF90;           //xor r8, [rbp-0x70]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            return r8;
        }
        case 10:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC10E);             //mov r9, [0x0000000002E6F192]
            rax = 0x98CD10A39FEEABC3;               //mov rax, 0x98CD10A39FEEABC3
            r8 *= rax;              //imul r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B2F15]
            rax += 0x8AF0;          //add rax, 0x8AF0
            rax += r11;             //add rax, r11
            r8 += rax;              //add r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x10;           //shr rax, 0x10
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x20;           //shr rax, 0x20
            r8 ^= rax;              //xor r8, rax
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            uintptr_t RSP_0xFFFFFFFFFFFFFF90;
            RSP_0xFFFFFFFFFFFFFF90 = mb + 0xF084;           //lea rax, [0xFFFFFFFFF93C2101] : RBP+0xFFFFFFFFFFFFFF90                rax ^= RSP_0xFFFFFFFFFFFFFF90;          //xor rax, [rbp-0x70]
            r8 += rax;              //add r8, rax
            rax = 0xDF8A1660CBF5F30F;               //mov rax, 0xDF8A1660CBF5F30F
            r8 *= rax;              //imul r8, rax
            rax = 0xA829D63D19635A8D;               //mov rax, 0xA829D63D19635A8D
            r8 ^= rax;              //xor r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            r8 ^= r11;              //xor r8, r11
            return r8;
        }
        case 11:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6ED22]
            rax = r8;               //mov rax, r8
            rax >>= 0x23;           //shr rax, 0x23
            rax ^= r11;             //xor rax, r11
            r8 ^= rax;              //xor r8, rax
            rax = mb + 0x17E7D121;          //lea rax, [0x000000001122F9DD]
            r8 ^= rax;              //xor r8, rax
            rax = r11 + 0x1;                //lea rax, [r11+0x01]
            uintptr_t RSP_0x60;
            RSP_0x60 = mb + 0x7D39B186;             //lea rax, [0x000000007674DD35] : RSP+0x60
            rax *= RSP_0x60;                //imul rax, [rsp+0x60]
            r8 += rax;              //add r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = mb;               //lea rax, [0xFFFFFFFFF93B29FB]
            rax += 0x8CB1;          //add rax, 0x8CB1
            rax += r11;             //add rax, r11
            r8 ^= rax;              //xor r8, rax
            r8 ^= r11;              //xor r8, r11
            rax = 0x71B5B118240CFD7D;               //mov rax, 0x71B5B118240CFD7D
            r8 *= rax;              //imul r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B2A63]
            rax += 0x2611654C;              //add rax, 0x2611654C
            rax += r11;             //add rax, r11
            r8 += rax;              //add r8, rax
            return r8;
        }
        case 12:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6E939]
            rax = mb;               //lea rax, [0xFFFFFFFFF93B256F]
            r8 -= rax;              //sub r8, rax
            rax = mb + 0x4291859E;          //lea rax, [0x000000003BCCAC2A]
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x10;           //shr rax, 0x10
            r8 ^= rax;              //xor r8, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rax = r8;               //mov rax, r8
            rcx = ~rcx;             //not rcx
            rax >>= 0x20;           //shr rax, 0x20
            r8 ^= rax;              //xor r8, rax
            rax = 0x6A01EB295C695943;               //mov rax, 0x6A01EB295C695943
            r8 *= rax;              //imul r8, rax
            rax = 0x9BBE6575DCB15C28;               //mov rax, 0x9BBE6575DCB15C28
            r8 ^= rax;              //xor r8, rax
            r8 *= *(uintptr_t*)(rcx + 0x19);                //imul r8, [rcx+0x19]
            rax = 0x9A76A1C3B04C8361;               //mov rax, 0x9A76A1C3B04C8361
            r8 *= rax;              //imul r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B26BD]
            r8 -= rax;              //sub r8, rax
            return r8;
        }
        case 13:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6E4A8]
            rax = mb;               //lea rax, [0xFFFFFFFFF93B1EA1]
            r8 ^= rax;              //xor r8, rax
            rax = 0x424E7D3CE7A4BDA3;               //mov rax, 0x424E7D3CE7A4BDA3
            r8 *= rax;              //imul r8, rax
            rax = mb + 0x1A767856;          //lea rax, [0x0000000013B197AD]
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            r8 += rax;              //add r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0xF;            //shr rax, 0x0F
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1E;           //shr rax, 0x1E
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x3C;           //shr rax, 0x3C
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1;            //shr rax, 0x01
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x2;            //shr rax, 0x02
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x4;            //shr rax, 0x04
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x8;            //shr rax, 0x08
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x10;           //shr rax, 0x10
            r8 ^= rax;              //xor r8, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rax = r8;               //mov rax, r8
            rcx ^= r10;             //xor rcx, r10
            rax >>= 0x20;           //shr rax, 0x20
            rcx = ~rcx;             //not rcx
            r8 ^= rax;              //xor r8, rax
            r8 *= *(uintptr_t*)(rcx + 0x19);                //imul r8, [rcx+0x19]
            uintptr_t RSP_0x48;
            RSP_0x48 = 0x8DB30096C278A251;          //mov rax, 0x8DB30096C278A251 : RSP+0x48
            r8 *= RSP_0x48;                 //imul r8, [rsp+0x48]
            rax = mb;               //lea rax, [0xFFFFFFFFF93B210E]
            rax += 0x31EB9108;              //add rax, 0x31EB9108
            rax += r11;             //add rax, r11
            r8 ^= rax;              //xor r8, rax
            return r8;
        }
        case 14:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6DF86]
            rcx = mb;               //lea rcx, [0xFFFFFFFFF93B1A93]
            rcx += 0xA832;          //add rcx, 0xA832
            rcx += r11;             //add rcx, r11
            rax = mb;               //lea rax, [0xFFFFFFFFF93B1A7F]
            rcx ^= r8;              //xor rcx, r8
            r8 = r11;               //mov r8, r11
            r8 = ~r8;               //not r8
            r8 += rcx;              //add r8, rcx
            r8 -= rax;              //sub r8, rax
            r8 -= 0x2FECE2F9;               //sub r8, 0x2FECE2F9
            rax = mb;               //lea rax, [0xFFFFFFFFF93B1AC2]
            rax += 0x4948;          //add rax, 0x4948
            rax += r11;             //add rax, r11
            r8 += rax;              //add r8, rax
            rax = 0xE986304E17E64F7D;               //mov rax, 0xE986304E17E64F7D
            r8 *= rax;              //imul r8, rax
            rax = mb + 0x5BB7;              //lea rax, [0xFFFFFFFFF93B788E]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x1A;           //shr rax, 0x1A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x34;           //shr rax, 0x34
            r8 ^= rax;              //xor r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            uintptr_t RSP_0xFFFFFFFFFFFFFFA8;
            RSP_0xFFFFFFFFFFFFFFA8 = mb + 0x30A5920C;               //lea rax, [0x0000000029E0B058] : RBP+0xFFFFFFFFFFFFFFA8
            rax ^= RSP_0xFFFFFFFFFFFFFFA8;          //xor rax, [rbp-0x58]
            r8 += rax;              //add r8, rax
            return r8;
        }
        case 15:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC10E);            //mov r10, [0x0000000002E6DA2D]
            rcx = mb + 0x56C36699;          //lea rcx, [0x000000004FFE7EFE]
            rax = 0x14288A7031FA1D2A;               //mov rax, 0x14288A7031FA1D2A
            r8 += rax;              //add r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0xA;            //shr rax, 0x0A
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x14;           //shr rax, 0x14
            r8 ^= rax;              //xor r8, rax
            rax = r8;               //mov rax, r8
            rax >>= 0x28;           //shr rax, 0x28
            r8 ^= rax;              //xor r8, rax
            rax = 0xF471F0FE111CB275;               //mov rax, 0xF471F0FE111CB275
            r8 *= rax;              //imul r8, rax
            rax = rcx;              //mov rax, rcx
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            r8 ^= rax;              //xor r8, rax
            rax = mb;               //lea rax, [0xFFFFFFFFF93B1768]
            r8 -= r11;              //sub r8, r11
            r8 -= rax;              //sub r8, rax
            r8 -= 0x57EC1422;               //sub r8, 0x57EC1422
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0xFFFFFFFFFFFFFF98;
            RSP_0xFFFFFFFFFFFFFF98 = mb + 0xF4C;            //lea rax, [0xFFFFFFFFF93B2827] : RBP+0xFFFFFFFFFFFFFF98                rax *= RSP_0xFFFFFFFFFFFFFF98;          //imul rax, [rbp-0x68]
            r8 += rax;              //add r8, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = ~rax;             //not rax
            r8 *= *(uintptr_t*)(rax + 0x19);                //imul r8, [rax+0x19]
            rax = mb + 0x1793;              //lea rax, [0xFFFFFFFFF93B2D7C]
            rax -= r11;             //sub rax, r11
            r8 ^= rax;              //xor r8, rax
            return r8;
        }
        }
    }
    uint64_t get_bone_ptr()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rdx = *(uintptr_t*)(mb + 0xD42BC80);
        if (!rdx)
            return rdx;
        r11 = Peb;              //mov r11, gs:[rax]
        rax = r11;              //mov rax, r11
        rax = _rotr64(rax, 0x15);               //ror rax, 0x15
        rax &= 0xF;
        switch (rax) {
        case 0:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x0000000007102E4D]
            r15 = mb + 0x629DAB46;          //lea r15, [0x0000000060021790]
            r13 = mb + 0x9895;              //lea r13, [0xFFFFFFFFFD6504D3]
            rax = 0xAC145E023332D189;               //mov rax, 0xAC145E023332D189
            rdx ^= rax;             //xor rdx, rax
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            rdx += rax;             //add rdx, rax
            rax = 0xFDEBD2F07B05670D;               //mov rax, 0xFDEBD2F07B05670D
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3;            //shr rax, 0x03
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x6;            //shr rax, 0x06
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x18;           //shr rax, 0x18
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x30;           //shr rax, 0x30
            rdx ^= rax;             //xor rdx, rax
            rax = 0xF0805972B46E082;                //mov rax, 0xF0805972B46E082
            rdx -= rax;             //sub rdx, rax
            rax = r11;              //mov rax, r11
            rax ^= r13;             //xor rax, r13
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x4;            //shr rax, 0x04
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x8;            //shr rax, 0x08
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x10;           //shr rax, 0x10
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 1:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x00000000071027CD]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x3ECBF33498144A56;               //mov rax, 0x3ECBF33498144A56
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xA;            //shr rax, 0x0A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x14;           //shr rax, 0x14
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x28;           //shr rax, 0x28
            rdx ^= rax;             //xor rdx, rax
            rax = 0x87F19886B363B05B;               //mov rax, 0x87F19886B363B05B
            rdx *= rax;             //imul rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0x6303659E1F345AFF;               //mov rax, 0x6303659E1F345AFF
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x15;           //shr rax, 0x15
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2A;           //shr rax, 0x2A
            rdx ^= rax;             //xor rdx, rax
            rdx += r11;             //add rdx, r11
            return rdx;
        }
        case 2:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x000000000710234D]
            r13 = mb + 0x6F7AC17A;          //lea r13, [0x000000006CDF22B8]
            rdx += r11;             //add rdx, r11
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rax = mb + 0x62BA;              //lea rax, [0xFFFFFFFFFD64C101]
            rax -= r11;             //sub rax, r11
            rdx += rax;             //add rdx, rax
            rax = 0x6367F6E201B667AF;               //mov rax, 0x6367F6E201B667AF
            rdx *= rax;             //imul rdx, rax
            rax = 0x7EA109C91958478C;               //mov rax, 0x7EA109C91958478C
            rdx -= rax;             //sub rdx, rax
            rdx ^= r11;             //xor rdx, r11
            rdx ^= r13;             //xor rdx, r13
            rax = 0x79658B29969CD86A;               //mov rax, 0x79658B29969CD86A
            rdx -= rax;             //sub rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 3:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x0000000007101E9D]
            r14 = mb + 0xF8CE;              //lea r14, [0xFFFFFFFFFD65555C]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            rax = 0xEA0A19EF431520D;                //mov rax, 0xEA0A19EF431520D
            rdx ^= rax;             //xor rdx, rax
            rax = 0xFFFFFFFF93B5ED93;               //mov rax, 0xFFFFFFFF93B5ED93
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = r11;              //mov rax, r11
            rax *= r14;             //imul rax, r14
            rdx -= rax;             //sub rdx, rax
            rax = 0x39F863E9187B3F65;               //mov rax, 0x39F863E9187B3F65
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1F;           //shr rax, 0x1F
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3E;           //shr rax, 0x3E
            rdx ^= rax;             //xor rdx, rax
            rax = 0x44AFB2020B72DD38;               //mov rax, 0x44AFB2020B72DD38
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 4:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x0000000007101982]
            r15 = mb + 0xD76E;              //lea r15, [0xFFFFFFFFFD652EE1]
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            rdx -= rax;             //sub rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = 0x2690031C441C94ED;               //mov rax, 0x2690031C441C94ED
            rdx *= rax;             //imul rdx, rax
            rdx ^= r11;             //xor rdx, r11
            rax = 0xA3A6498F1C56BC17;               //mov rax, 0xA3A6498F1C56BC17
            rdx ^= rax;             //xor rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 5:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x0000000007101535]
            rax = mb;               //lea rax, [0xFFFFFFFFFD64516C]
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x19;           //shr rax, 0x19
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x32;           //shr rax, 0x32
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rcx = *(uintptr_t*)(rax + 0x11);                //mov rcx, [rax+0x11]
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0x48;
            RSP_0x48 = mb + 0x598F00A5;             //lea rax, [0x0000000056F35384] : RSP+0x48
            rax *= RSP_0x48;                //imul rax, [rsp+0x48]
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = 0xC6D870371839E04D;               //mov rax, 0xC6D870371839E04D
            rdx *= rax;             //imul rdx, rax
            rax = 0x2435BC22D4E2922B;               //mov rax, 0x2435BC22D4E2922B
            rdx -= rax;             //sub rdx, rax
            rdx *= rcx;             //imul rdx, rcx
            rax = 0xBBD9DF3CECEEFE74;               //mov rax, 0xBBD9DF3CECEEFE74
            rdx ^= rax;             //xor rdx, rax
            rax = 0x23B4F504FA125955;               //mov rax, 0x23B4F504FA125955
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 6:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000071010F9]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rax += 0xFFFFFFFFA7D1474C;              //add rax, 0xFFFFFFFFA7D1474C
            rdx += rax;             //add rdx, rax
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rax = 0xFFFFFFFFC0CD4EE3;               //mov rax, 0xFFFFFFFFC0CD4EE3
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x23;           //shr rax, 0x23
            rax ^= rdx;             //xor rax, rdx
            rdx = 0x5A8397EF69EB3410;               //mov rdx, 0x5A8397EF69EB3410
            rax += r11;             //add rax, r11
            rax += rdx;             //add rax, rdx
            rdx = mb;               //lea rdx, [0xFFFFFFFFFD644C06]
            rdx += rax;             //add rdx, rax
            rax = 0x94B908816CF2DBE1;               //mov rax, 0x94B908816CF2DBE1
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 7:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x0000000007100CE8]
            r15 = mb + 0x6B60;              //lea r15, [0xFFFFFFFFFD64B639]
            rax = mb;               //lea rax, [0xFFFFFFFFFD6447EF]
            rax += 0xC77B;          //add rax, 0xC77B
            rax += r11;             //add rax, r11
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x9;            //shr rax, 0x09
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx += rax;             //add rdx, rax
            rax = 0x3BAB7EE1C2FB5485;               //mov rax, 0x3BAB7EE1C2FB5485
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rdx += rax;             //add rdx, rax
            rax = 0xD64310FF7669DED5;               //mov rax, 0xD64310FF7669DED5
            rdx *= rax;             //imul rdx, rax
            rax = 0xC9A0080E2B52320A;               //mov rax, 0xC9A0080E2B52320A
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 8:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x0000000007100715]
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rax += 0xFFFFFFFF954B94E9;              //add rax, 0xFFFFFFFF954B94E9
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0xE;            //shr rax, 0x0E
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1C;           //shr rax, 0x1C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x38;           //shr rax, 0x38
            rdx ^= rax;             //xor rdx, rax
            rax = 0x28853EAC80AAB90;                //mov rax, 0x28853EAC80AAB90
            rdx -= rax;             //sub rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xB;            //shr rax, 0x0B
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x16;           //shr rax, 0x16
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2C;           //shr rax, 0x2C
            rdx ^= rax;             //xor rdx, rax
            rax = 0x9ED615C5A516F48D;               //mov rax, 0x9ED615C5A516F48D
            rdx *= rax;             //imul rdx, rax
            rax = 0x4A5451CFD1051B0F;               //mov rax, 0x4A5451CFD1051B0F
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 9:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000071001FC]
            r13 = mb + 0x9F7F;              //lea r13, [0xFFFFFFFFFD64DF6C]
            rcx = mb + 0x590B7B0F;          //lea rcx, [0x00000000566FBA87]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x25;           //shr rax, 0x25
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0xE41AAE0B4978C7A7;               //mov rax, 0xE41AAE0B4978C7A7
            rdx *= rax;             //imul rdx, rax
            rax = 0x4884ED1EDA36D9B2;               //mov rax, 0x4884ED1EDA36D9B2
            rdx -= rax;             //sub rdx, rax
            rax = r11;              //mov rax, r11
            rax ^= r13;             //xor rax, r13
            rdx ^= rax;             //xor rdx, rax
            rax = 0xA5F46429036B04E5;               //mov rax, 0xA5F46429036B04E5
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x27;           //shr rax, 0x27
            rdx ^= rax;             //xor rdx, rax
            rdx -= r11;             //sub rdx, r11
            rdx += rcx;             //add rdx, rcx
            return rdx;
        }
        case 10:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x00000000070FFD6E]
            rdx -= mb;              //sub rdx, [rsp+0xA0] -- didn't find trace -> use base
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rax += 0xFFFFFFFFFFFF88EC;              //add rax, 0xFFFFFFFFFFFF88EC
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = 0x4DBC160E13E56349;               //mov rax, 0x4DBC160E13E56349
            rdx *= rax;             //imul rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD643A44]
            rdx ^= rax;             //xor rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD643916]
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        case 11:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000070FF8C8]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = mb + 0x73A4FAE9;          //lea rax, [0x0000000071092EED]
            rax = ~rax;             //not rax
            rdx -= r11;             //sub rdx, r11
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x16;           //shr rax, 0x16
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2C;           //shr rax, 0x2C
            rdx ^= rax;             //xor rdx, rax
            rax = 0x861DF3431C84C629;               //mov rax, 0x861DF3431C84C629
            rdx *= rax;             //imul rdx, rax
            rax = 0x714B44E8CE73C4F0;               //mov rax, 0x714B44E8CE73C4F0
            rdx -= rax;             //sub rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD643368]
            rcx = rax * 0xFFFFFFFFFFFFFFFE;                 //imul rcx, rax, 0xFFFFFFFFFFFFFFFE
            rax = 0x6F9175143B9ED737;               //mov rax, 0x6F9175143B9ED737
            rdx += rax;             //add rdx, rax
            rdx += rcx;             //add rdx, rcx
            return rdx;
        }
        case 12:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000070FF509]
            r15 = mb + 0xD3DA;              //lea r15, [0xFFFFFFFFFD6506D4]
            rcx = r11;              //mov rcx, r11
            rcx = ~rcx;             //not rcx
            rax = mb + 0x1F86111B;          //lea rax, [0x000000001CEA40D8]
            rax = ~rax;             //not rax
            rcx *= rax;             //imul rcx, rax
            rax = r15;              //mov rax, r15
            rax -= r11;             //sub rax, r11
            rax += rdx;             //add rax, rdx
            rdx = rcx;              //mov rdx, rcx
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rcx = mb;               //lea rcx, [0xFFFFFFFFFD64316D]
            rax = r11;              //mov rax, r11
            rax -= rcx;             //sub rax, rcx
            rcx = rax + 0xffffffffca7be9d9;                 //lea rcx, [rax-0x35841627]
            rcx += rdx;             //add rcx, rdx
            rax = rcx;              //mov rax, rcx
            rax >>= 0x18;           //shr rax, 0x18
            rcx ^= rax;             //xor rcx, rax
            rax = 0xFBA7ABC8BBB4629D;               //mov rax, 0xFBA7ABC8BBB4629D
            rdx = rcx;              //mov rdx, rcx
            rdx >>= 0x30;           //shr rdx, 0x30
            rdx ^= rcx;             //xor rdx, rcx
            rdx *= rax;             //imul rdx, rax
            rax = 0x1FE6307AA1F54B4D;               //mov rax, 0x1FE6307AA1F54B4D
            rdx *= rax;             //imul rdx, rax
            rax = 0x57A7A919AF723E1B;               //mov rax, 0x57A7A919AF723E1B
            rdx -= rax;             //sub rdx, rax
            return rdx;
        }
        case 13:
        {
            r9 = *(uintptr_t*)(mb + 0x9ABC1FC);             //mov r9, [0x00000000070FF10F]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1A;           //shr rax, 0x1A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x34;           //shr rax, 0x34
            rdx ^= rax;             //xor rdx, rax
            rax = 0x525F068BC2643DF7;               //mov rax, 0x525F068BC2643DF7
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xD;            //shr rax, 0x0D
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1A;           //shr rax, 0x1A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x34;           //shr rax, 0x34
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x75DFF140FA1FB5BC;               //mov rax, 0x75DFF140FA1FB5BC
            rdx += rax;             //add rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD642B46]
            rdx += rax;             //add rdx, rax
            rax = 0xE5945E699002C625;               //mov rax, 0xE5945E699002C625
            rdx *= rax;             //imul rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD642B2E]
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 14:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000070FECAA]
            r15 = mb + 0x73A4A654;          //lea r15, [0x000000007108D0EF]
            rax = 0x75736E13202430E1;               //mov rax, 0x75736E13202430E1
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x9;            //shr rax, 0x09
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rdx += r11;             //add rdx, r11
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            rax *= r15;             //imul rax, r15
            rdx ^= rax;             //xor rdx, rax
            rax = 0xABD8E138F25E5687;               //mov rax, 0xABD8E138F25E5687
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 15:
        {
            r10 = *(uintptr_t*)(mb + 0x9ABC1FC);            //mov r10, [0x00000000070FE8F9]
            rsi = 0xB656FAE057EB613B;               //mov rsi, 0xB656FAE057EB613B
            rdx *= rsi;             //imul rdx, rsi
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x5CA0A4447C245D90;               //mov rax, 0x5CA0A4447C245D90
            rdx -= rax;             //sub rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0xF071D0312866EB9D;               //mov rax, 0xF071D0312866EB9D
            rdx *= rax;             //imul rdx, rax
            rax = 0xFFFFFFFFFFFFF34A;               //mov rax, 0xFFFFFFFFFFFFF34A
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        }
    }
    uint64_t get_bone_index(uint32_t bone_index)
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rdi = bone_index;
        rcx = rdi * 0x13C8;
        rax = 0x2AC83BD095F98207;               //mov rax, 0x2AC83BD095F98207
        rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
        rax = rcx;              //mov rax, rcx
        r11 = mb;               //lea r11, [0xFFFFFFFFFD13EA32]
        rax -= rdx;             //sub rax, rdx
        r10 = 0xD1F23647CF68B0F;                //mov r10, 0xD1F23647CF68B0F
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0xC;            //shr rax, 0x0C
        rax = rax * 0x1B6B;             //imul rax, rax, 0x1B6B
        rcx -= rax;             //sub rcx, rax
        rax = 0x6768ECB62DD904F3;               //mov rax, 0x6768ECB62DD904F3
        r8 = rcx * 0x1B6B;              //imul r8, rcx, 0x1B6B
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rdx >>= 0xC;            //shr rdx, 0x0C
        rax = rdx * 0x279C;             //imul rax, rdx, 0x279C
        r8 -= rax;              //sub r8, rax
        rax = 0x8888888888888889;               //mov rax, 0x8888888888888889
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = 0x1778A191BD684181;               //mov rax, 0x1778A191BD684181
        rdx >>= 0x4;            //shr rdx, 0x04
        rcx = rdx * 0x1E;               //imul rcx, rdx, 0x1E
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r8;               //mov rax, r8
        rax -= rdx;             //sub rax, rdx
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0x8;            //shr rax, 0x08
        rcx += rax;             //add rcx, rax
        rax = rcx * 0x3AA;              //imul rax, rcx, 0x3AA
        rcx = r8 * 0x3AC;               //imul rcx, r8, 0x3AC
        rcx -= rax;             //sub rcx, rax
        rax = *(uint16_t*)(rcx + r11 * 1 + 0x9B96580);          //movzx eax, word ptr [rcx+r11*1+0x9B96580]
        r8 = rax * 0x13C8;              //imul r8, rax, 0x13C8
        rax = r10;              //mov rax, r10
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r10;              //mov rax, r10
        rdx >>= 0x9;            //shr rdx, 0x09
        rcx = rdx * 0x2705;             //imul rcx, rdx, 0x2705
        r8 -= rcx;              //sub r8, rcx
        r9 = r8 * 0x49D8;               //imul r9, r8, 0x49D8
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rdx >>= 0x9;            //shr rdx, 0x09
        rax = rdx * 0x2705;             //imul rax, rdx, 0x2705
        r9 -= rax;              //sub r9, rax
        rax = 0xA10BFDF49906A50F;               //mov rax, 0xA10BFDF49906A50F
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rcx = r9;               //mov rcx, r9
        r9 &= 0x1;             //and r9d, 0x01
        rdx >>= 0xC;            //shr rdx, 0x0C
        rax = rdx * 0x196F;             //imul rax, rdx, 0x196F
        rcx -= rax;             //sub rcx, rax
        rax = r9 + rcx * 2;             //lea rax, [r9+rcx*2]
        r12 = *(uint16_t*)(r11 + rax * 2 + 0x9B9D370);          //movsx r12d, word ptr [r11+rax*2+0x9B9D370]
        return r12;
    }
}

namespace steam
{
    uintptr_t Get_cg_t()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rbx = *(uintptr_t*)(mb + 0x1208D198);
        if (!rbx)
            return rbx;
        rdx = Peb;              //mov rdx, gs:[rax]
        //r9 = 00000088C270F2A0//failed to trace. base: 00007FF625E60000 It's possibly wrong
        rax = rbx;              //mov rax, rbx
        rax >>= 0xB;            //shr rax, 0x0B
        r8 = 0x1E53D8998E54A23D;                //mov r8, 0x1E53D8998E54A23D
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x16;           //shr rax, 0x16
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x2C;           //shr rax, 0x2C
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rbx ^= rax;             //xor rbx, rax
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= *(uintptr_t*)(mb + 0x973F0DD);           //xor rcx, [0x0000000007030A44]
        rax = mb + 0x266259AD;          //lea rax, [0x0000000023F1730D]
        rbx *= r8;              //imul rbx, r8
        rax = ~rax;             //not rax
        rbx -= r9;              //sub rbx, r9
        rax -= rdx;             //sub rax, rdx
        rcx = _byteswap_uint64(rcx);            //bswap rcx
        rbx *= *(uintptr_t*)(rcx + 0xb);                //imul rbx, [rcx+0x0B]
        rbx ^= rax;             //xor rbx, rax
        rax = 0xC338B9C132E5301E;               //mov rax, 0xC338B9C132E5301E
        rbx ^= rax;             //xor rbx, rax
        return rbx;
    }
    uint64_t get_client_info_base()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        r10 = *(uintptr_t*)(Get_cg_t() + 0x162b80);
        if (!r10)
            return r10;
        r11 = ~Peb;              //mov r11, gs:[rax]
        rax = r11;              //mov rax, r11
        rax = _rotl64(rax, 0x21);               //rol rax, 0x21
        rax &= 0xF;
        switch (rax) {
        case 0:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FDB8E9]
            rax = 0x9141C45BFD5B39F7;               //mov rax, 0x9141C45BFD5B39F7
            r10 ^= rax;             //xor r10, rax
            rcx = mb;               //lea rcx, [0xFFFFFFFFFD89C3C3]
            rcx += r10;             //add rcx, r10
            r10 = mb + 0x6E33AF72;          //lea r10, [0x000000006BBD731C]
            rax = 0xF605A67470E7C53D;               //mov rax, 0xF605A67470E7C53D
            rcx *= rax;             //imul rcx, rax
            rax = r11;              //mov rax, r11
            rax ^= r10;             //xor rax, r10
            rcx -= rax;             //sub rcx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD89C37C]
            rcx -= rax;             //sub rcx, rax
            r10 = rcx + 0xffffffffd10685d8;                 //lea r10, [rcx-0x2EF97A28]
            r10 += r11;             //add r10, r11
            rax = mb + 0x526CB4F4;          //lea rax, [0x000000004FF67BFB]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r10 += rax;             //add r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1A;           //shr rax, 0x1A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x34;           //shr rax, 0x34
            r10 ^= rax;             //xor r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            return r10;
        }
        case 1:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FDB448]
            rax = r10;              //mov rax, r10
            rax >>= 0x24;           //shr rax, 0x24
            r10 ^= rax;             //xor r10, rax
            rax = 0xE570A6F93EC9464F;               //mov rax, 0xE570A6F93EC9464F
            r10 *= rax;             //imul r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = r10;              //mov rax, r10
            rax >>= 0x24;           //shr rax, 0x24
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x11;           //shr rax, 0x11
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x22;           //shr rax, 0x22
            r10 ^= rax;             //xor r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD89C190]
            r10 -= rax;             //sub r10, rax
            r10 += r11;             //add r10, r11
            rax = 0x14F095F380F9EB43;               //mov rax, 0x14F095F380F9EB43
            r10 += rax;             //add r10, rax
            return r10;
        }
        case 2:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FDAF91]
            rax = 0x8ADB88DACDCF2087;               //mov rax, 0x8ADB88DACDCF2087
            r10 *= rax;             //imul r10, rax
            rax = 0x7962CBE13BD24CEA;               //mov rax, 0x7962CBE13BD24CEA
            r10 += rax;             //add r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x10;           //shr rax, 0x10
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x20;           //shr rax, 0x20
            r10 ^= rax;             //xor r10, rax
            r10 += r11;             //add r10, r11
            r10 += r11;             //add r10, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            r10 -= r11;             //sub r10, r11
            rax = r10;              //mov rax, r10
            rax >>= 0x1D;           //shr rax, 0x1D
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x3A;           //shr rax, 0x3A
            r10 ^= rax;             //xor r10, rax
            return r10;
        }
        case 3:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FDABBC]
            rax = r10;              //mov rax, r10
            rax >>= 0x25;           //shr rax, 0x25
            rax ^= r10;             //xor rax, r10
            r10 = 0x99B04B837FD2242B;               //mov r10, 0x99B04B837FD2242B
            rax += r10;             //add rax, r10
            r10 = mb;               //lea r10, [0xFFFFFFFFFD89B9CC]
            r10 += rax;             //add r10, rax
            r10 ^= r11;             //xor r10, r11
            rax = mb;               //lea rax, [0xFFFFFFFFFD89B7E5]
            r10 -= rax;             //sub r10, rax
            rax = mb + 0x11751E8B;          //lea rax, [0x000000000EFED59E]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r10 += rax;             //add r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = 0xDA47FAB853EFDBF7;               //mov rax, 0xDA47FAB853EFDBF7
            r10 *= rax;             //imul r10, rax
            return r10;
        }
        case 4:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FDA704]
            rax = r10;              //mov rax, r10
            rax >>= 0x2;            //shr rax, 0x02
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x4;            //shr rax, 0x04
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x8;            //shr rax, 0x08
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x10;           //shr rax, 0x10
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x20;           //shr rax, 0x20
            r10 ^= rax;             //xor r10, rax
            rax = 0x718CFE6D52D76081;               //mov rax, 0x718CFE6D52D76081
            r10 *= rax;             //imul r10, rax
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0xFFFFFFFFFFFFFF90;
            RSP_0xFFFFFFFFFFFFFF90 = mb + 0x746D9936;               //lea rax, [0x0000000071F74F01] : RBP+0xFFFFFFFFFFFFFF90
            rax ^= RSP_0xFFFFFFFFFFFFFF90;          //xor rax, [rbp-0x70]
            r10 ^= rax;             //xor r10, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rax = mb;               //lea rax, [0xFFFFFFFFFD89B363]
            rcx ^= r9;              //xor rcx, r9
            rcx = ~rcx;             //not rcx
            r10 *= *(uintptr_t*)(rcx + 0x19);               //imul r10, [rcx+0x19]
            rcx = 0x6A0BA8494B6820F5;               //mov rcx, 0x6A0BA8494B6820F5
            r10 += rcx;             //add r10, rcx
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x15;           //shr rax, 0x15
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x2A;           //shr rax, 0x2A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1C;           //shr rax, 0x1C
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x38;           //shr rax, 0x38
            r10 ^= rax;             //xor r10, rax
            return r10;
        }
        case 5:
        {
            r8 = *(uintptr_t*)(mb + 0x973F10E);             //mov r8, [0x0000000006FDA185]
            rax = r10;              //mov rax, r10
            rax >>= 0x15;           //shr rax, 0x15
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x2A;           //shr rax, 0x2A
            r10 ^= rax;             //xor r10, rax
            rax = 0x6B4D10E3FCFC0235;               //mov rax, 0x6B4D10E3FCFC0235
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x3;            //shr rax, 0x03
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x6;            //shr rax, 0x06
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0xC;            //shr rax, 0x0C
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x18;           //shr rax, 0x18
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x30;           //shr rax, 0x30
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1A;           //shr rax, 0x1A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x34;           //shr rax, 0x34
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1C;           //shr rax, 0x1C
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x38;           //shr rax, 0x38
            r10 ^= rax;             //xor r10, rax
            uintptr_t RSP_0x48;
            RSP_0x48 = 0x9FE7D7D2C91086EF;          //mov rax, 0x9FE7D7D2C91086EF : RSP+0x48
            r10 *= RSP_0x48;                //imul r10, [rsp+0x48]
            uintptr_t RSP_0x78;
            RSP_0x78 = 0x3B044E06AA0DC65D;          //mov rax, 0x3B044E06AA0DC65D : RSP+0x78
            r10 += RSP_0x78;                //add r10, [rsp+0x78]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r8;              //xor rax, r8
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            return r10;
        }
        case 6:
        {
            r8 = *(uintptr_t*)(mb + 0x973F10E);             //mov r8, [0x0000000006FD9C2C]
            rax = r10;              //mov rax, r10
            rax >>= 0xF;            //shr rax, 0x0F
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1E;           //shr rax, 0x1E
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x3C;           //shr rax, 0x3C
            r10 ^= rax;             //xor r10, rax
            r10 -= r11;             //sub r10, r11
            r10 ^= r11;             //xor r10, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r8;              //xor rax, r8
            rax = ~rax;             //not rax
            rax = *(uintptr_t*)(rax + 0x19);                //mov rax, [rax+0x19]
            uintptr_t RSP_0x58;
            RSP_0x58 = 0xB64C05FA8BB41ED5;          //mov rax, 0xB64C05FA8BB41ED5 : RSP+0x58
            rax *= RSP_0x58;                //imul rax, [rsp+0x58]
            r10 *= rax;             //imul r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD89AA46]
            r10 -= rax;             //sub r10, rax
            rax = 0xA1839DE961442277;               //mov rax, 0xA1839DE961442277
            r10 *= rax;             //imul r10, rax
            return r10;
        }
        case 7:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD97AA]
            rax = mb + 0x7E9B;              //lea rax, [0xFFFFFFFFFD8A2133]
            rcx = r11;              //mov rcx, r11
            r10 += rax;             //add r10, rax
            rcx = ~rcx;             //not rcx
            r10 += rcx;             //add r10, rcx
            rax = r10;              //mov rax, r10
            rax >>= 0xD;            //shr rax, 0x0D
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1A;           //shr rax, 0x1A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x34;           //shr rax, 0x34
            r10 ^= rax;             //xor r10, rax
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0xFFFFFFFFFFFFFF90;
            RSP_0xFFFFFFFFFFFFFF90 = mb + 0x2B12;           //lea rax, [0xFFFFFFFFFD89D183] : RBP+0xFFFFFFFFFFFFFF90
            rax ^= RSP_0xFFFFFFFFFFFFFF90;          //xor rax, [rbp-0x70]
            r10 -= rax;             //sub r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x27;           //shr rax, 0x27
            r10 ^= rax;             //xor r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = 0x6B0468CD6B4B36F5;               //mov rax, 0x6B0468CD6B4B36F5
            r10 *= rax;             //imul r10, rax
            rax = 0x67D32343BA582459;               //mov rax, 0x67D32343BA582459
            r10 -= rax;             //sub r10, rax
            rax = 0x2AB381DC49040AEF;               //mov rax, 0x2AB381DC49040AEF
            r10 ^= rax;             //xor r10, rax
            return r10;
        }
        case 8:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD92B0]
            rax = 0xF27764D7BCC134E1;               //mov rax, 0xF27764D7BCC134E1
            r10 *= rax;             //imul r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x7;            //shr rax, 0x07
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0xE;            //shr rax, 0x0E
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1C;           //shr rax, 0x1C
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x38;           //shr rax, 0x38
            r10 ^= rax;             //xor r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD89A0A7]
            r10 ^= rax;             //xor r10, rax
            rax = 0xC3107C6F6CB6AAB7;               //mov rax, 0xC3107C6F6CB6AAB7
            r10 *= rax;             //imul r10, rax
            rax = 0xD1B5E7C8461A7E03;               //mov rax, 0xD1B5E7C8461A7E03
            r10 ^= rax;             //xor r10, rax
            rax = mb + 0x4977696C;          //lea rax, [0x0000000047010938]
            rax = ~rax;             //not rax
            rax += r11;             //add rax, r11
            r10 += rax;             //add r10, rax
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rbp-0x58] -- didn't find trace -> use base
            rax += 0xFFFFFFFFFFFF4E09;              //add rax, 0xFFFFFFFFFFFF4E09
            r10 += rax;             //add r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            return r10;
        }
        case 9:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD8E06]
            rax = r10;              //mov rax, r10
            rax >>= 0x23;           //shr rax, 0x23
            r10 ^= rax;             //xor r10, rax
            rax = 0xA7B0F0AA378850A7;               //mov rax, 0xA7B0F0AA378850A7
            r10 *= rax;             //imul r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD899A09]
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1B;           //shr rax, 0x1B
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x36;           //shr rax, 0x36
            r10 ^= rax;             //xor r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD899963]
            rax += 0xD85;           //add rax, 0xD85
            rax += r11;             //add rax, r11
            r10 += rax;             //add r10, rax
            r10 -= mb;              //sub r10, [rbp-0x58] -- didn't find trace -> use base
            uintptr_t RSP_0x30;
            RSP_0x30 = 0x4E5E1AE762C3863A;          //mov rax, 0x4E5E1AE762C3863A : RSP+0x30
            r10 ^= RSP_0x30;                //xor r10, [rsp+0x30]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            return r10;
        }
        case 10:
        {
            r8 = *(uintptr_t*)(mb + 0x973F10E);             //mov r8, [0x0000000006FD894C]
            rax = 0x98CD10A39FEEABC3;               //mov rax, 0x98CD10A39FEEABC3
            r10 *= rax;             //imul r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD8996C5]
            rax += 0x8AF0;          //add rax, 0x8AF0
            rax += r11;             //add rax, r11
            r10 += rax;             //add r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x10;           //shr rax, 0x10
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x20;           //shr rax, 0x20
            r10 ^= rax;             //xor r10, rax
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            uintptr_t RSP_0xFFFFFFFFFFFFFF88;
            RSP_0xFFFFFFFFFFFFFF88 = mb + 0xF084;           //lea rax, [0xFFFFFFFFFD8A88BB] : RBP+0xFFFFFFFFFFFFFF88
            rax ^= RSP_0xFFFFFFFFFFFFFF88;          //xor rax, [rbp-0x78]
            r10 += rax;             //add r10, rax
            rax = 0xDF8A1660CBF5F30F;               //mov rax, 0xDF8A1660CBF5F30F
            r10 *= rax;             //imul r10, rax
            rax = 0xA829D63D19635A8D;               //mov rax, 0xA829D63D19635A8D
            r10 ^= rax;             //xor r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r8;              //xor rax, r8
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            r10 ^= r11;             //xor r10, r11
            return r10;
        }
        case 11:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD84D1]
            rcx = r10;              //mov rcx, r10
            rcx >>= 0x23;           //shr rcx, 0x23
            rcx ^= r11;             //xor rcx, r11
            rax = mb + 0x17E7D121;          //lea rax, [0x000000001571618A]
            rcx ^= rax;             //xor rcx, rax
            rax = r11 + 0x1;                //lea rax, [r11+0x01]
            rcx ^= r10;             //xor rcx, r10
            r10 = mb + 0x7D39B186;          //lea r10, [0x000000007AC341DE]
            r10 *= rax;             //imul r10, rax
            r10 += rcx;             //add r10, rcx
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = mb;               //lea rax, [0xFFFFFFFFFD8991A5]
            rax += 0x8CB1;          //add rax, 0x8CB1
            rax += r11;             //add rax, r11
            r10 ^= rax;             //xor r10, rax
            r10 ^= r11;             //xor r10, r11
            rax = 0x71B5B118240CFD7D;               //mov rax, 0x71B5B118240CFD7D
            r10 *= rax;             //imul r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD899210]
            rax += 0x2611654C;              //add rax, 0x2611654C
            rax += r11;             //add rax, r11
            r10 += rax;             //add r10, rax
            return r10;
        }
        case 12:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD80DE]
            rax = mb;               //lea rax, [0xFFFFFFFFFD898D05]
            r10 -= rax;             //sub r10, rax
            rax = mb + 0x4291859E;          //lea rax, [0x00000000401B13C7]
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x10;           //shr rax, 0x10
            r10 ^= rax;             //xor r10, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rax = r10;              //mov rax, r10
            rcx = ~rcx;             //not rcx
            rax >>= 0x20;           //shr rax, 0x20
            r10 ^= rax;             //xor r10, rax
            rax = 0x6A01EB295C695943;               //mov rax, 0x6A01EB295C695943
            r10 *= rax;             //imul r10, rax
            rax = 0x9BBE6575DCB15C28;               //mov rax, 0x9BBE6575DCB15C28
            r10 ^= rax;             //xor r10, rax
            r10 *= *(uintptr_t*)(rcx + 0x19);               //imul r10, [rcx+0x19]
            rax = 0x9A76A1C3B04C8361;               //mov rax, 0x9A76A1C3B04C8361
            r10 *= rax;             //imul r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD898E5A]
            r10 -= rax;             //sub r10, rax
            return r10;
        }
        case 13:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD7C3C]
            rax = mb;               //lea rax, [0xFFFFFFFFFD89863E]
            r10 ^= rax;             //xor r10, rax
            rax = 0x424E7D3CE7A4BDA3;               //mov rax, 0x424E7D3CE7A4BDA3
            r10 *= rax;             //imul r10, rax
            rax = mb + 0x1A767856;          //lea rax, [0x0000000017FFFF49]
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            r10 += rax;             //add r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0xF;            //shr rax, 0x0F
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1E;           //shr rax, 0x1E
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x3C;           //shr rax, 0x3C
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1;            //shr rax, 0x01
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x2;            //shr rax, 0x02
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x4;            //shr rax, 0x04
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x8;            //shr rax, 0x08
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x10;           //shr rax, 0x10
            r10 ^= rax;             //xor r10, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rax = r10;              //mov rax, r10
            rcx ^= r9;              //xor rcx, r9
            rax >>= 0x20;           //shr rax, 0x20
            rcx = ~rcx;             //not rcx
            r10 ^= rax;             //xor r10, rax
            r10 *= *(uintptr_t*)(rcx + 0x19);               //imul r10, [rcx+0x19]
            uintptr_t RSP_0x58;
            RSP_0x58 = 0x8DB30096C278A251;          //mov rax, 0x8DB30096C278A251 : RSP+0x58
            r10 *= RSP_0x58;                //imul r10, [rsp+0x58]
            rax = mb;               //lea rax, [0xFFFFFFFFFD8988AB]
            rax += 0x31EB9108;              //add rax, 0x31EB9108
            rax += r11;             //add rax, r11
            r10 ^= rax;             //xor r10, rax
            return r10;
        }
        case 14:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD7723]
            rcx = mb;               //lea rcx, [0xFFFFFFFFFD898240]
            rcx += 0xA832;          //add rcx, 0xA832
            rcx += r11;             //add rcx, r11
            rax = mb;               //lea rax, [0xFFFFFFFFFD89822C]
            rcx ^= r10;             //xor rcx, r10
            r10 = r11;              //mov r10, r11
            r10 = ~r10;             //not r10
            r10 += rcx;             //add r10, rcx
            r10 -= rax;             //sub r10, rax
            r10 -= 0x2FECE2F9;              //sub r10, 0x2FECE2F9
            rax = mb;               //lea rax, [0xFFFFFFFFFD89826F]
            rax += 0x4948;          //add rax, 0x4948
            rax += r11;             //add rax, r11
            r10 += rax;             //add r10, rax
            rax = 0xE986304E17E64F7D;               //mov rax, 0xE986304E17E64F7D
            r10 *= rax;             //imul r10, rax
            rax = mb + 0x5BB7;              //lea rax, [0xFFFFFFFFFD89E03B]
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x1A;           //shr rax, 0x1A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x34;           //shr rax, 0x34
            r10 ^= rax;             //xor r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            uintptr_t RSP_0x30;
            RSP_0x30 = mb + 0x30A5920C;             //lea rax, [0x000000002E2F17F6] : RSP+0x30
            rax ^= RSP_0x30;                //xor rax, [rsp+0x30]
            r10 += rax;             //add r10, rax
            return r10;
        }
        case 15:
        {
            r9 = *(uintptr_t*)(mb + 0x973F10E);             //mov r9, [0x0000000006FD71CD]
            rcx = mb + 0x56C36699;          //lea rcx, [0x00000000544CE699]
            rax = 0x14288A7031FA1D2A;               //mov rax, 0x14288A7031FA1D2A
            r10 += rax;             //add r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0xA;            //shr rax, 0x0A
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x14;           //shr rax, 0x14
            r10 ^= rax;             //xor r10, rax
            rax = r10;              //mov rax, r10
            rax >>= 0x28;           //shr rax, 0x28
            r10 ^= rax;             //xor r10, rax
            rax = 0xF471F0FE111CB275;               //mov rax, 0xF471F0FE111CB275
            r10 *= rax;             //imul r10, rax
            rax = rcx;              //mov rax, rcx
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            r10 ^= rax;             //xor r10, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD897EF8]
            r10 -= r11;             //sub r10, r11
            r10 -= rax;             //sub r10, rax
            r10 -= 0x57EC1422;              //sub r10, 0x57EC1422
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0x48;
            RSP_0x48 = mb + 0xF4C;          //lea rax, [0xFFFFFFFFFD898FC4] : RSP+0x48
            rax *= RSP_0x48;                //imul rax, [rsp+0x48]
            r10 += rax;             //add r10, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = ~rax;             //not rax
            r10 *= *(uintptr_t*)(rax + 0x19);               //imul r10, [rax+0x19]
            rax = mb + 0x1793;              //lea rax, [0xFFFFFFFFFD899507]
            rax -= r11;             //sub rax, r11
            r10 ^= rax;             //xor r10, rax
            return r10;
        }
        }
    }
    uint64_t get_bone_ptr()
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rdx = *(uintptr_t*)(mb + 0xD1E0658);
        if (!rdx)
            return rdx;
        r11 = Peb;              //mov r11, gs:[rax]
        rax = r11;              //mov rax, r11
        rax = _rotr64(rax, 0x15);               //ror rax, 0x15
        rax &= 0xF;
        switch (rax) {
        case 0:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D9A83D]
            r15 = mb + 0x629DAB46;          //lea r15, [0x0000000060036180]
            r13 = mb + 0x9895;              //lea r13, [0xFFFFFFFFFD664EC3]
            rax = 0xAC145E023332D189;               //mov rax, 0xAC145E023332D189
            rdx ^= rax;             //xor rdx, rax
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            rdx += rax;             //add rdx, rax
            rax = 0xFDEBD2F07B05670D;               //mov rax, 0xFDEBD2F07B05670D
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3;            //shr rax, 0x03
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x6;            //shr rax, 0x06
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x18;           //shr rax, 0x18
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x30;           //shr rax, 0x30
            rdx ^= rax;             //xor rdx, rax
            rax = 0xF0805972B46E082;                //mov rax, 0xF0805972B46E082
            rdx -= rax;             //sub rdx, rax
            rax = r11;              //mov rax, r11
            rax ^= r13;             //xor rax, r13
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x4;            //shr rax, 0x04
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x8;            //shr rax, 0x08
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x10;           //shr rax, 0x10
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 1:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D9A1BD]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x3ECBF33498144A56;               //mov rax, 0x3ECBF33498144A56
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xA;            //shr rax, 0x0A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x14;           //shr rax, 0x14
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x28;           //shr rax, 0x28
            rdx ^= rax;             //xor rdx, rax
            rax = 0x87F19886B363B05B;               //mov rax, 0x87F19886B363B05B
            rdx *= rax;             //imul rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0x6303659E1F345AFF;               //mov rax, 0x6303659E1F345AFF
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x15;           //shr rax, 0x15
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2A;           //shr rax, 0x2A
            rdx ^= rax;             //xor rdx, rax
            rdx += r11;             //add rdx, r11
            return rdx;
        }
        case 2:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D99D3D]
            r13 = mb + 0x6F7AC17A;          //lea r13, [0x000000006CE06CA8]
            rdx += r11;             //add rdx, r11
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rax = mb + 0x62BA;              //lea rax, [0xFFFFFFFFFD660AF1]
            rax -= r11;             //sub rax, r11
            rdx += rax;             //add rdx, rax
            rax = 0x6367F6E201B667AF;               //mov rax, 0x6367F6E201B667AF
            rdx *= rax;             //imul rdx, rax
            rax = 0x7EA109C91958478C;               //mov rax, 0x7EA109C91958478C
            rdx -= rax;             //sub rdx, rax
            rdx ^= r11;             //xor rdx, r11
            rdx ^= r13;             //xor rdx, r13
            rax = 0x79658B29969CD86A;               //mov rax, 0x79658B29969CD86A
            rdx -= rax;             //sub rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 3:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D9988D]
            r14 = mb + 0xF8CE;              //lea r14, [0xFFFFFFFFFD669F4C]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            rax = 0xEA0A19EF431520D;                //mov rax, 0xEA0A19EF431520D
            rdx ^= rax;             //xor rdx, rax
            rax = 0xFFFFFFFF93B5ED93;               //mov rax, 0xFFFFFFFF93B5ED93
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = r11;              //mov rax, r11
            rax *= r14;             //imul rax, r14
            rdx -= rax;             //sub rdx, rax
            rax = 0x39F863E9187B3F65;               //mov rax, 0x39F863E9187B3F65
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1F;           //shr rax, 0x1F
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3E;           //shr rax, 0x3E
            rdx ^= rax;             //xor rdx, rax
            rax = 0x44AFB2020B72DD38;               //mov rax, 0x44AFB2020B72DD38
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 4:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D99372]
            r15 = mb + 0xD76E;              //lea r15, [0xFFFFFFFFFD6678D1]
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax ^= r11;             //xor rax, r11
            rdx -= rax;             //sub rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = 0x2690031C441C94ED;               //mov rax, 0x2690031C441C94ED
            rdx *= rax;             //imul rdx, rax
            rdx ^= r11;             //xor rdx, r11
            rax = 0xA3A6498F1C56BC17;               //mov rax, 0xA3A6498F1C56BC17
            rdx ^= rax;             //xor rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            return rdx;
        }
        case 5:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D98F25]
            rax = mb;               //lea rax, [0xFFFFFFFFFD659B5C]
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x19;           //shr rax, 0x19
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x32;           //shr rax, 0x32
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rcx = *(uintptr_t*)(rax + 0x11);                //mov rcx, [rax+0x11]
            rax = r11;              //mov rax, r11
            uintptr_t RSP_0x48;
            RSP_0x48 = mb + 0x598F00A5;             //lea rax, [0x0000000056F49D74] : RSP+0x48
            rax *= RSP_0x48;                //imul rax, [rsp+0x48]
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = 0xC6D870371839E04D;               //mov rax, 0xC6D870371839E04D
            rdx *= rax;             //imul rdx, rax
            rax = 0x2435BC22D4E2922B;               //mov rax, 0x2435BC22D4E2922B
            rdx -= rax;             //sub rdx, rax
            rdx *= rcx;             //imul rdx, rcx
            rax = 0xBBD9DF3CECEEFE74;               //mov rax, 0xBBD9DF3CECEEFE74
            rdx ^= rax;             //xor rdx, rax
            rax = 0x23B4F504FA125955;               //mov rax, 0x23B4F504FA125955
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 6:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D98AE9]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rax += 0xFFFFFFFFA7D1474C;              //add rax, 0xFFFFFFFFA7D1474C
            rdx += rax;             //add rdx, rax
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rax = 0xFFFFFFFFC0CD4EE3;               //mov rax, 0xFFFFFFFFC0CD4EE3
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x23;           //shr rax, 0x23
            rax ^= rdx;             //xor rax, rdx
            rdx = 0x5A8397EF69EB3410;               //mov rdx, 0x5A8397EF69EB3410
            rax += r11;             //add rax, r11
            rax += rdx;             //add rax, rdx
            rdx = mb;               //lea rdx, [0xFFFFFFFFFD6595F6]
            rdx += rax;             //add rdx, rax
            rax = 0x94B908816CF2DBE1;               //mov rax, 0x94B908816CF2DBE1
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 7:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D986D8]
            r15 = mb + 0x6B60;              //lea r15, [0xFFFFFFFFFD660029]
            rax = mb;               //lea rax, [0xFFFFFFFFFD6591DF]
            rax += 0xC77B;          //add rax, 0xC77B
            rax += r11;             //add rax, r11
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x9;            //shr rax, 0x09
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax *= r11;             //imul rax, r11
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx += rax;             //add rdx, rax
            rax = 0x3BAB7EE1C2FB5485;               //mov rax, 0x3BAB7EE1C2FB5485
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rdx += rax;             //add rdx, rax
            rax = 0xD64310FF7669DED5;               //mov rax, 0xD64310FF7669DED5
            rdx *= rax;             //imul rdx, rax
            rax = 0xC9A0080E2B52320A;               //mov rax, 0xC9A0080E2B52320A
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 8:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D98105]
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rax += 0xFFFFFFFF954B94E9;              //add rax, 0xFFFFFFFF954B94E9
            rdx += rax;             //add rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0xE;            //shr rax, 0x0E
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1C;           //shr rax, 0x1C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x38;           //shr rax, 0x38
            rdx ^= rax;             //xor rdx, rax
            rax = 0x28853EAC80AAB90;                //mov rax, 0x28853EAC80AAB90
            rdx -= rax;             //sub rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xB;            //shr rax, 0x0B
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x16;           //shr rax, 0x16
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2C;           //shr rax, 0x2C
            rdx ^= rax;             //xor rdx, rax
            rax = 0x9ED615C5A516F48D;               //mov rax, 0x9ED615C5A516F48D
            rdx *= rax;             //imul rdx, rax
            rax = 0x4A5451CFD1051B0F;               //mov rax, 0x4A5451CFD1051B0F
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 9:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D97BEC]
            r13 = mb + 0x9F7F;              //lea r13, [0xFFFFFFFFFD66295C]
            rcx = mb + 0x590B7B0F;          //lea rcx, [0x0000000056710477]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x25;           //shr rax, 0x25
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0xE41AAE0B4978C7A7;               //mov rax, 0xE41AAE0B4978C7A7
            rdx *= rax;             //imul rdx, rax
            rax = 0x4884ED1EDA36D9B2;               //mov rax, 0x4884ED1EDA36D9B2
            rdx -= rax;             //sub rdx, rax
            rax = r11;              //mov rax, r11
            rax ^= r13;             //xor rax, r13
            rdx ^= rax;             //xor rdx, rax
            rax = 0xA5F46429036B04E5;               //mov rax, 0xA5F46429036B04E5
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x27;           //shr rax, 0x27
            rdx ^= rax;             //xor rdx, rax
            rdx -= r11;             //sub rdx, r11
            rdx += rcx;             //add rdx, rcx
            return rdx;
        }
        case 10:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D9775E]
            rdx -= mb;              //sub rdx, [rsp+0xA0] -- didn't find trace -> use base
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = r11;              //mov rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rax += 0xFFFFFFFFFFFF88EC;              //add rax, 0xFFFFFFFFFFFF88EC
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = 0x4DBC160E13E56349;               //mov rax, 0x4DBC160E13E56349
            rdx *= rax;             //imul rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD658434]
            rdx ^= rax;             //xor rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD658306]
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        case 11:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D972B8]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = mb + 0x73A4FAE9;          //lea rax, [0x00000000710A78DD]
            rax = ~rax;             //not rax
            rdx -= r11;             //sub rdx, r11
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x16;           //shr rax, 0x16
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2C;           //shr rax, 0x2C
            rdx ^= rax;             //xor rdx, rax
            rax = 0x861DF3431C84C629;               //mov rax, 0x861DF3431C84C629
            rdx *= rax;             //imul rdx, rax
            rax = 0x714B44E8CE73C4F0;               //mov rax, 0x714B44E8CE73C4F0
            rdx -= rax;             //sub rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD657D58]
            rcx = rax * 0xFFFFFFFFFFFFFFFE;                 //imul rcx, rax, 0xFFFFFFFFFFFFFFFE
            rax = 0x6F9175143B9ED737;               //mov rax, 0x6F9175143B9ED737
            rdx += rax;             //add rdx, rax
            rdx += rcx;             //add rdx, rcx
            return rdx;
        }
        case 12:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D96EF9]
            r15 = mb + 0xD3DA;              //lea r15, [0xFFFFFFFFFD6650C4]
            rcx = r11;              //mov rcx, r11
            rcx = ~rcx;             //not rcx
            rax = mb + 0x1F86111B;          //lea rax, [0x000000001CEB8AC8]
            rax = ~rax;             //not rax
            rcx *= rax;             //imul rcx, rax
            rax = r15;              //mov rax, r15
            rax -= r11;             //sub rax, r11
            rax += rdx;             //add rax, rdx
            rdx = rcx;              //mov rdx, rcx
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rcx = mb;               //lea rcx, [0xFFFFFFFFFD657B5D]
            rax = r11;              //mov rax, r11
            rax -= rcx;             //sub rax, rcx
            rcx = rax + 0xffffffffca7be9d9;                 //lea rcx, [rax-0x35841627]
            rcx += rdx;             //add rcx, rdx
            rax = rcx;              //mov rax, rcx
            rax >>= 0x18;           //shr rax, 0x18
            rcx ^= rax;             //xor rcx, rax
            rax = 0xFBA7ABC8BBB4629D;               //mov rax, 0xFBA7ABC8BBB4629D
            rdx = rcx;              //mov rdx, rcx
            rdx >>= 0x30;           //shr rdx, 0x30
            rdx ^= rcx;             //xor rdx, rcx
            rdx *= rax;             //imul rdx, rax
            rax = 0x1FE6307AA1F54B4D;               //mov rax, 0x1FE6307AA1F54B4D
            rdx *= rax;             //imul rdx, rax
            rax = 0x57A7A919AF723E1B;               //mov rax, 0x57A7A919AF723E1B
            rdx -= rax;             //sub rdx, rax
            return rdx;
        }
        case 13:
        {
            r9 = *(uintptr_t*)(mb + 0x973F1FC);             //mov r9, [0x0000000006D96AFF]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1A;           //shr rax, 0x1A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x34;           //shr rax, 0x34
            rdx ^= rax;             //xor rdx, rax
            rax = 0x525F068BC2643DF7;               //mov rax, 0x525F068BC2643DF7
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xD;            //shr rax, 0x0D
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1A;           //shr rax, 0x1A
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x34;           //shr rax, 0x34
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x75DFF140FA1FB5BC;               //mov rax, 0x75DFF140FA1FB5BC
            rdx += rax;             //add rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD657536]
            rdx += rax;             //add rdx, rax
            rax = 0xE5945E699002C625;               //mov rax, 0xE5945E699002C625
            rdx *= rax;             //imul rdx, rax
            rax = mb;               //lea rax, [0xFFFFFFFFFD65751E]
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 14:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D9669A]
            r15 = mb + 0x73A4A654;          //lea r15, [0x00000000710A1ADF]
            rax = 0x75736E13202430E1;               //mov rax, 0x75736E13202430E1
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rdx *= *(uintptr_t*)(rcx + 0x11);               //imul rdx, [rcx+0x11]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x9;            //shr rax, 0x09
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rdx += r11;             //add rdx, r11
            rax = r11;              //mov rax, r11
            rax = ~rax;             //not rax
            rax *= r15;             //imul rax, r15
            rdx ^= rax;             //xor rdx, rax
            rax = 0xABD8E138F25E5687;               //mov rax, 0xABD8E138F25E5687
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 15:
        {
            r10 = *(uintptr_t*)(mb + 0x973F1FC);            //mov r10, [0x0000000006D962E9]
            rsi = 0xB656FAE057EB613B;               //mov rsi, 0xB656FAE057EB613B
            rdx *= rsi;             //imul rdx, rsi
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x11);               //imul rdx, [rax+0x11]
            rax = 0x5CA0A4447C245D90;               //mov rax, 0x5CA0A4447C245D90
            rdx -= rax;             //sub rdx, rax
            rdx -= r11;             //sub rdx, r11
            rax = 0xF071D0312866EB9D;               //mov rax, 0xF071D0312866EB9D
            rdx *= rax;             //imul rdx, rax
            rax = 0xFFFFFFFFFFFFF34A;               //mov rax, 0xFFFFFFFFFFFFF34A
            rax -= r11;             //sub rax, r11
            rax -= mb;              //sub rax, [rsp+0xA0] -- didn't find trace -> use base
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        }
    }
    uint64_t get_bone_index(uint32_t bone_index)
    {
        auto mb = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rdi = bone_index;
        rcx = rdi * 0x13C8;
        rax = 0x2AC83BD095F98207;               //mov rax, 0x2AC83BD095F98207
        rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
        rax = rcx;              //mov rax, rcx
        r11 = mb;               //lea r11, [0xFFFFFFFFFD1559B2]
        rax -= rdx;             //sub rax, rdx
        r10 = 0xD1F23647CF68B0F;                //mov r10, 0xD1F23647CF68B0F
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0xC;            //shr rax, 0x0C
        rax = rax * 0x1B6B;             //imul rax, rax, 0x1B6B
        rcx -= rax;             //sub rcx, rax
        rax = 0x6768ECB62DD904F3;               //mov rax, 0x6768ECB62DD904F3
        r8 = rcx * 0x1B6B;              //imul r8, rcx, 0x1B6B
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rdx >>= 0xC;            //shr rdx, 0x0C
        rax = rdx * 0x279C;             //imul rax, rdx, 0x279C
        r8 -= rax;              //sub r8, rax
        rax = 0x8888888888888889;               //mov rax, 0x8888888888888889
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = 0x1778A191BD684181;               //mov rax, 0x1778A191BD684181
        rdx >>= 0x4;            //shr rdx, 0x04
        rcx = rdx * 0x1E;               //imul rcx, rdx, 0x1E
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r8;               //mov rax, r8
        rax -= rdx;             //sub rax, rdx
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0x8;            //shr rax, 0x08
        rcx += rax;             //add rcx, rax
        rax = rcx * 0x3AA;              //imul rax, rcx, 0x3AA
        rcx = r8 * 0x3AC;               //imul rcx, r8, 0x3AC
        rcx -= rax;             //sub rcx, rax
        rax = *(uint16_t*)(rcx + r11 * 1 + 0x9817310);          //movzx eax, word ptr [rcx+r11*1+0x9817310]
        r8 = rax * 0x13C8;              //imul r8, rax, 0x13C8
        rax = r10;              //mov rax, r10
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r10;              //mov rax, r10
        rdx >>= 0x9;            //shr rdx, 0x09
        rcx = rdx * 0x2705;             //imul rcx, rdx, 0x2705
        r8 -= rcx;              //sub r8, rcx
        r9 = r8 * 0x49D8;               //imul r9, r8, 0x49D8
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rdx >>= 0x9;            //shr rdx, 0x09
        rax = rdx * 0x2705;             //imul rax, rdx, 0x2705
        r9 -= rax;              //sub r9, rax
        rax = 0xA10BFDF49906A50F;               //mov rax, 0xA10BFDF49906A50F
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rcx = r9;               //mov rcx, r9
        r9 &= 0x1;             //and r9d, 0x01
        rdx >>= 0xC;            //shr rdx, 0x0C
        rax = rdx * 0x196F;             //imul rax, rdx, 0x196F
        rcx -= rax;             //sub rcx, rax
        rax = r9 + rcx * 2;             //lea rax, [r9+rcx*2]
        r12 = *(uint16_t*)(r11 + rax * 2 + 0x981E100);          //movsx r12d, word ptr [r11+rax*2+0x981E100]
        return r12;
    }
}

namespace sdk
{
    const DWORD nTickTime = 64;//64 ms
    bool bUpdateTick = false;

    bool in_game()
    {
        if (globals::platform == 0)
        {
            auto gameMode = *(int*)(g_data::base + bnet::game_mode);
            return  gameMode > 1;
        }
        else if (globals::platform == 1)
        {
            auto gameMode = *(int*)(g_data::base + steam::game_mode);
            return  gameMode > 1;
        }
    }

    int get_max_player_count()
    {
        if (globals::platform == 0)
        {
            return *(int*)(g_data::base + bnet::game_mode);
        }
        else if (globals::platform == 1)
        {
            return *(int*)(g_data::base + steam::game_mode);
        }
    }

    player_t get_player(int i)
    {
        if (globals::platform == 0)
        {
            auto client_info_base = bnet::get_client_info_base();
            if (is_bad_ptr(client_info_base)) return player_t(NULL);

            auto player = client_info_base + (i * bnet::player_size);
            if (is_bad_ptr(player)) return player_t(NULL);

            return player_t(player);

        }
        else if (globals::platform == 1)
        {
            uintptr_t client_info = steam::Get_cg_t();
            if (is_bad_ptr(client_info)) return player_t(NULL);

            uintptr_t client_info_base = steam::get_client_info_base();
            if (is_bad_ptr(client_info_base)) return player_t(NULL);

            return player_t(client_info_base + (i * steam::player_size));

        }
    }

    player_t get_local_player()
    {
        if (globals::platform == 0)
        {
            uintptr_t decryptedPtr = bnet::get_client_info();
            if (is_bad_ptr(decryptedPtr))return player_t(NULL);

            auto local_index = *(uintptr_t*)(decryptedPtr + bnet::local_index);
            if (is_bad_ptr(local_index))return player_t(NULL);

            auto index = *(int*)(local_index + bnet::local_index_pos);
            return get_player(index);
        }
        else if (globals::platform == 1)
        {
            uintptr_t decryptedPtr = steam::Get_cg_t();
            if (is_bad_ptr(decryptedPtr))return player_t(NULL);

            auto local_index = *(uintptr_t*)(decryptedPtr + steam::local_index);
            if (is_bad_ptr(local_index))return player_t(NULL);

            auto index = *(int*)(local_index + steam::local_index_pos);
            return get_player(index);
        }
    }

    refdef_t* get_refdef()
    {
        if (globals::platform == 0)
        {
            uint32_t crypt_0 = *(uint32_t*)(g_data::base + bnet::refdef_ptr);
            uint32_t crypt_1 = *(uint32_t*)(g_data::base + bnet::refdef_ptr + 0x4);
            uint32_t crypt_2 = *(uint32_t*)(g_data::base + bnet::refdef_ptr + 0x8);

            // lower 32 bits
            uint32_t entry_1 = (uint32_t)(g_data::base + bnet::refdef_ptr);
            uint32_t entry_2 = (uint32_t)(g_data::base + bnet::refdef_ptr + 0x4);

            // decryption
            uint32_t _low = entry_1 ^ crypt_2;
            uint32_t _high = entry_2 ^ crypt_2;
            uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
            uint32_t high_bit = crypt_1 ^ _high * (_high + 2);
            auto ret = (refdef_t*)(((QWORD)high_bit << 32) + low_bit);
            if (is_valid_ptr(ret))
                return ret;
            else
                return 0;
        }
        else if (globals::platform == 1)
        {
            uint32_t crypt_0 = *(uint32_t*)(g_data::base + steam::refdef_ptr);
            uint32_t crypt_1 = *(uint32_t*)(g_data::base + steam::refdef_ptr + 0x4);
            uint32_t crypt_2 = *(uint32_t*)(g_data::base + steam::refdef_ptr + 0x8);

            // lower 32 bits
            uint32_t entry_1 = (uint32_t)(g_data::base + steam::refdef_ptr);
            uint32_t entry_2 = (uint32_t)(g_data::base + steam::refdef_ptr + 0x4);

            // decryption
            uint32_t _low = entry_1 ^ crypt_2;
            uint32_t _high = entry_2 ^ crypt_2;
            uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
            uint32_t high_bit = crypt_1 ^ _high * (_high + 2);
            auto ret = (refdef_t*)(((QWORD)high_bit << 32) + low_bit);
            if (is_valid_ptr(ret))
                return ret;
            else
                return 0;
        }
    }

    bool get_bone_by_player_index(int i, int bone_id, Vector3* out_bone_pos)
    {
        if (globals::platform == 0)
        {
            uint64_t decrypted_ptr = bnet::get_bone_ptr();

            if (is_bad_ptr(decrypted_ptr))return false;

            unsigned short index = bnet::get_bone_index(i);
            if (index != 0)
            {
                uint64_t bone_ptr = *(uint64_t*)(decrypted_ptr + (index * bnet::bone_size) + bnet::bone_offset);

                if (is_bad_ptr(bone_ptr))return false;

                Vector3 bone_pos = *(Vector3*)(bone_ptr + (bone_id * 0x20) + 0x10);

                if (bone_pos.IsZero())return false;

                uint64_t client_info = bnet::get_client_info();

                if (is_bad_ptr(client_info))return false;

                Vector3 basepos = *(Vector3*)(client_info + bnet::bone_base_pos);

                if (basepos.IsZero())return false;

                bone_pos.x += basepos.x;
                bone_pos.y += basepos.y;
                bone_pos.z += basepos.z;

                *out_bone_pos = bone_pos;
                return true;
            }
            return false;
        }
        else if (globals::platform == 1)
        {
            uint64_t decrypted_ptr = steam::get_bone_ptr();

            if (is_bad_ptr(decrypted_ptr))return false;

            unsigned short index = steam::get_bone_index(i);
            if (index != 0)
            {
                uint64_t bone_ptr = *(uint64_t*)(decrypted_ptr + (index * steam::bone_size) + steam::bone_offset);

                if (is_bad_ptr(bone_ptr))return false;

                Vector3 bone_pos = *(Vector3*)(bone_ptr + (bone_id * 0x20) + 0x10);

                if (bone_pos.IsZero())return false;

                uint64_t client_info = steam::Get_cg_t();

                if (is_bad_ptr(client_info))return false;

                Vector3 basepos = *(Vector3*)(client_info + steam::bone_base_pos);

                if (basepos.IsZero())return false;

                bone_pos.x += basepos.x;
                bone_pos.y += basepos.y;
                bone_pos.z += basepos.z;

                *out_bone_pos = bone_pos;
                return true;
            }
            return false;
        }
    }

    int get_player_health(int i)
    {
        if (globals::platform == 0)
        {
            uint64_t bgs = *(uint64_t*)(g_data::base + bnet::name_array);
            if (is_bad_ptr(bgs))return 0;
            clientinfo_t* pClientInfo = (clientinfo_t*)(bgs + bnet::name_array_padding + (i * bnet::name_array_size));
            if (is_bad_ptr(pClientInfo))return 0;
            uint8_t healthValue = pClientInfo->health;
            float healthPercentage = ((float)healthValue / 100.0f) * 100.0f;
            if (healthPercentage >= 100.0f)
                return 100;
            else
                return (int)(healthPercentage + 0.5f);
        }
        else if (globals::platform == 1)
        {
            uint64_t bgs = *(uint64_t*)(g_data::base + steam::name_array);
            if (is_bad_ptr(bgs))return 0;
            clientinfo_t* pClientInfo = (clientinfo_t*)(bgs + steam::name_array_padding + (i * steam::name_array_size));
            if (is_bad_ptr(pClientInfo))return 0;
            uint8_t healthValue = pClientInfo->health;
            float healthPercentage = ((float)healthValue / 100.0f) * 100.0f;
            if (healthPercentage >= 100.0f)
                return 100;
            else
                return (int)(healthPercentage + 0.5f);
        }
    }

    std::string get_player_name(int entityNum)
    {
        if (globals::platform == 0)
        {
            uint64_t bgs = *(uint64_t*)(g_data::base + bnet::name_array);
            if (is_bad_ptr(bgs))return XOR("Player");
            clientinfo_t* clientInfo_ptr = (clientinfo_t*)(bgs + bnet::name_array_padding + (entityNum * bnet::name_array_size));
            if (is_bad_ptr(clientInfo_ptr))return XOR("Player");
            return clientInfo_ptr->name;
        }
        else if (globals::platform == 1)
        {
            uint64_t bgs = *(uint64_t*)(g_data::base + steam::name_array);
            if (is_bad_ptr(bgs))return XOR("Player");
            clientinfo_t* clientInfo_ptr = (clientinfo_t*)(bgs + steam::name_array_padding + (entityNum * steam::name_array_size));
            if (is_bad_ptr(clientInfo_ptr))return XOR("Player");
            return clientInfo_ptr->name;
        }
    }

    // player class methods
    bool player_t::is_valid() {
        if (globals::platform == 0)
        {
            return *(bool*)((uint64_t)address + bnet::player_valid);
        }
        else if (globals::platform == 1)
        {
            return *(bool*)((uint64_t)address + steam::player_valid);
        }
    }

    bool player_t::is_dead() {
        if (globals::platform == 0)
        {
            auto dead1 = *(bool*)((uint64_t)address + bnet::player_dead_1);
            auto dead2 = *(bool*)((uint64_t)address + bnet::player_dead_2);
            if (is_valid_ptr(address))
                return !(!dead1 && !dead2 && !is_valid());
            else
                return 0;
        }
        else if (globals::platform == 1)
        {
            auto dead1 = *(bool*)((uint64_t)address + steam::player_dead_1);
            auto dead2 = *(bool*)((uint64_t)address + steam::player_dead_2);
            if (is_valid_ptr(address))
                return !(!dead1 && !dead2 && !is_valid());
            else
                return 0;
        }
    }

    int player_t::team_id() {
        if (globals::platform == 0)
        {
            if (is_valid_ptr(address))
                return *(uint8_t*)((uint64_t)address + bnet::player_team_id);
            else
                return 0;
        }
        else if (globals::platform == 1)
        {
            if (is_valid_ptr(address))
                return *(uint8_t*)((uint64_t)address + steam::player_team_id);
            else
                return 0;
        }
    }

    Vector3 player_t::get_pos()
    {
        if (globals::platform == 0)
        {
            auto local_pos_ptr = *(uint64_t*)((uint64_t)address + bnet::player_pos);

            if (is_valid_ptr(local_pos_ptr))
                return *(Vector3*)(local_pos_ptr + 0x48);
            else
                return{};
            return Vector3{};
        }
        else if (globals::platform == 1)
        {
            auto local_pos_ptr = *(__int64*)((uint64_t)address + steam::player_pos);

            if (is_valid_ptr(local_pos_ptr))
                return *(Vector3*)(local_pos_ptr + 0x48);
            else
                return{};
            return Vector3{};
        }
    }

    uintptr_t player_t::get_index()
    {
        if (globals::platform == 0)
        {
            if (is_bad_ptr(this->address))return 0;

            auto cl_info_base = bnet::get_client_info_base();
            if (is_bad_ptr(cl_info_base))return 0;

            return ((uintptr_t)this->address - cl_info_base) / bnet::player_size;
        }
        else if (globals::platform == 1)
        {
            if (is_bad_ptr(this->address))return 0;

            auto cl_info_base = steam::get_client_info_base();
            if (is_bad_ptr(cl_info_base))return 0;

            return ((uintptr_t)this->address - cl_info_base) / steam::player_size;
        }
    }

    int player_t::get_stance()
    {
        if (globals::platform == 0)
        {
            auto ret = *(int*)((uintptr_t)address + bnet::player_stance);
            if (is_valid_ptr(address))
                return ret;
            else
                return 4;
        }
        else if (globals::platform == 1)
        {
            auto ret = *(int*)((uintptr_t)address + steam::player_stance);
            if (is_valid_ptr(address))
                return ret;
            else
                return 4;
        }
    }

}