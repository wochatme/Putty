// InputView.h : interface of the CInputView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CInputView
{
	U8* m_inputBuf = nullptr;
	U32 m_inputMax = INPUT_BUF_INPUT_MAX;
	U32 m_inputLen = 0;

public:
	HWND m_hWnd = nullptr;

	CInputView()
	{
		m_inputMax = INPUT_BUF_INPUT_MAX;
		m_inputLen = 0;
		m_inputBuf = (U8*)VirtualAlloc(NULL, INPUT_BUF_INPUT_MAX, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_inputBuf);
	}

	~CInputView()
	{
		if (nullptr != m_inputBuf)
		{
			VirtualFree(m_inputBuf, 0, MEM_RELEASE);
			m_inputBuf = nullptr;
		}
		m_inputLen = 0;
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
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
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

	BOOL SetWindowPos(
		HWND hWndInsertAfter,
		int  X,
		int  Y,
		int  cx,
		int  cy,
		UINT uFlags
	)
	{
		BOOL bRet = FALSE;
		if (IsWindow())
		{
			bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
		return bRet;
	}
#if 0
	int SetTechnology(int mode)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, mode, 0);
		}
		return 0;
	}

	int SetCodePage(int codepage)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, codepage, 0);
		}
		return 0;
	}

	int SetEOLMode(int eol)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETEOLMODE, eol, 0);
		}
		return 0;
	}

#endif
	int GetTextLength(U32& length)
	{
		length = 0;
		if (IsWindow())
		{
			length = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
		}
		return 0;
	}

	int GetText(char* text, U32 buf_max)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_GETTEXT, buf_max, (LPARAM)text);
		}
		return 0;
	}

	int SetText(const char* text)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
		}
		return 0;
	}

	int AppendText(const char* text, U32 length)
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

	int SetWrapMode(int wrapmode)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, wrapmode, 0);
		}
		return 0;
	}

	int SetReadOnly(BOOL readonly = TRUE)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, EM_SETREADONLY, readonly, 0);
		}
		return 0;
	}

	int StyleSetFont(int style, const char* fontName)
	{
		if (IsWindow() && fontName)
		{
			::SendMessage(m_hWnd, SCI_STYLESETFONT, style, (LPARAM)fontName);
		}
		return 0;
	}

	int StyleSetFontSize(int style, int pointSize)
	{
		if (IsWindow())
		{
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, style, pointSize * 100);
		}
		return 0;
	}

	int GetCurrentPosition()
	{
		int r = 0;
		if (IsWindow())
		{
			r = (int)::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0);
		}
		return r;
	}

	char GetCharAtPosition(int pos)
	{
		char ch = '\0';
		if (IsWindow())
		{
			ch = (char)::SendMessage(m_hWnd, SCI_GETCHARAT, pos, 0);
		}
		return ch;
	}

	char GetLastChar()
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

	U8* GetInputData(U32& bytes, U8& offset, bool& shareScreen)
	{
		U8* p = nullptr;
		ZXConfig* cf = &ZXCONFIGURATION;

		if (IsWindow())
		{
			U32 real_len, len = 0;
			bool hasPrefix = false;
			bool ss = cf->property & AI_PROP_SSCREEN;
			len = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (len > INPUT_BUF_INPUT_MAX - 10)
				len = INPUT_BUF_INPUT_MAX - 10;

			p = m_inputBuf + 10;
			::SendMessage(m_hWnd, SCI_GETTEXT, len, (LPARAM)p);

			if (len > 3)
			{
				if (p[0] == '-' && p[1] == '-')
				{
					hasPrefix = true;
					ss = false;
					p += 2;
					len -= 2;
				}
				else if (p[0] == '+' && p[1] == '+')
				{
					hasPrefix = true;
					ss = true;
					p += 2;
					len -= 2;
				}
			}
			/* skip the white space */
			real_len = len;
			while (real_len && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
			{
				p++;
				real_len--;
			}

			if (real_len) /* the user does input some data */
			{
				offset = 6;
				U8* q = p - offset;
				if (hasPrefix)
				{
					offset = 9;
					q = p - offset;
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
					if (ss)
					{
						q[6] = '+'; q[7] = '+'; q[8] = ' ';
					}
					else
					{
						q[6] = '-'; q[7] = '-'; q[8] = ' ';
					}
				}
				else
				{
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
				}
				bytes = real_len + offset;
				shareScreen = ss;
				p = q;
			}
			else
			{
				p = nullptr;
			}
		}
		return p;
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
