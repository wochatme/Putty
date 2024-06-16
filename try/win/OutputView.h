// OutputView.h : interface of the COutputView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

typedef struct
{
	HWND hWnd;
	WCHAR fname[MAX_PATH + 1];
	U8* data;
	U32 size;
} WriteFileInfo;

static WriteFileInfo wfi = { 0 };
static DWORD WINAPI savefile_threadfunc(void* param);

#pragma warning( disable: 4838)
static const char ai_greeting[] =
{
	'\n',
	0xF0,0x9F,0x99,0x82,0x0A,0x48,0x65,0x6C,
	0x6C,0x6F,0x2C,0x20,0x69,0x66,0x20,0x79,
	0x6F,0x75,0x20,0x68,0x61,0x76,0x65,0x20,
	0x61,0x6E,0x79,0x20,0x74,0x65,0x63,0x68,
	0x6E,0x69,0x63,0x61,0x6C,0x20,0x71,0x75,
	0x65,0x73,0x74,0x69,0x6F,0x6E,0x2C,0x20,
	0x70,0x6C,0x65,0x61,0x73,0x65,0x20,0x69,
	0x6E,0x70,0x75,0x74,0x20,0x69,0x6E,0x20,
	0x74,0x68,0x65,0x20,0x62,0x65,0x6C,0x6F,
	0x77,0x20,0x77,0x69,0x6E,0x64,0x6F,0x77,
	0x2E,0x0A,0
};

class COutputView
{
public:
	HWND m_hWnd = nullptr;

	HWND Create(
		_In_opt_ HWND hWndParent,
		_In_ _U_RECT rect = NULL,
		_In_opt_z_ LPCTSTR szWindowName = NULL,
		_In_ DWORD dwStyle = 0,
		_In_ DWORD dwExStyle = 0,
		_In_ _U_MENUorID MenuOrID = 0U,
		_In_opt_ LPVOID lpCreateParam = NULL)
	{
		m_hWnd = CreateWindowExW(0, L"Scintilla", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			0, 0, 16, 16, hWndParent, NULL, HINST_THISCOMPONENT, NULL);

		if (::IsWindow(m_hWnd))
		{
#if 0
			sci_SetTechnology(SC_TECHNOLOGY_DIRECTWRITE);
			sci_SetCodePage(SC_CP_UTF8);
			sci_SetEOLMode(SC_EOL_LF);
			sci_SetWrapMode(SC_WRAP_WORD);
			sci_StyleSetFont(STYLE_DEFAULT, (const char*)"Courier New");
			sci_StyleSetFontSize(STYLE_DEFAULT, 11);
#endif 
			ZXConfig* cf = &ZXCONFIGURATION;

			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			if (cf->font_Name && cf->font_Size)
			{
				::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)cf->font_Name);
				::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, cf->font_Size*100);
			}
			else
			{
				::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
				::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);
			}

			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);
			::SendMessage(m_hWnd, SCI_SETVSCROLLBAR, 0, 0);
		}
		return m_hWnd;
	}

	operator HWND() const throw()
	{
		return m_hWnd;
	}

	BOOL IsWindow() const throw()
	{
		return ::IsWindow(m_hWnd);
	}

	BOOL GetFocus()
	{
		return (m_hWnd == ::GetFocus());
	}

	BOOL ShowWindow(int nCmdShow)
	{
		BOOL bRet = FALSE;

		if (IsWindow())
		{
			bRet = ::ShowWindow(m_hWnd, nCmdShow);
		}

		return bRet;
	}

	BOOL SetWindowPos(HWND hWndInsertAfter,	int  X,	int  Y,	int  cx, int  cy, UINT uFlags)
	{
		BOOL bRet = FALSE;
		if (IsWindow())
		{
			bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
		return bRet;
	}

#if 0
	int sci_SetTechnology(int mode)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, mode, 0);
		}
		return 0;
	}

	int sci_SetCodePage(int codepage)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, codepage, 0);
		}
		return 0;
	}

	int sci_SetEOLMode(int eol)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETEOLMODE, eol, 0);
		}
		return 0;
	}
#endif 
	int sci_GetTextLength(U32& length)
	{
		length = 0;
		if (IsWindow())
		{
			length = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
		}
		return 0;
	}

	int sci_GetText(char* text, U32 buf_max)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_GETTEXT, buf_max, (LPARAM)text);
		}
		return 0;
	}

	int ScrollToBottom()
	{
		if (IsWindow())
		{
			int totalLines = (int)SendMessage(m_hWnd, SCI_GETLINECOUNT, 0, 0);
			SendMessage(m_hWnd, SCI_LINESCROLL, 0, totalLines);
		}
		return 0;
	}

#if 0
	int SetText(const char* text)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
		}
		return 0;
	}
#endif 
	int AppendText(const char* text, U32 length)
	{
		if (IsWindow())
		{
			int totalLines;
			//bool readonly = (bool)::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_APPENDTEXT, length, (LPARAM)text);
			totalLines = (int)SendMessage(m_hWnd, SCI_GETLINECOUNT, 0, 0);
			::SendMessage(m_hWnd, SCI_LINESCROLL, 0, totalLines);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);
		}
		return 0;
	}

	int sci_SetWrapMode(int wrapmode)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, wrapmode, 0);
		}
		return 0;
	}

	int sci_SetReadOnly(BOOL readonly = TRUE)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, EM_SETREADONLY, readonly, 0);
		}
		return 0;
	}

	int sci_StyleSetFont(int style, const char* fontName)
	{
		if (IsWindow() && fontName)
		{
			::SendMessage(m_hWnd, SCI_STYLESETFONT, style, (LPARAM)fontName);
		}
		return 0;
	}

	int sci_StyleSetFontSize(int style, int pointSize)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, style, pointSize * 100);
		}
		return 0;
	}
#if 0
	int sci_GetCurrentPosition()
	{
		int r = 0;
		if (IsWindow())
		{
			r = (int)::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0);
		}
		return r;
	}

	char sci_GetCharAtPosition(int pos)
	{
		char ch = '\0';
		if (IsWindow())
		{
			ch = (char)::SendMessage(m_hWnd, SCI_GETCHARAT, pos, 0);
		}
		return ch;
	}

	char sci_GetLastChar()
	{
		char ch = '\0';
		if (IsWindow())
		{
			int pos = (int)::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0);
			if (pos > 0)
				ch = (char)::SendMessage(m_hWnd, SCI_GETCHARAT, pos - 1, 0);
		}
		return ch;
	}
#endif
	bool DoEditCopy()
	{
		bool bRet = false;
		if (IsWindow())
		{
			bool bNoSelection = (bool)::SendMessage(m_hWnd, SCI_GETSELECTIONEMPTY, 0, 0);
			if (bNoSelection == false)
			{
				::SendMessage(m_hWnd, SCI_COPY, 0, 0);
				bRet = true;
			}
		}
		return bRet;
	}

	void DoFontChange()
	{
		ZXConfig* cf = &ZXCONFIGURATION;

		if (cf->font_Name && cf->font_Size)
		{
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)cf->font_Name);
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, cf->font_Size * 100);
		}
		else
		{
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);
		}
		if (::IsWindowVisible(m_hWnd))
		{
			::InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	void DoEmptyChatLog()
	{
		if (IsWindow())
		{
			U32 length = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (length > (U32)strlen((const char*)ai_greeting))
			{
				int choice = ::MessageBoxW(m_hWnd, L"Do you want to clear the chat history?", L"Clear Chat History", MB_YESNO);
				if (choice == IDYES)
				{
					::SendMessage(m_hWnd, SCI_SETREADONLY, FALSE, 0);
					::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
					::SendMessage(m_hWnd, SCI_SETREADONLY, TRUE, 0);
				}
			}
		}
	}

	void DoSaveChatLog()
	{
		if (IsWindow())
		{
			U32 length = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (length > (U32)strlen((const char*)ai_greeting))
			{
				WCHAR filename[MAX_PATH + 1] = { 0 };
				SYSTEMTIME st;
				GetSystemTime(&st);

				swprintf((WCHAR*)filename, MAX_PATH, L"zterm_chat_%04d_%2d_%2d_%2d_%2d_%2d.txt", 
					st.wYear, 
					st.wMonth,
					st.wDay,
					st.wHour,
					st.wMinute,
					st.wSecond
				);

				CFileDialog fileDlg(FALSE, _T("txt"), filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					_T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"), m_hWnd);
				
				if (fileDlg.DoModal() == IDOK)
				{
					WCHAR* wp = fileDlg.m_szFileName;
					size_t size = wcslen(wp);
					if (size && size < MAX_PATH)
					{
						char* buf = (char*)malloc(WT_ALIGN_DEFAULT64(length + 1));
						if (buf)
						{
							DWORD in_threadid = 0; /* required for Win9x */
							HANDLE hThread;

							SendMessage(m_hWnd, SCI_GETTEXT, length, (LPARAM)buf);
							for (size_t i = 0; i < size; i++) wfi.fname[i] = wp[i];
							wfi.fname[size] = L'\0';
							wfi.hWnd = m_hWnd;
							wfi.data = (U8*)buf;
							wfi.size = length;

							hThread = CreateThread(NULL, 0, savefile_threadfunc, &wfi, 0, &in_threadid);
							if (hThread) /* we don't need the thread handle */
								CloseHandle(hThread);
						}
					}
				}
			}
		}
	}
};

static DWORD WINAPI savefile_threadfunc(void* param)
{
	WriteFileInfo* pfi = (WriteFileInfo*)param;
	ATLASSERT(pfi);

	if (pfi->data && pfi->size)
	{
		size_t size = wcslen(pfi->fname);
		if (size)
		{
			int fd = 0;
			if (_wsopen_s(&fd, pfi->fname, _O_WRONLY | _O_CREAT, _SH_DENYNO, _S_IWRITE) == 0)
			{
				_write(fd, pfi->data, pfi->size);
				_close(fd);
			}
		}
		free(pfi->data);
		pfi->data = NULL;
		pfi->size = 0;
	}
	return 0;
}

