
#pragma once

#define XVIEWTB_BITMAP_KNOWLEGEBASE		0
#define XVIEWTB_BITMAP_TERMINAL_WIN		1
#define XVIEWTB_BITMAP_AICHATWINDOW		2
#define XVIEWTB_BITMAP_TTYCONFIGWIN		3
#define XVIEWTB_BITMAP_NOTE_PAD_WIN		4
#define XVIEWTB_BITMAP_INPUT_WINDOW		5
#define XVIEWTB_BITMAP_SWITCHLAYOUT		6
#define XVIEWTB_BITMAP_AI_CONFIGWIN		7

#define XVIEWTB_BUTTON_KNOWLEGEBASE		0
#define XVIEWTB_BUTTON_TERMINAL_WIN		1
#define XVIEWTB_BUTTON_AICHATWINDOW		2
#define XVIEWTB_BUTTON_TTYCONFIGWIN		3
#define XVIEWTB_BUTTON_NOTE_PAD_WIN		4
#define XVIEWTB_BUTTON_INPUT_WINDOW		5
#define XVIEWTB_BUTTON_SWITCHLAYOUT		6
#define XVIEWTB_BUTTON_AI_CONFIGWIN		7

#define WM_TOOLBAR_MSG		WM_XWINDOWS00

class XViewToolbar : public XViewT<XViewToolbar>
{
public:
	XViewToolbar()
	{
		m_backgroundColor = 0xFFF0F0F0;
		m_property |= (DUI_PROP_MOVEWIN);
		m_message = WM_TOOLBAR_MSG;
	}
	~XViewToolbar() {}

public:

	void UpdateControlPosition()
	{
#if 0
		int sw, sh, dx, dy;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XControl* xctl;

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_KNOWLEGEBASE];
		assert(nullptr != xctl);
		sw = xctl->getWidth();
		sh = xctl->getHeight();
		dy = (h - sh) >> 1;
		dx = 8;
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_TTYCONFIGWIN];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_TERMINAL_WIN];
		assert(nullptr != xctl);
		dx = dx + sw + (sw>>1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_AICHATWINDOW];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_NOTE_PAD_WIN];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_INPUT_WINDOW];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_SWITCHLAYOUT];
		assert(nullptr != xctl);
		dx = dx + sw + (sw >> 1);
		xctl->setPosition(dx, dy);

		xctl = m_ctlArray[0][XVIEWTB_BUTTON_AI_CONFIGWIN];
		assert(nullptr != xctl);
		sw = xctl->getWidth();
		sh = xctl->getHeight();
		dy = (h - sh) >> 1;
		dx = w - (sw + dy);
		xctl->setPosition(dx, dy);
#endif
	}

private:
	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 32;
		int h = 32;
#if 0
		id = XVIEWTB_BITMAP_KNOWLEGEBASE;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpKB;

		id = XVIEWTB_BITMAP_TERMINAL_WIN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpTTY;

		id = XVIEWTB_BITMAP_AICHATWINDOW;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAI;

		id = XVIEWTB_BITMAP_TTYCONFIGWIN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpGear;

		id = XVIEWTB_BITMAP_NOTE_PAD_WIN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpNotePad;

		id = XVIEWTB_BITMAP_INPUT_WINDOW;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpType;

		id = XVIEWTB_BITMAP_SWITCHLAYOUT;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSwitch;

		w = h = 24;
		id = XVIEWTB_BITMAP_AI_CONFIGWIN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpAISetting;

		id = XWIN0_BITMAP_FRIENDN;   bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->data = (U32*)xbmpFriendN;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDH;   bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->data = (U32*)xbmpFriendH;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDP;   bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->data = (U32*)xbmpFriendP;   bmp->w = w; bmp->h = h;
		id = XWIN0_BITMAP_FRIENDA;   bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->data = (U32*)xbmpFriendA;   bmp->w = w; bmp->h = h;
#endif 
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
		return;
#if 0
		w = m_area.right - m_area.left;
		h = m_area.bottom - m_area.top;

		id = XVIEWTB_BUTTON_KNOWLEGEBASE;
		objSize = sizeof(XButton);
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB0");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_KNOWLEGEBASE]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_TERMINAL_WIN;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB1");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_TERMINAL_WIN]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_AICHATWINDOW;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB2");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_AICHATWINDOW]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_TTYCONFIGWIN;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB3");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_TTYCONFIGWIN]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_NOTE_PAD_WIN;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB4");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_NOTE_PAD_WIN]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_INPUT_WINDOW;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB5");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_INPUT_WINDOW]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_SWITCHLAYOUT;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB6");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_SWITCHLAYOUT]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWTB_BUTTON_AI_CONFIGWIN;
		mem = (U8*)wt_palloc0(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "VTB7");
			bmpN = &(m_bmpArray[0][XVIEWTB_BITMAP_AI_CONFIGWIN]);
			button->setBitmap(bmpN, bmpN, bmpN, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;
		m_maxCtl[m_mode] = id + 1;
#endif
	}
};

