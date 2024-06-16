
#pragma once

class XViewEditGap : public XViewT<XViewEditGap>
{
public:
	XViewEditGap()
	{
		m_backgroundColor = 0xFFF0F0F0;
		//m_backgroundColor = 0xFF0000FF;
		m_property |= (DUI_PROP_MOVEWIN);
		m_message = WM_XWINDOWS02;
	}
	~XViewEditGap() {}

public:
	void UpdateControlPosition()
	{
	}

private:
	void InitBitmap()
	{
	}

public:
	void InitControl()
	{
		m_mode = 0;
		m_maxCtl[m_mode] = 0;
	}

};

