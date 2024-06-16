// ZTerm.h

#pragma once

#include "wt/wt_utils.h"
#include "wt/wt_unicode.h"
#include "wt/wt_mempool.h"
#include "wt/wt_sha256.h"
#include "wt/wt_aes256.h"
#include "wt/wt_chacha20.h"

#include "config.h"
#include "network.h"

#define WM_BRING_TO_FRONT   (WM_USER + 1)
#define WM_NETWORK_STATUS   (WM_USER + 2)
#define WM_LAUNCH_XPLAYER   (WM_USER + 3)
#define WM_LOADPERCENTMSG	(WM_USER + 4)
#define WM_NET_STATUS_MSG	(WM_USER + 5)
#define WM_LOADNOTEPADMSG	(WM_USER + 6)
#define WM_SYNCKBDATA_MSG	(WM_USER + 7)

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template <class T>
inline void ReleaseUnknown(T*& ppUnknown) noexcept {
    if (ppUnknown) {
        try {
            ppUnknown->Release();
        }
        catch (...) {
            // Never occurs
        }
        ppUnknown = nullptr;
    }
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
    if (!hModule) {
        return nullptr;
    }
    FARPROC function = ::GetProcAddress(hModule, lpProcName);
    static_assert(sizeof(T) == sizeof(function));
    T fp{};
    memcpy(&fp, &function, sizeof(T));
    return fp;
}

#define MESSAGE_HANDLER_DUIWINDOW(msg, func) \
	if(true) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

extern ID2D1Factory* g_pD2DFactory;
extern IDWriteFactory* g_pDWriteFactory;
extern UINT  g_nDPI;

extern HWND	g_hWndTTY;
extern HWND	g_hWndXPlayer;

extern WCHAR     g_exePath[MAX_PATH + 1];
extern WCHAR     g_appPath[MAX_PATH + 1];
extern WCHAR     g_cachePath[MAX_PATH + 1];
extern WCHAR     g_dbFilePath[MAX_PATH + 1];

extern volatile LONG g_threadCount;
extern volatile LONG  g_threadCountBKG;
extern volatile LONG g_Quit;
extern volatile DWORD g_dwMainThreadID;

extern ZXConfig ZXCONFIGURATION;
#if 0
//extern ZXFileTree ZXFILETREE;
#endif 
extern ZXKBTree* g_rootKBNode;

/* the message queue from the remote server */
extern MessageTask* g_sendQueue;
extern MessageTask* g_receQueue;

/* used to sync different threads */
extern CRITICAL_SECTION  g_csSendMsg;
extern CRITICAL_SECTION  g_csReceMsg;

extern MemoryPoolContext g_sendMemPool;
extern MemoryPoolContext g_receMemPool;
extern MemoryPoolContext g_kbMemPool;

extern U8* g_editBuf;
extern U32 g_editLen;
extern U32 g_editMax;

int DUI_Init();
void DUI_Term();

void zx_InitZXConfig(ZXConfig* cf);
void zx_SaveDocumentInformation(U8* docId, U8* docTitle);
U32 zx_Write_LogMessage(const char* message, U32 length, U8 rs);

DWORD WINAPI LoadingDataThread(LPVOID lpData);

#ifdef __cplusplus
extern "C" {
#endif
    int  PTerm_Init(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int show);
    void PTerm_Exit();
    HWND PTerm_CreateWindow(HWND hWndParent);
    BOOL PTerm_before_loop(MSG* msg);
    bool PTerm_run_toplevel_callbacks(void);
    int  PTerm_do_settings(HWND hWnd);
    int PTerm_do_paste(HWND hWndParent);
    unsigned int PTerm_copy_screen(wchar_t* caller_buff, unsigned int max_length);

#ifdef __cplusplus
}
#endif


