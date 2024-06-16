
#pragma once

#define XVIEWSB_BITMAP_NETWORKSTATUSG	0
#define XVIEWSB_BITMAP_NETWORKSTATUSB	1
#define XVIEWSB_BITMAP_NOTEPADWINDOWN	2
#define XVIEWSB_BITMAP_NOTEPADWINDOWH	3
#define XVIEWSB_BITMAP_NOTEPADWINDOWP	4
#define XVIEWSB_BITMAP_NOTEPADWINDOWA	5
#define XVIEWSB_BITMAP_LAYOUTSWITCHRN	6
#define XVIEWSB_BITMAP_LAYOUTSWITCHRH	7
#define XVIEWSB_BITMAP_LAYOUTSWITCHRP	8
#define XVIEWSB_BITMAP_LAYOUTSWITCHRA	9
#define XVIEWSB_BITMAP_LAYOUTSWITCHLN	10
#define XVIEWSB_BITMAP_LAYOUTSWITCHLH	11
#define XVIEWSB_BITMAP_LAYOUTSWITCHLP	12
#define XVIEWSB_BITMAP_LAYOUTSWITCHLA	13
#define XVIEWSB_BITMAP_AICHATWINDOW_N	14
#define XVIEWSB_BITMAP_AICHATWINDOW_H	15
#define XVIEWSB_BITMAP_AICHATWINDOW_P	16
#define XVIEWSB_BITMAP_AICHATWINDOW_A	17
#define XVIEWSB_BITMAP_MAIN_MENU_N		18
#define XVIEWSB_BITMAP_MAIN_MENU_P		19

#define XVIEWSB_BUTTON_NETWORK_STATUS	0
#define XVIEWSB_BUTTON_NOTEPAD_WINDOW	1
#define XVIEWSB_BUTTON_LAYOUT_SWITCH	2
#define XVIEWSB_BUTTON_AI_CHAT_WINDOW	3
#define XVIEWSB_BUTTON_MAIN_MENU		4

#define WM_STATUSBAR_MSG		WM_XWINDOWS00
class XViewStatusBar : public XViewT<XViewStatusBar>
{
	bool m_networkIsGood = false;
public:
	XViewStatusBar()
	{
		m_backgroundColor = 0xFFF0F0F0;
		m_property |= (DUI_PROP_MOVEWIN);
		m_message = WM_STATUSBAR_MSG;
	}
	~XViewStatusBar() {}

public:
	void UpdateControlPosition()
	{
		int sw, sh, dx, dy;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XControl* xctl;

		xctl = m_ctlArray[0][XVIEWSB_BUTTON_MAIN_MENU];
		assert(nullptr != xctl);
		sw = xctl->getWidth();
		sh = xctl->getHeight();
		dx = dy = (h - sh) >> 1;
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWSB_BUTTON_NETWORK_STATUS];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWSB_BUTTON_NOTEPAD_WINDOW];
		assert(nullptr != xctl);
		dx = dx + sw + (sw>>1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWSB_BUTTON_LAYOUT_SWITCH];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWSB_BUTTON_AI_CHAT_WINDOW];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);
	}

private:
	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 16;
		int h = 16;

		id = XVIEWSB_BITMAP_NETWORKSTATUSG;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNetworkGood;
		id = XVIEWSB_BITMAP_NETWORKSTATUSB;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNetworkBad;

		id = XVIEWSB_BITMAP_NOTEPADWINDOWN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNotePadN;
		id = XVIEWSB_BITMAP_NOTEPADWINDOWH;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNotePadH;
		id = XVIEWSB_BITMAP_NOTEPADWINDOWP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNotePadP;

		id = XVIEWSB_BITMAP_LAYOUTSWITCHRN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpLayoutRN;
		id = XVIEWSB_BITMAP_LAYOUTSWITCHRH;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpLayoutRH;
		id = XVIEWSB_BITMAP_LAYOUTSWITCHRP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpLayoutRP;

		id = XVIEWSB_BITMAP_AICHATWINDOW_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAIN;
		id = XVIEWSB_BITMAP_AICHATWINDOW_P;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAIP;

		id = XVIEWSB_BITMAP_MAIN_MENU_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpMenuN;
		id = XVIEWSB_BITMAP_MAIN_MENU_P;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpMenuP;
	}

public:
	void InitControl()
	{
		U16 id;
		int	w, h;
		int left, top, sw, sh, T;
		U32 objSize;

		U8* mem;
		XBitmap* bmpN;
		XBitmap* bmpH;
		XBitmap* bmpP;
		XBitmap* bmpA;
		XButton* button;

		assert(nullptr != m_pool);
		InitBitmap(); // inital all bitmap resource

		m_mode = 0;
		m_maxCtl[m_mode] = 0;
		
		w = m_area.right - m_area.left;
		h = m_area.bottom - m_area.top;

		id = XVIEWSB_BUTTON_NETWORK_STATUS;
		objSize = sizeof(XButton);
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "SBB0");
			bmpN = &(m_bmpArray[0][XVIEWSB_BITMAP_NETWORKSTATUSB]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			button->setProperty(XCONTROL_PROP_STATIC);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWSB_BUTTON_NOTEPAD_WINDOW;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "SBB1");
			bmpN = &(m_bmpArray[0][XVIEWSB_BITMAP_NOTEPADWINDOWN]);
			bmpH = &(m_bmpArray[0][XVIEWSB_BITMAP_NOTEPADWINDOWH]);
			bmpP = &(m_bmpArray[0][XVIEWSB_BITMAP_NOTEPADWINDOWP]);
			button->setBitmap(bmpN, bmpH, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWSB_BUTTON_LAYOUT_SWITCH;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "SBB2");
			bmpN = &(m_bmpArray[0][XVIEWSB_BITMAP_LAYOUTSWITCHRN]);
			bmpH = &(m_bmpArray[0][XVIEWSB_BITMAP_LAYOUTSWITCHRH]);
			bmpP = &(m_bmpArray[0][XVIEWSB_BITMAP_LAYOUTSWITCHRP]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWSB_BUTTON_AI_CHAT_WINDOW;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "SBB2");
			bmpN = &(m_bmpArray[0][XVIEWSB_BITMAP_AICHATWINDOW_N]);
			bmpP = &(m_bmpArray[0][XVIEWSB_BITMAP_AICHATWINDOW_P]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWSB_BUTTON_MAIN_MENU;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "SBB2");
			bmpN = &(m_bmpArray[0][XVIEWSB_BITMAP_MAIN_MENU_N]);
			bmpP = &(m_bmpArray[0][XVIEWSB_BITMAP_MAIN_MENU_P]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		m_maxCtl[m_mode] = id + 1;
	}

#if 0
	int Do_DUI_TIMER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
	{
		int r = 0;

		bool isNetworkGood = g_NetworkStatus; // take a snapshot of this global variable
		if (isNetworkGood)
		{
			if (!m_networkIsGood)
				r++;
		}
		else
		{
			if (m_networkIsGood)
				r++;
		}
		m_networkIsGood = isNetworkGood;

		if (r)
		{
			XButton* btn = (XButton*)m_ctlArray[0][XWIN0_BUTTON_NETWORK];
			XBitmap* bmp = m_networkIsGood ? &(m_bmpArray[0][XWIN0_BITMAP_NETWORKN]) : &(m_bmpArray[0][XWIN0_BITMAP_NETWORKH]);
			btn->setBitmap(bmp, bmp, bmp, bmp);
		}
		return r;
	}
#endif
	void UpdateNetworkStatus(bool isGood)
	{
		XButton* btn = (XButton*)m_ctlArray[0][XVIEWSB_BUTTON_NETWORK_STATUS];
		ATLASSERT(btn);
		XBitmap* bmp = isGood ? &(m_bmpArray[0][XVIEWSB_BITMAP_NETWORKSTATUSG]) : &(m_bmpArray[0][XVIEWSB_BITMAP_NETWORKSTATUSB]);
		btn->setBitmap(bmp, bmp, bmp, bmp);
		InvalidateScreen();
	}
};

