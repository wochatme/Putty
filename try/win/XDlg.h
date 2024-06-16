// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
#if 0
class COpenURLDlg : public CDialogImpl<COpenURLDlg>
{
	WCHAR m_url[40] = { 0 };
public:
	enum { IDD = IDD_OPENURL };

	BEGIN_MSG_MAP(COpenURLDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HWND hWndCtl = GetDlgItem(IDC_EDIT_URL);
		if (::IsWindow(hWndCtl))
		{
			/* https://zterm.ai/t/0123456789abcdef*/
			::SendMessage(hWndCtl, EM_LIMITTEXT, 36, 0);
		}

		CenterWindow(GetParent());
		return TRUE;
	}

	WCHAR* GetURL()
	{
		return m_url;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hWndCtl = GetDlgItem(IDC_EDIT_URL);
		if (::IsWindow(hWndCtl))
		{
			::GetWindowTextW(hWndCtl, (LPWSTR)m_url, 36);
		}

		EndDialog(wID);
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
#endif 
static const WCHAR* errmsg = L"something is wrong!";

static const WCHAR* pxtype0 = L"0 - NO PROXY";
static const WCHAR* pxtype1 = L"1 - CURLPROXY_HTTP";
static const WCHAR* pxtype2 = L"2 - CURLPROXY_HTTPS";
static const WCHAR* pxtype3 = L"3 - CURLPROXY_HTTPS2";
static const WCHAR* pxtype4 = L"4 - CURLPROXY_HTTP_1_0";
static const WCHAR* pxtype5 = L"5 - CURLPROXY_SOCKS4";
static const WCHAR* pxtype6 = L"6 - CURLPROXY_SOCKS4A";
static const WCHAR* pxtype7 = L"7 - CURLPROXY_SOCKS5";
static const WCHAR* pxtype8 = L"8 - CURLPROXY_SOCKS5_HOSTNAME";

class ConfDlg : public CDialogImpl<ConfDlg>
{
	U8  m_font_Name[AI_FONTNAMELENGTH + 1] = { 0 };
	U8  m_font_Size = 0;
	U8  m_proxyType = 0;
public:
	enum { IDD = IDD_CONFIGURAION };

	BEGIN_MSG_MAP(ConfDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BTN_FONT, OnChangeFont)
		COMMAND_HANDLER(IDC_COMBO_NETWORK_PROXY_TYPE, CBN_SELCHANGE, OnProxyTypSelectionChange)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		U32 status, utf16len, utf8len;
		HWND hWndCtl;
		ZXConfig* cf = &ZXCONFIGURATION;
		WCHAR tmpstring[MAX_PATH] = { 0 };

		hWndCtl = GetDlgItem(IDC_EDIT_SERVER_URL);
		if (::IsWindow(hWndCtl))
		{
			::SendMessage(hWndCtl, EM_LIMITTEXT, AI_NET_URL_LENGTH, 0);
			utf8len = strlen((const char*)cf->serverURL);
			if (utf8len && utf8len < AI_NET_URL_LENGTH)
			{
				utf16len = 0;
				status = wt_UTF8ToUTF16(cf->serverURL, utf8len, (U16*)tmpstring, &utf16len);
				if (status == WT_OK && utf16len && utf16len < AI_NET_URL_LENGTH)
				{
					tmpstring[utf16len] = L'\0';
					::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
				}
				else
				{
					::SetWindowTextW(hWndCtl, (LPCWSTR)errmsg);
				}
			}
		}

		hWndCtl = GetDlgItem(IDC_EDIT_KBDATA);
		if (::IsWindow(hWndCtl))
		{
			::SendMessage(hWndCtl, EM_LIMITTEXT, AI_NET_URL_LENGTH, 0);
			utf8len = strlen((const char*)cf->kbdataURL);
			if (utf8len && utf8len < AI_NET_URL_LENGTH)
			{
				utf16len = 0;
				status = wt_UTF8ToUTF16(cf->kbdataURL, utf8len, (U16*)tmpstring, &utf16len);
				if (status == WT_OK && utf16len && utf16len < AI_NET_URL_LENGTH)
				{
					tmpstring[utf16len] = L'\0';
					::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
				}
				else
				{
					::SetWindowTextW(hWndCtl, (LPCWSTR)errmsg);
				}
			}
		}

		hWndCtl = GetDlgItem(IDC_EDIT_NETWORK_TIMEOUT);
		if (::IsWindow(hWndCtl))
		{
			::SendMessage(hWndCtl, EM_LIMITTEXT, 4, 0);
			swprintf((wchar_t*)tmpstring, MAX_PATH, L"%d", cf->networkTimout);
			::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
		}

		m_font_Size = cf->font_Size;
		memcpy_s(m_font_Name, AI_FONTNAMELENGTH, cf->font_Name, AI_FONTNAMELENGTH);
		m_font_Name[AI_FONTNAMELENGTH] = '\0';

		hWndCtl = GetDlgItem(IDC_STATIC_FONT);
		if (::IsWindow(hWndCtl))
		{
			utf8len = strlen((const char*)cf->font_Name);
			if (utf8len)
			{
				WCHAR fontname[128] = { 0 };
				wt_UTF8ToUTF16(cf->font_Name, utf8len, (U16*)fontname, NULL);
				swprintf((wchar_t*)tmpstring, MAX_PATH, L"%s - %d points", fontname, cf->font_Size);
				::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
			}
		}

		m_proxyType = cf->proxy_Type;
		hWndCtl = GetDlgItem(IDC_COMBO_NETWORK_PROXY_TYPE);
		if (::IsWindow(hWndCtl))
		{
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype0);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype1);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype2);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype3);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype4);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype5);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype6);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype7);
			::SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype8);

			ATLASSERT(cf->proxy_Type >= AI_CURLPROXY_NO_PROXY);
			ATLASSERT(cf->proxy_Type <= AI_CURLPROXY_TYPE_MAX);

			::SendMessage(hWndCtl, CB_SETCURSEL, 0, cf->proxy_Type);

			hWndCtl = GetDlgItem(IDC_EDIT_NETWORK_PROXY);
			if (::IsWindow(hWndCtl))
			{
				::SendMessage(hWndCtl, EM_LIMITTEXT, AI_NET_URL_LENGTH, 0);
				if (cf->proxy_Type == AI_CURLPROXY_NO_PROXY)
					::EnableWindow(hWndCtl, FALSE);
				else if (cf->proxy_Str[0]) // we have some value
				{
					utf8len = strlen((const char*)cf->proxy_Str);
					utf16len = 0;
					status = wt_UTF8ToUTF16(cf->proxy_Str, utf8len, (U16*)tmpstring, &utf16len);
					if (status == WT_OK)
					{
						tmpstring[utf16len] = L'\0';
						::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
					}
				}
			}
		}

		CheckDlgButton(IDC_CHECK_AI_STARTUP, (cf->property & AI_PROP_STARTUP) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK_AUTO_LOGGING, (cf->property & AI_PROP_AUTOLOG) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK_SHARE_SCREEN, (cf->property & AI_PROP_SSCREEN) ? BST_CHECKED : BST_UNCHECKED);
		
		hWndCtl = GetDlgItem(IDC_CHECK_AI_STARTUP);
		if (::IsWindow(hWndCtl))
		{
			::EnableWindow(hWndCtl, FALSE);
		}

		hWndCtl = GetDlgItem(IDC_EDIT_MY_PUBKEY);
		if (::IsWindow(hWndCtl))
		{
			wt_Raw2HexStringW(cf->my_pubKey, AI_PUB_KEY_LENGTH, tmpstring, NULL);
			tmpstring[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH] = L'\0';
			::SetWindowTextW(hWndCtl, (LPCWSTR)tmpstring);
			::SendMessage(hWndCtl, EM_SETREADONLY, TRUE, 0);
		}

		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
#if 0
	LRESULT OnFileLocation(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog dlg(nullptr, _T("Select Folder"), BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE);
		if (dlg.DoModal() == IDOK)
		{
			U32 utf16len = wcslen(dlg.m_szFolderPath);
			if (utf16len)
			{
				HWND hWndCtl = GetDlgItem(IDC_EDIT_DATA_DIRECTORY);
				if (::IsWindow(hWndCtl))
				{
					::SetWindowTextW(hWndCtl, (LPCWSTR)dlg.m_szFolderPath);
				}
			}
		}
		return 0;
	}
#endif 
	LRESULT OnChangeFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFontDialog dlg;
		if (dlg.DoModal() == IDOK)
		{
			U32 utf8len, utf16len, status;
			LOGFONTW lf = { 0 };
			dlg.GetCurrentFont(&lf);
			int fontSize = abs(lf.lfHeight);
			int pointSize = ::MulDiv(fontSize, 72, GetDeviceCaps(GetDC(), LOGPIXELSY));
			m_font_Size = pointSize;

			utf16len = (U32)wcslen(lf.lfFaceName);
			if (utf16len)
			{
				utf8len = 0;
				status = wt_UTF16ToUTF8((U16*)lf.lfFaceName, utf16len, NULL, &utf8len);
				if (status == WT_OK && utf8len < AI_FONTNAMELENGTH)
				{
					status = wt_UTF16ToUTF8((U16*)lf.lfFaceName, utf16len, m_font_Name, NULL);
					m_font_Name[utf8len] = '\0';
				}
			}
			utf8len = strlen((const char*)m_font_Name);
			if (utf8len)
			{
				WCHAR fontnameW[AI_FONTNAMELENGTH + 1] = { 0 };
				utf16len = 0;
				status = wt_UTF8ToUTF16((U8*)m_font_Name, utf8len, (U16*)fontnameW, &utf16len);
				fontnameW[utf16len] = L'\0';
				if (utf16len)
				{
					HWND hWndCtl = GetDlgItem(IDC_STATIC_FONT);
					if (::IsWindow(hWndCtl))
					{
						WCHAR fontInfo[128] = { 0 };
						swprintf((wchar_t*)fontInfo, 128, L"%s - %d points", fontnameW, m_font_Size);
						::SetWindowTextW(hWndCtl, (LPCWSTR)fontInfo);
					}
				}
			}
		}
		return 0;
	}

	LRESULT OnProxyTypSelectionChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CComboBox comboBox = GetDlgItem(IDC_COMBO_NETWORK_PROXY_TYPE);
		m_proxyType = comboBox.GetCurSel();
		ATLASSERT(m_proxyType >= AI_CURLPROXY_NO_PROXY);
		ATLASSERT(m_proxyType <= AI_CURLPROXY_TYPE_MAX);

		HWND hWndCtl = GetDlgItem(IDC_EDIT_NETWORK_PROXY);
		if (::IsWindow(hWndCtl))
		{
			if (m_proxyType == AI_CURLPROXY_NO_PROXY)
			{
				::SetWindowTextW(hWndCtl, (LPCWSTR)L"");
			}
			::EnableWindow(hWndCtl, m_proxyType != AI_CURLPROXY_NO_PROXY);
		}
		return 0;
	}

	/* the user pressed the OK button */
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		U32 status, utf16len, utf8len;
		HWND hWndCtl;
		ZXConfig* cf = &ZXCONFIGURATION;
		CButton btnCheck;
		WCHAR tmpstring[MAX_PATH] = { 0 };
		U8 utf8str[AI_NET_URL_LENGTH + AI_NET_URL_LENGTH] = { 0 };

		hWndCtl = GetDlgItem(IDC_EDIT_SERVER_URL);
		if (::IsWindow(hWndCtl))
		{
			::GetWindowTextW(hWndCtl, (LPWSTR)tmpstring, AI_NET_URL_LENGTH + 1);
			utf16len = wcslen(tmpstring);
			if (utf16len && utf16len < AI_NET_URL_LENGTH)
			{
				utf8len = 0;
				status = wt_UTF16ToUTF8((U16*)tmpstring, utf16len, utf8str, &utf8len);
				if (status == WT_OK && utf8len && utf8len < AI_NET_URL_LENGTH)
				{
					utf8str[utf8len] = '\0';
					if (memcmp(utf8str, cf->serverURL, utf8len))
					{
						memcpy_s(cf->serverURL, AI_NET_URL_LENGTH, utf8str, utf8len);
						cf->serverURL[utf8len] = '\0';
					}
				}
			}
		}

		hWndCtl = GetDlgItem(IDC_EDIT_KBDATA);
		if (::IsWindow(hWndCtl))
		{
			::GetWindowTextW(hWndCtl, (LPWSTR)tmpstring, AI_NET_URL_LENGTH + 1);
			utf16len = wcslen(tmpstring);
			if (utf16len && utf16len < AI_NET_URL_LENGTH)
			{
				utf8len = 0;
				status = wt_UTF16ToUTF8((U16*)tmpstring, utf16len, utf8str, &utf8len);
				if (status == WT_OK && utf8len && utf8len < AI_NET_URL_LENGTH)
				{
					utf8str[utf8len] = '\0';
					if (memcmp(utf8str, cf->kbdataURL, utf8len))
					{
						memcpy_s(cf->kbdataURL, AI_NET_URL_LENGTH, utf8str, utf8len);
						cf->kbdataURL[utf8len] = '\0';
					}
				}
			}
		}

		hWndCtl = GetDlgItem(IDC_EDIT_NETWORK_TIMEOUT);
		if (::IsWindow(hWndCtl))
		{
			::GetWindowTextW(hWndCtl, (LPWSTR)tmpstring, 4 + 1);
			utf16len = wcslen(tmpstring);
			if (utf16len == 0)
			{
				cf->networkTimout = AI_NETWORK_TIMEOUT;
			}
			else
			{
				int timeout = _wtoi(tmpstring);
				if (timeout < 5) timeout = 5;
				if (timeout > 3600) timeout = 3600;
				cf->networkTimout = (U16)timeout;
			}
		}

		cf->proxy_Type = m_proxyType;
		if (m_proxyType == AI_CURLPROXY_NO_PROXY)
		{
			cf->proxy_Str[0] = '\0';
		}
		else
		{
			hWndCtl = GetDlgItem(IDC_EDIT_NETWORK_PROXY);
			if (::IsWindow(hWndCtl))
			{
				::GetWindowTextW(hWndCtl, (LPWSTR)tmpstring, AI_NET_URL_LENGTH + 1);
				utf16len = wcslen(tmpstring);
				if (utf16len < AI_NET_URL_LENGTH)
				{
					utf8len = 0;
					status = wt_UTF16ToUTF8((U16*)tmpstring, utf16len, utf8str, &utf8len);
					if (status == WT_OK && utf8len && utf8len < AI_NET_URL_LENGTH)
					{
						utf8str[utf8len] = '\0';
						memcpy_s(cf->proxy_Str, AI_NET_URL_LENGTH, utf8str, utf8len);
					}
				}
			}
		}

		cf->font_Size = m_font_Size;
		memcpy_s(cf->font_Name, AI_FONTNAMELENGTH, m_font_Name, AI_FONTNAMELENGTH);
		cf->font_Name[AI_FONTNAMELENGTH] = '\0';

		btnCheck = GetDlgItem(IDC_CHECK_AI_STARTUP);
		if (btnCheck.GetCheck() == BST_CHECKED)
			cf->property |= AI_PROP_STARTUP;
		else
			cf->property &= ~AI_PROP_STARTUP;

		btnCheck = GetDlgItem(IDC_CHECK_AUTO_LOGGING);
		if (btnCheck.GetCheck() == BST_CHECKED)
			cf->property |= AI_PROP_AUTOLOG;
		else
			cf->property &= ~AI_PROP_AUTOLOG;

		btnCheck = GetDlgItem(IDC_CHECK_SHARE_SCREEN);
		if (btnCheck.GetCheck() == BST_CHECKED)
			cf->property |= AI_PROP_SSCREEN;
		else
			cf->property &= ~AI_PROP_SSCREEN;

		EndDialog(wID);
		return 0;
	}
};
