#include "stdafx.h"
#include "ZTerm.h"

#include "XView.h"

// global variables
U8          DUIMessageOSMap[MESSAGEMAP_TABLE_SIZE] = { 0 };
U64         dui_status;

HCURSOR	dui_hCursorArrow = nullptr;
HCURSOR	dui_hCursorHand = nullptr;
#if 0
HCURSOR	dui_hCursorWE = nullptr;
HCURSOR	dui_hCursorNS = nullptr;
HCURSOR	dui_hCursorIBeam = nullptr;
#endif 

int DUI_Init()
{
    int i;
    U8* p = DUIMessageOSMap;

    dui_hCursorArrow = ::LoadCursor(NULL, IDC_ARROW);
    dui_hCursorHand = ::LoadCursor(nullptr, IDC_HAND);

    if (nullptr == dui_hCursorHand || nullptr == dui_hCursorArrow)
        return DUI_FAILED;
#if 0
    for (i = 0; i < MESSAGEMAP_TABLE_SIZE; i++)
        p[i] = DUI_NULL;
#endif 

#ifdef _WIN32
    // We establish the mapping relationship between Windows message to DUI message
    p[WM_CREATE] = DUI_CREATE;
    p[WM_DESTROY] = DUI_DESTROY;
    p[WM_ERASEBKGND] = DUI_ERASEBKGND;
    p[WM_PAINT] = DUI_PAINT;
    p[WM_TIMER] = DUI_TIMER;
    p[WM_MOUSEMOVE] = DUI_MOUSEMOVE;
    p[WM_MOUSEWHEEL] = DUI_MOUSEWHEEL;
    p[WM_LBUTTONDOWN] = DUI_LBUTTONDOWN;
    p[WM_LBUTTONUP] = DUI_LBUTTONUP;
    p[WM_LBUTTONDBLCLK] = DUI_LBUTTONDBLCLK;
    p[WM_RBUTTONDOWN] = DUI_RBUTTONDOWN;
    p[WM_RBUTTONUP] = DUI_RBUTTONUP;
    p[WM_RBUTTONDBLCLK] = DUI_RBUTTONDBLCLK;
    p[WM_MBUTTONDOWN] = DUI_MBUTTONDOWN;
    p[WM_MBUTTONUP] = DUI_MBUTTONUP;
    p[WM_MBUTTONDBLCLK] = DUI_MBUTTONDBLCLK;
    p[WM_CAPTURECHANGED] = DUI_CAPTURECHANGED;
    p[WM_KEYFIRST] = DUI_KEYFIRST;
    p[WM_KEYDOWN] = DUI_KEYDOWN;
    p[WM_KEYUP] = DUI_KEYUP;
    p[WM_CHAR] = DUI_CHAR;
    p[WM_DEADCHAR] = DUI_DEADCHAR;
    p[WM_SYSKEYDOWN] = DUI_SYSKEYDOWN;
    p[WM_SYSKEYUP] = DUI_SYSKEYUP;
    p[WM_SYSCHAR] = DUI_SYSCHAR;
    p[WM_SYSDEADCHAR] = DUI_SYSDEADCHAR;
    p[WM_GETMINMAXINFO] = DUI_GETMINMAXINFO;
    p[WM_SIZE] = DUI_SIZE;
    p[WM_SETCURSOR] = DUI_SETCURSOR;
    p[WM_SETFOCUS] = DUI_SETFOCUS;
    p[WM_MOUSEACTIVATE] = DUI_MOUSEACTIVATE;
    p[WM_MOUSELEAVE] = DUI_MOUSELEAVE;
    p[WM_MOUSEHOVER] = DUI_MOUSEHOVER;
    p[WM_NCLBUTTONDOWN] = DUI_NCLBUTTONDOWN;
#endif

    return DUI_OK;
}

void DUI_Term()
{

}
