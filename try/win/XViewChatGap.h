
#pragma once

#define XVIEWCG_BITMAP_ASKQUESTIONN	0
#define XVIEWCG_BITMAP_ASKQUESTIONH	1
#define XVIEWCG_BITMAP_ASKQUESTIONP	2
#define XVIEWCG_BITMAP_ASKQUESTIONA	3

#define XVIEWCG_BITMAP_TOLEFTRIGHTN	4
#define XVIEWCG_BITMAP_TOLEFTRIGHTH	5
#define XVIEWCG_BITMAP_TOLEFTRIGHTP	6
#define XVIEWCG_BITMAP_TOLEFTRIGHTA	7

#define XVIEWCG_BITMAP_SEARCHKBT_N	8
#define XVIEWCG_BITMAP_SEARCHKBT_H	9
#define XVIEWCG_BITMAP_SEARCHKBT_P	10
#define XVIEWCG_BITMAP_SEARCHKBT_A	11

#define XVIEWCG_BUTTON_ASKQUESTION	0
#define XVIEWCG_BUTTON_TOLEFTRIGHT	1
#define XVIEWCG_BUTTON_SEARCHKB		2

#define AI_IS_IN_LEFT			0
#define AI_IS_IN_RIGHT			1

#define MODE_AICHAT				0
#define MODE_KBTREE				1

#define WM_CHATGAP_MSG			WM_XWINDOWS01

class XViewChatGap : public XViewT<XViewChatGap>
{
	int m_nAIPane = AI_IS_IN_RIGHT;
	int m_winMode = MODE_AICHAT;
public:
	XViewChatGap()
	{
		m_backgroundColor = 0xFFF0F0F0;
		m_property |= (DUI_PROP_MOVEWIN);
		m_message = WM_CHATGAP_MSG;
	}
	~XViewChatGap() {}

public:

	void SetWinMode(int mode, bool Show = true)
	{
		XControl* xctl;
		m_winMode = mode;
		if (m_winMode == MODE_AICHAT)
		{

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
			assert(nullptr != xctl);
			if(Show)
				xctl->clearProperty(XCONTROL_PROP_HIDDEN);
			else
				xctl->setProperty(XCONTROL_PROP_HIDDEN);
			xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
			assert(nullptr != xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);
		}
		else
		{
			xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
			assert(nullptr != xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);
			xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
			assert(nullptr != xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);
		}

		xctl = m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
		assert(nullptr != xctl);
		if(Show)
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);
		else
			xctl->setProperty(XCONTROL_PROP_HIDDEN);

		UpdateControlPosition();
		InvalidateScreen();
	}

	void SetAIPos(int nAIPane, bool Show = true)
	{
		XControl* xctl;
		ATLASSERT(nAIPane == AI_IS_IN_LEFT || nAIPane == AI_IS_IN_RIGHT);
		m_nAIPane = nAIPane;
		if (!Show)
		{
			if (m_winMode == MODE_AICHAT)
			{
				xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
				assert(nullptr != xctl);
				xctl->setProperty(XCONTROL_PROP_HIDDEN);
			}

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
			assert(nullptr != xctl);
			xctl->setProperty(XCONTROL_PROP_HIDDEN);
		}
		else
		{
			if (m_winMode == MODE_AICHAT)
			{
				xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
				assert(nullptr != xctl);
				xctl->clearProperty(XCONTROL_PROP_HIDDEN);
				xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
				assert(nullptr != xctl);
				xctl->setProperty(XCONTROL_PROP_HIDDEN);
			}
			else
			{
				xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
				assert(nullptr != xctl);
				xctl->setProperty(XCONTROL_PROP_HIDDEN);
				xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
				assert(nullptr != xctl);
				xctl->clearProperty(XCONTROL_PROP_HIDDEN);
			}

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
			assert(nullptr != xctl);
			xctl->clearProperty(XCONTROL_PROP_HIDDEN);
		}

		UpdateControlPosition();
		InvalidateScreen();
	}

	void UpdateControlPosition()
	{
		int sw, sh, dx, dy;
		int w = m_area.right - m_area.left;
		int h = m_area.bottom - m_area.top;
		XControl* xctl;

		XButton* button = (XButton*)m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
		XBitmap* bmpN = &(m_bmpArray[0][XVIEWCG_BITMAP_TOLEFTRIGHTN]);
		XBitmap* bmpP = &(m_bmpArray[0][XVIEWCG_BITMAP_TOLEFTRIGHTP]);
		bmpN->data = (m_nAIPane == AI_IS_IN_RIGHT) ? (U32*)xbmpToRightN : (U32*)xbmpToLeftN;
		bmpP->data = (m_nAIPane == AI_IS_IN_RIGHT) ? (U32*)xbmpToRightP : (U32*)xbmpToLeftP;
		button->setBitmap(bmpN, bmpN, bmpP, bmpN);

		if (m_nAIPane == AI_IS_IN_LEFT)
		{
			xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
			assert(nullptr != xctl);
			sw = xctl->getWidth();
			sh = xctl->getHeight();
			dy = (h - sh) >> 1;
			dx = w - (dy + sw);
			xctl->setPosition(dx, dy);

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
			assert(nullptr != xctl);
			xctl->setPosition(dx, dy);

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
			assert(nullptr != xctl);
			dx = 0;
			xctl->setPosition(dx, dy);
		}
		else
		{
			ATLASSERT(m_nAIPane == AI_IS_IN_RIGHT);
			xctl = m_ctlArray[0][XVIEWCG_BUTTON_ASKQUESTION];
			assert(nullptr != xctl);
			sw = xctl->getWidth();
			sh = xctl->getHeight();
			dx = dy = (h - sh) >> 1;
			xctl->setPosition(dx, dy);

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_SEARCHKB];
			assert(nullptr != xctl);
			xctl->setPosition(dx, dy);

			xctl = m_ctlArray[0][XVIEWCG_BUTTON_TOLEFTRIGHT];
			assert(nullptr != xctl);
			dx = w - sw;
			xctl->setPosition(dx, dy);
		}
	}

private:
	void InitBitmap()
	{
		U8 id;
		XBitmap* bmp;

		int w = 24;
		int h = 24;

		id = XVIEWCG_BITMAP_ASKQUESTIONN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpQuestionN;
		id = XVIEWCG_BITMAP_ASKQUESTIONP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpQuestionP;

		id = XVIEWCG_BITMAP_TOLEFTRIGHTN;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; 
		bmp->data = (m_nAIPane == AI_IS_IN_RIGHT) ? (U32*)xbmpToRightN : (U32*)xbmpToLeftN;

		id = XVIEWCG_BITMAP_TOLEFTRIGHTP;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h;
		bmp->data = (m_nAIPane == AI_IS_IN_RIGHT) ? (U32*)xbmpToRightP : (U32*)xbmpToLeftP;

		id = XVIEWCG_BITMAP_SEARCHKBT_N;
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSearchN;
		id = XVIEWCG_BITMAP_SEARCHKBT_H;
#if 0
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSearchH;
		id = XVIEWCG_BITMAP_SEARCHKBT_P;
#endif 
		bmp = &(m_bmpArray[0][id]); bmp->id = id; bmp->w = w; bmp->h = h; bmp->data = (U32*)xbmpSearchP;

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

		id = XVIEWCG_BUTTON_ASKQUESTION;
		objSize = sizeof(XButton);
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CGB0");
			bmpN = &(m_bmpArray[0][XVIEWCG_BITMAP_ASKQUESTIONN]);
			bmpP = &(m_bmpArray[0][XVIEWCG_BITMAP_ASKQUESTIONP]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCG_BUTTON_TOLEFTRIGHT;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CGB1");
			bmpN = &(m_bmpArray[0][XVIEWCG_BITMAP_TOLEFTRIGHTN]);
			bmpP = &(m_bmpArray[0][XVIEWCG_BITMAP_TOLEFTRIGHTP]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			m_ctlArray[0][id] = button;
		}
		else return;

		id = XVIEWCG_BUTTON_SEARCHKB;
		mem = (U8*)wt_palloc(m_pool, objSize);
		if (NULL != mem)
		{
			button = new(mem)XButton;
			assert(nullptr != button);
			button->Init(((m_mode << 8) | id), "CGB1");
			bmpN = &(m_bmpArray[0][XVIEWCG_BITMAP_SEARCHKBT_N]);
			//bmpH = &(m_bmpArray[0][XVIEWCG_BITMAP_SEARCHKBT_H]);
			bmpP = &(m_bmpArray[0][XVIEWCG_BITMAP_SEARCHKBT_P]);
			button->setBitmap(bmpN, bmpN, bmpP, bmpN);
			button->setProperty(XCONTROL_PROP_HIDDEN);
			m_ctlArray[0][id] = button;
		}
		else return;

		m_maxCtl[m_mode] = id + 1;
	}
};

