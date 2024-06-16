// EditView.h : interface of the CEditView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CEditView
{
public:
	HWND m_hWnd = nullptr;
	
	void SaveText() // save the text into SQLite before exiting
	{
		if (IsWindow())
		{
			U32 utf8Len = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (utf8Len)
			{
				if (g_editMax < utf8Len) // if the content is too long, we need to enlarge the buffer
				{
					g_editMax = WT_ALIGN_PAGE(utf8Len + 1);
					if (nullptr != g_editBuf)
					{
						VirtualFree(g_editBuf, 0, MEM_RELEASE);
						g_editBuf = nullptr;
					}
					g_editBuf = (U8*)VirtualAlloc(NULL, g_editMax, MEM_COMMIT, PAGE_READWRITE);
				}
				if (g_editBuf)
				{
					ATLASSERT(g_editMax >= utf8Len);
					::SendMessage(m_hWnd, SCI_GETTEXT, utf8Len, (LPARAM)g_editBuf);
					g_editLen = utf8Len;
				}
			}
		}
	}

	HWND Create(
		_In_opt_ HWND hWndParent,
		_In_ _U_RECT rect = NULL,
		_In_opt_z_ LPCTSTR szWindowName = NULL,
		_In_ DWORD dwStyle = 0,
		_In_ DWORD dwExStyle = 0,
		_In_ _U_MENUorID MenuOrID = 0U,
		_In_opt_ LPVOID lpCreateParam = NULL)
	{
		m_hWnd = CreateWindowExW(0, L"Scintilla", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL,
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
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETVSCROLLBAR, 0, 0);
#if 0
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 2);
			::SendMessage(m_hWnd, SCI_SETMARGINTYPEN, 4, SC_MARGIN_NUMBER);
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 228, 0));
#endif
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

	HWND SetFocus()
	{
		HWND hWnd = NULL;
		if (IsWindow())
		{
			hWnd = ::SetFocus(m_hWnd);
		}
		return hWnd;
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

	BOOL SetWindowPos(HWND hWndInsertAfter,	int X,	int Y,	int cx,int cy,UINT uFlags)
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

	int SetText(const char* text, U32 length)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
		}
		return 0;
	}

	int sci_AppendText(const char* text, U32 length)
	{
		if (IsWindow())
		{
			int readonly = (int)::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0);
			::SendMessage(m_hWnd, EM_SETREADONLY, FALSE, 0);
			::SendMessage(m_hWnd, SCI_APPENDTEXT, length, (LPARAM)text);
			if (readonly)
				::SendMessage(m_hWnd, SCI_GETREADONLY, TRUE, 0);
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

	int sci_SetWrapMode(int wrapmode)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, wrapmode, 0);
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

	BOOL GetFocus()
	{
		return (m_hWnd == ::GetFocus());
	}

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

	bool DoEditPaste()
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_PASTE, 0, 0);
		}
		return true;
	}

	bool DoEditCut()
	{
		bool bRet = false;
		if (IsWindow())
		{
			bool bNoSelection = (bool)::SendMessage(m_hWnd, SCI_GETSELECTIONEMPTY, 0, 0);
			if (bNoSelection == false)
			{
				::SendMessage(m_hWnd, SCI_CUT, 0, 0);
				bRet = true;
			}
		}
		return bRet;
	}

	bool DoEditUndo()
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_UNDO, 0, 0);
		}
		return true;
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
};
