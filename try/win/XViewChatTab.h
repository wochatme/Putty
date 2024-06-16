
#pragma once

#define XVIEWCT_BITMAP_EMPTYLOGN	0
#define XVIEWCT_BITMAP_EMPTYLOGH	1
#define XVIEWCT_BITMAP_EMPTYLOGP	2
#define XVIEWCT_BITMAP_EMPTYLOGA	3

#define XVIEWCT_BITMAP_SAVELOG_N	4
#define XVIEWCT_BITMAP_SAVELOG_H	5
#define XVIEWCT_BITMAP_SAVELOG_P	6
#define XVIEWCT_BITMAP_SAVELOG_A	7

#define XVIEWCT_BITMAP_AISETTINGN	8
#define XVIEWCT_BITMAP_AISETTINGH	9
#define XVIEWCT_BITMAP_AISETTINGP	10
#define XVIEWCT_BITMAP_AISETTINGA	11

#define XVIEWCT_BITMAP_TABAI_N		12
#define XVIEWCT_BITMAP_TABAI_H		13
#define XVIEWCT_BITMAP_TABAI_P		14
#define XVIEWCT_BITMAP_TABAI_A		15

#define XVIEWCT_BITMAP_TABKB_N		16
#define XVIEWCT_BITMAP_TABKB_H		17
#define XVIEWCT_BITMAP_TABKB_P		18
#define XVIEWCT_BITMAP_TABKB_A		19

#define XVIEWCT_BITMAP_SYNC_N		20
#define XVIEWCT_BITMAP_SYNC_H		21
#define XVIEWCT_BITMAP_SYNC_P		22
#define XVIEWCT_BITMAP_SYNC_A		23

#define XVIEWCT_BUTTON_EMPTYLOG		0
#define XVIEWCT_BUTTON_SAVELOG		1
#define XVIEWCT_BUTTON_AISETTING	2
#define XVIEWCT_BUTTON_TABAI		3
#define XVIEWCT_BUTTON_TABKB		4
#define XVIEWCT_BUTTON_SYNC			5

#define XVIEWCT_AI_IS_IN_LEFT			0
#define XVIEWCT_AI_IS_IN_RIGHT			1

#define WM_CHATTAB_MSG			WM_XWINDOWS03

class XViewChatTab : public XViewT<XViewChatTab>
{
	int m_posAI = XVIEWCT_AI_IS_IN_RIGHT;
public:
	XViewChatTab()
	{
		m_backgroundColor = 0xFFCDCDCD;
		m_property |= (DUI_PROP_MOVEWIN | DUI_PROP_BTNACTIVE);
		m_message = WM_CHATTAB_MSG;
	}
	~XViewChatTab() {}

public:
#if 0
	void SetAIPos(int posAI)
	{
		ATLASSERT(posAI == XVIEWCT_AI_IS_IN_LEFT || posAI == XVIEWCT_AI_IS_IN_RIGHT);
		m_posAI = posAI;
		UpdateControlPosition();
		InvalidateScreen();
	}
#endif

	void DoTabIsChanged(U16 tabId)
	{
		U8 id;
		XControl* xctl;
		ATLASSERT(tabId == XVIEWCT_BUTTON_TABAI || tabId == XVIEWCT_BUTTON_TABKB);
		if (tabId == XVIEWCT_BUTTON_TABAI)
		{
			id = XVIEWCT_BUTTON_EMPTYLOG;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_SAVELOG;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_AISETTING;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_SYNC;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);
		}
		else
		{
			id = XVIEWCT_BUTTON_EMPTYLOG;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_SAVELOG;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_AISETTING;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);

			id = XVIEWCT_BUTTON_SYNC;
			xctl = m_ctlArray[0][id];
			ATLASSERT(xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);
		}

		InvalidateScreen();
	}

	void UpdateControlPosition()
	{
		int sw, sh, dx, dy;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XControl* xctl;

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_TABAI];
		assert(nullptr != xctl);
		sw = xctl->getWidth();
		sh = xctl->getHeight();
		dy = h - sh;
		dx = 0;
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_TABKB];
		assert(nullptr != xctl);
		dx = sw;
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_AISETTING];
		assert(nullptr != xctl);
		sw = xctl->getWidth();
		sh = xctl->getHeight();
		dy = (h - sh) >> 1;
		dx = w - (dy + sw);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_SYNC];
		assert(nullptr != xctl);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_SAVELOG];
		assert(nullptr != xctl);
		dx = dx - (sw + (sw >> 1));
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWCT_BUTTON_EMPTYLOG];
		assert(nullptr != xctl);
		dx = dx - (sw + (sw >> 1));
		xctl->setPosition(dx, dy);
	}

private:
	void InitBitmap()
	{

		U8 id;
		XBitmap* bmp;

		int w = 16;
		int h = 16;

		id = XVIEWCT_BITMAP_EMPTYLOGN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpEmptyLogN;
		id = XVIEWCT_BITMAP_EMPTYLOGH;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpEmptyLogH;
		id = XVIEWCT_BITMAP_EMPTYLOGP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpEmptyLogP;

		id = XVIEWCT_BITMAP_SAVELOG_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSaveLogN;
		id = XVIEWCT_BITMAP_SAVELOG_P;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSaveLogP;

		id = XVIEWCT_BITMAP_AISETTINGN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAISettingN;
		id = XVIEWCT_BITMAP_AISETTINGH;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAISettingH;
		id = XVIEWCT_BITMAP_AISETTINGP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAISettingP;

		id = XVIEWCT_BITMAP_SYNC_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSyncN;
		id = XVIEWCT_BITMAP_SYNC_P;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSyncP;

		w = 162;
		h = 32;
		id = XVIEWCT_BITMAP_TABAI_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpTABAIN;
		id = XVIEWCT_BITMAP_TABAI_A;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpTABAIA;

		id = XVIEWCT_BITMAP_TABKB_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpTABKBN;
		id = XVIEWCT_BITMAP_TABKB_A;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpTABKBA;
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

		id = XVIEWCT_BUTTON_EMPTYLOG;
		objSize = sizeof(XButton);
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB0");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_EMPTYLOGN]);
			bmpH = &(m_bmpArray[0][XVIEWCT_BITMAP_EMPTYLOGH]);
			bmpP = &(m_bmpArray[0][XVIEWCT_BITMAP_EMPTYLOGP]);
			button->setBitmap(bmpN, bmpH, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCT_BUTTON_SAVELOG;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB1");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_SAVELOG_N]);
			bmpP = &(m_bmpArray[0][XVIEWCT_BITMAP_SAVELOG_P]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCT_BUTTON_AISETTING;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB2");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_AISETTINGN]);
			bmpH = &(m_bmpArray[0][XVIEWCT_BITMAP_AISETTINGH]);
			bmpP = &(m_bmpArray[0][XVIEWCT_BITMAP_AISETTINGP]);
			button->setBitmap(bmpN, bmpH, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCT_BUTTON_TABAI;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB3");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_TABAI_N]);
			bmpA = &(m_bmpArray[0][XVIEWCT_BITMAP_TABAI_A]);
			button->setBitmap(bmpN, bmpN, bmpA, bmpA);
			//button->setProperty(XCONTROL_PROP_ACTIVE);
			button->setProperty(XCONTROL_PROP_ACTIVE| XCONTROL_PROP_LBDOWN);
			button->setStatus(XCONTROL_STATE_ACTIVE);
			m_ctlArray[0][id] = button;
			m_activeControl = id; // we set talk as the first active button
		}
		else return;

		id = XVIEWCT_BUTTON_TABKB;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB4");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_TABKB_N]);
			bmpA = &(m_bmpArray[0][XVIEWCT_BITMAP_TABKB_A]);
			button->setBitmap(bmpN, bmpN, bmpA, bmpA);
			//button->setProperty(XCONTROL_PROP_ACTIVE);
			button->setProperty(XCONTROL_PROP_ACTIVE | XCONTROL_PROP_LBDOWN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCT_BUTTON_SYNC;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CTB5");
			bmpN = &(m_bmpArray[0][XVIEWCT_BITMAP_SYNC_N]);
			bmpP = &(m_bmpArray[0][XVIEWCT_BITMAP_SYNC_P]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			button->setProperty(XCONTROL_PROP_HIDDEN);
			m_ctlArray[0][id] = button;
		}
		else return;

		m_maxCtl[m_mode] = id + 1;
	}

#if 0
	int Do_DUI_PAINT(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) 
	{ 
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		ATLASSERT(w > 0);
		ATLASSERT(h > 0);
		wt_ScreenDrawRect(m_screen, w, h, (U32*)xbmpTabAL, 16, 32, 0, h - 32);
		wt_ScreenFillRect(m_screen, w, h, 0xFFFFFFFF, 64, 32, 16, h - 32);
		wt_ScreenDrawRect(m_screen, w, h, (U32*)xbmpAICloud, 32, 32, 16, h - 32);
		wt_ScreenDrawRect(m_screen, w, h, (U32*)xbmpDotRed, 8, 32, 72, h - 32);
		wt_ScreenDrawRect(m_screen, w, h, (U32*)xbmpTabAR, 16, 32, 80, h - 32);

		return 0; 
	}
#endif 
};

