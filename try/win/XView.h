
#pragma once

#include "wt/wt_render.h"

#ifdef _MSC_VER
#define DUI_NO_VTABLE __declspec(novtable)
#else
#define DUI_NO_VTABLE
#endif

#define MESSAGEMAP_TABLE_SIZE           (1<<16)

extern U8 DUIMessageOSMap[MESSAGEMAP_TABLE_SIZE];

#define DUI_GLOBAL_STATE_IN_NONE_MODE   0x0000000000000000
#define DUI_GLOBAL_STATE_NEED_REDRAW    0x0000000000000001
#define DUI_GLOBAL_STATE_IN_DRAG_MODE   0x0000000000000002
#define DUI_GLOBAL_STATE_SET_CURSOR     0x0000000000000004
#define DUI_GLOBAL_STATE_INIT_FAILED    0x0000000000000008
#define DUI_GLOBAL_STATE_CTRL_PRESSED   0x0000000000000010
#define DUI_GLOBAL_STATE_SHIFT_PRESSED  0x0000000000000020
#define DUI_GLOBAL_STATE_ALT_PRESSED    0x0000000000000040
#define DUI_GLOBAL_STATE_LBUTTON_DOWN   0x0000000000000080

// this gloable variable is shared by all virtual windows so they can know each other
extern U64 dui_status;

#define DUIWindowLButtonDown()           (DUI_GLOBAL_STATE_LBUTTON_DOWN & dui_status)
#define SetDUIWindowLBtnDown()          do { dui_status |= DUI_GLOBAL_STATE_LBUTTON_DOWN;  } while(0)
#define ClearDUIWindowLBtnDown()        do { dui_status &= ~DUI_GLOBAL_STATE_LBUTTON_DOWN; } while(0)

#define DUIWindowInDragMode()           (DUI_GLOBAL_STATE_IN_DRAG_MODE & dui_status)
#define SetDUIWindowDragMode()          do { dui_status |= DUI_GLOBAL_STATE_IN_DRAG_MODE;  } while(0)
#define ClearDUIWindowDragMode()        do { dui_status &= ~DUI_GLOBAL_STATE_IN_DRAG_MODE; } while(0)

#define DUIWindowNeedReDraw()           (DUI_GLOBAL_STATE_NEED_REDRAW & dui_status)
#define InvalidateDUIWindow()           do { dui_status |= DUI_GLOBAL_STATE_NEED_REDRAW; } while(0)
#define ClearDUIWindowReDraw()          do { dui_status &= ~DUI_GLOBAL_STATE_NEED_REDRAW;} while(0)

#define DUIWindowCursorIsSet()          (DUI_GLOBAL_STATE_SET_CURSOR & dui_status)
#define SetDUIWindowCursor()            do { dui_status |= DUI_GLOBAL_STATE_SET_CURSOR; } while(0)
#define ClearDUIWindowCursor()          do { dui_status &= ~DUI_GLOBAL_STATE_SET_CURSOR;} while(0)

#define DUIWindowInitFailed()           (DUI_GLOBAL_STATE_INIT_FAILED & dui_status)
#define SetDUIWindowInitFailed()        do { dui_status |= DUI_GLOBAL_STATE_INIT_FAILED; } while(0)

#define DUIShiftKeyIsPressed()          (DUI_GLOBAL_STATE_SHIFT_PRESSED & dui_status)
#define DUICtrlKeyIsPressed()           (DUI_GLOBAL_STATE_CTRL_PRESSED & dui_status)
#define DUIAltKeyIsPressed()            (DUI_GLOBAL_STATE_ALT_PRESSED & dui_status)

#define SetDUIWindowShiftKey()          do { dui_status |= DUI_GLOBAL_STATE_SHIFT_PRESSED; } while(0)
#define SetDUIWindowCtrlKey()           do { dui_status |= DUI_GLOBAL_STATE_CTRL_PRESSED; } while(0)
#define SetDUIWindowAltKey()            do { dui_status |= DUI_GLOBAL_STATE_ALT_PRESSED; } while(0)

#define ClearDUIWindowShiftKey()        do { dui_status &= ~DUI_GLOBAL_STATE_SHIFT_PRESSED; } while(0)
#define ClearDUIWindowCtrlKey()         do { dui_status &= ~DUI_GLOBAL_STATE_CTRL_PRESSED; } while(0)
#define ClearDUIWindowAltKey()          do { dui_status &= ~DUI_GLOBAL_STATE_ALT_PRESSED; } while(0)

extern HCURSOR	dui_hCursorArrow;
extern HCURSOR	dui_hCursorHand;

#define DUI_DEBUG	1
#define DUI_OK      0
#define DUI_FAILED  0xFF

typedef struct tagXPOINT
{
    int  x;
    int  y;
} XPOINT;

typedef struct tagXSIZE
{
    int  cx;
    int  cy;
} XSIZE;

typedef struct tagXRECT
{
    int    left;
    int    top;
    int    right;
    int    bottom;
} XRECT;

// a pure 32-bit true color bitmap object
typedef struct XBitmap
{
    U8    id;
    U32* data;
    int   w;
    int   h;
} XBitmap;

// determin if one object is hitted
#define XWinPointInRect(x, y, OBJ)  \
        (((x) >= ((OBJ)->left)) && ((x) < ((OBJ)->right)) && ((y) >= ((OBJ)->top)) && ((y) < ((OBJ)->bottom)))

#define XMOUSE_NULL         0
#define XMOUSE_MOVE         1
#define XMOUSE_LBDOWN       2
#define XMOUSE_LBUP         3
#define XMOUSE_RBDOWN       4
#define XMOUSE_RBUP         5

#define XKEYBOARD_NORMAL    0x0000
#define XKEYBOARD_CTRL      0x0001
#define XKEYBOARD_SHIF      0x0002
#define XKEYBOARD_ALT       0x0004

enum XControlProperty
{
    XCONTROL_PROP_NONE     = 0x00000000,
    XCONTROL_PROP_ROUND    = 0x00000001,
    XCONTROL_PROP_STATIC   = 0x00000002,
    XCONTROL_PROP_ACTIVE   = 0x00000004,
    XCONTROL_PROP_KEYBOARD = 0x00000008,
    XCONTROL_PROP_EDITBOX  = 0x00000010,
    XCONTROL_PROP_TEXT     = 0x00000020,
    XCONTROL_PROP_HIDDEN   = 0x00000040,
    XCONTROL_PROP_FOCUS    = 0x00000080,
    XCONTROL_PROP_LBDOWN   = 0x00000100,
    XCONTROL_PROP_RBUP     = 0x00000200
};

enum XControlState
{
    XCONTROL_STATE_HIDDEN = 0,
    XCONTROL_STATE_NORMAL,
    XCONTROL_STATE_HOVERED,
    XCONTROL_STATE_PRESSED,
    XCONTROL_STATE_ACTIVE
};

typedef void* DUI_Surface;
typedef void* DUI_TextFactory;
typedef void* DUI_TextFormat;
typedef void* DUI_Brush;

// the generic control class
class XControl
{
public:
    U16   m_Id = 0;
protected:
    U8   m_Name[16 + 1] = { 0 };

    // each control has two sizes: inner and outer.
    // for Button/Label, they are the same.
    // for Editbox with vertical scrollbar, they are not the same
    // inner size
    int  m_left = 0;
    int  m_top = 0;
    int  m_right = 0;
    int  m_bottom = 0;

    U32  m_property = XCONTROL_PROP_NONE;
    U32  m_status = XCONTROL_STATE_NORMAL;
    U32  m_statusPrev = XCONTROL_STATE_NORMAL;

    U32* m_parentBuf = nullptr; // the draw screen buffer of the parent
    int  m_parentL = 0; // the left position of the parent
    int  m_parentT = 0; // the top position of the parent
    int  m_parentR = 0; // the right position of the parent
    int  m_parentB = 0; // the bottom position of the parent

    U32  m_Color0 = 0xFFFFFFFF;
    U32  m_Color1 = 0xFFFFFFFF;

    wchar_t  m_tipMessage[32];

    void* m_Cursor1 = nullptr;  // the cursor when the mouse is over the inner part
    void* m_Cursor2 = nullptr;  // the cursor when the mousr is over the outer part

public:
    int DoMouseMove(int x, int y, int idxActive)
    {
        int r = 0;

        if (DUIWindowLButtonDown()) // the mouse is moving when the mouse left button is pressed
            return 0;

        if (x < 0) // fast path, for the most controls. The mouse is not hitting me
        {
            if (m_status != XCONTROL_STATE_NORMAL && m_status != XCONTROL_STATE_ACTIVE)
                r++;
            m_status = (idxActive != m_Id) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;
            return r;
        }

        // the mousr point is in the area. In this case, we should set the status to hovered.
        if ((x >= m_left) && (y >= m_top) && (x < m_right) && (y < m_bottom))
        {
            if ((m_property & XCONTROL_PROP_ACTIVE) == 0)
            {
                r = (m_status == XCONTROL_STATE_HOVERED) ? 0 : 1;
                m_status = XCONTROL_STATE_HOVERED;
            }
            ShowCursor();
            return r;
        }

        // the mousr is not hovered me
        if (m_status != XCONTROL_STATE_NORMAL && m_status != XCONTROL_STATE_ACTIVE)
            r++;

        m_status = (idxActive != m_Id) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;
        return 0;
    }

    int DoMouseLBClickDown(int x, int y, int* idxActive = nullptr)
    {
        int r = 0;
        U32 status = m_status;
        U32 prop = (XCONTROL_PROP_EDITBOX | XCONTROL_PROP_FOCUS) & m_property;

        if (x < 0) // fast path, for the most controls. The mouse is not hitting me
        {
            // if the old status is normal or active, we do not need to redraw
            r = (m_status == XCONTROL_STATE_NORMAL || m_status == XCONTROL_STATE_ACTIVE) ? 0 : 1;
            m_status = (status != XCONTROL_STATE_ACTIVE) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;

            if (prop == (XCONTROL_PROP_EDITBOX | XCONTROL_PROP_FOCUS)) // the edit box has focus, now we lose the focus
                r++;

            m_property &= ~XCONTROL_PROP_FOCUS;
            return r;
        }

        // the mousr point is in the area. In this case, we should set the status to hovered.
        if ((x >= m_left) && (y >= m_top) && (x < m_right) && (y < m_bottom))
        {
            r = (m_status == XCONTROL_STATE_PRESSED) ? 0 : 1;
            m_status = XCONTROL_STATE_PRESSED;
            m_property |= XCONTROL_PROP_FOCUS;
            if (idxActive)
                *idxActive = m_Id;
            ShowCursor();
            return r;
        }
        // the mousr is not hovered me
        // if the old status is normal or active, we do not need to redraw
        r = (m_status == XCONTROL_STATE_NORMAL || m_status == XCONTROL_STATE_ACTIVE) ? 0 : 1;
        m_status = (status != XCONTROL_STATE_ACTIVE) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;

        if (prop == (XCONTROL_PROP_EDITBOX | XCONTROL_PROP_FOCUS))
            r++;
        m_property &= ~XCONTROL_PROP_FOCUS;

        return r;
    }

    int DoMouseLBClickUp(int x, int y, int* idxActive = nullptr)
    {
        int r = 0;
        U32 status = m_status;

        if (x < 0) // fast path, for the most controls
        {
            // if the old status is normal or active, we do not need to redraw
            r = (m_status == XCONTROL_STATE_NORMAL || m_status == XCONTROL_STATE_ACTIVE) ? 0 : 1;
            m_status = (status != XCONTROL_STATE_ACTIVE) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;
            m_property &= ~XCONTROL_PROP_FOCUS;
            return r;
        }

        // the mousr point is in the area. In this case, we should set the status to hovered.
        if ((x >= m_left) && (y >= m_top) && (x < m_right) && (y < m_bottom))
        {
            r = (m_status == XCONTROL_STATE_HOVERED) ? 0 : 1;
            m_status = XCONTROL_STATE_HOVERED;
            if (idxActive)
                *idxActive = m_Id;
            ShowCursor();
            return r;
        }
        // the mousr is not hovered me
        // if the old status is normal or active, we do not need to redraw
        r = (m_status == XCONTROL_STATE_NORMAL || m_status == XCONTROL_STATE_ACTIVE) ? 0 : 1;
        m_status = (status != XCONTROL_STATE_ACTIVE) ? XCONTROL_STATE_NORMAL : XCONTROL_STATE_ACTIVE;
        return r;
    }

    int DoMouseRBClickDown(int x, int y, int* idxActive = nullptr)
    {
        return 0;
    }

    int DoMouseRBClickUp(int x, int y, int absX, int absY, HWND hWnd)
    {
        return 0;
    }

    int DoMouseLeave()
    {
        int r = 0;

        if (m_status != XCONTROL_STATE_NORMAL && m_status != XCONTROL_STATE_ACTIVE)
            r++;
        if (m_status != XCONTROL_STATE_ACTIVE)
            m_status = XCONTROL_STATE_NORMAL;

        return r;
    }

    int DoMouseHover(int x, int y)
    {
        int r = -1;

        if ((x >= m_left) && (y >= m_top) && (x < m_right) && (y < m_bottom))
        {
            r = m_Id;
        }
        return r;
    }

    int ShowCursor()
    {
        int ret = 0;
        if (!(XCONTROL_PROP_STATIC & m_property))
        {
            if (nullptr != m_Cursor1)
            {
                ::SetCursor((HCURSOR)m_Cursor1);
                SetDUIWindowCursor();
                ret = 1;
            }
        }
        return ret;
    }

    void setRoundColor(U32 c0, U32 c1)
    {
        m_Color0 = c0;
        m_Color1 = c1;
    }

    int getLeft()
    {
        return (m_left);
    }

    int getRight()
    {
        return (m_right);
    }

    int getTop()
    {
        return (m_top);
    }

    int getBottom()
    {
        return (m_bottom);
    }

    int getWidth()
    {
        assert(m_right >= m_left);
        return (m_right - m_left);
    }

    int getHeight()
    {
        assert(m_bottom >= m_top);
        return (m_bottom - m_top);
    }

    void setSize(int width, int height)
    {
        assert(width >= 0);
        assert(height >= 0);
        m_left = m_top = 0;
        m_right = width;
        m_bottom = height;
    }

    void setPosition(int left_, int top_, int right_, int bottom_)
    {
#if 0
        assert(left_ >= 0);
        assert(top_ >= 0);
        assert(right_ >= left_);
        assert(bottom_ >= top_);
#endif 
        m_left = left_;
        m_top = top_;
        m_right = right_;
        m_bottom = bottom_;

        AfterPositionIsChanged();
    }

    void setPosition(int left_, int top_)
    {
        int w = m_right - m_left;
        int h = m_bottom - m_top;

        assert(w >= 0);
        assert(h >= 0);

        m_left = left_;
        m_top = top_;
        m_right = m_left + w;
        m_bottom = m_top + h;

        AfterPositionIsChanged();
    }

    void setProperty(U32 property)
    {
        m_property |= property;
    }

    int clearProperty(U32 property)
    {
        int r = 0;

        r = (m_property & property) ? 1 : 0; // if property is exist, then we need to redraw

        m_property &= (~property);

        return r;
    }

    U32 getProperty()
    {
        return m_property;
    }

    void AttachParent(U32* parentBuf, int left, int top, int right, int bottom)
    {
        m_parentBuf = parentBuf;
        m_parentL = left;
        m_parentT = top;
        m_parentR = right;
        m_parentB = bottom;
    }

    bool IsVisible()
    {
        return (0 == (XCONTROL_PROP_HIDDEN & m_property));
    }

    void HideMe(bool hide = true)
    {
        if (hide)
            m_property |= XCONTROL_PROP_HIDDEN;
        else
            m_property &= ~XCONTROL_PROP_HIDDEN;
    }

    bool IsOverMe(int xPos, int yPos)
    {
        bool bRet = false;
        if (!(XCONTROL_PROP_STATIC & m_property)) // this is not a static control
        {
            if (xPos >= m_left && xPos < m_right && yPos >= m_top && yPos < m_bottom)
            {
                bRet = true;
            }
        }
        return bRet;
    }

    int setStatus(U32 newStatus, U8 mouse_event = XMOUSE_NULL)
    {
        int r = 0;

        if (!(XCONTROL_PROP_STATIC & m_property)) // the static control cannot change the status
        {
            r = (m_status != newStatus) ? 1 : 0;
            m_statusPrev = m_status;
            m_status = newStatus;
        }
        return r;
    }

    U32 getStatus()
    {
        return m_status;
    }

    int Init(U16 id, const char* name, void* ptr0 = nullptr, void* ptr1 = nullptr)
    {
        size_t len = strlen(name);

        m_Id = id;

        if (len > 16)
            len = 16;

        if (len > 0)
        {
            size_t i;
            char* p = (char*)name;
            for (i = 0; i < len; i++)
                m_Name[i] = *p++;
            m_Name[i] = '\0';
        }
        else m_Name[0] = '\0';

        return InitControl(ptr0, ptr1);
    }

    virtual int OnTimer() { return 0; }
    virtual int AfterPositionIsChanged(bool inner = true) { return 0; }
    virtual void Term() {}
    virtual int Draw(int dx = 0, int dy = 0) { return 0; }
    virtual int OnKeyBoard(U32 msg, U64 wparam = 0, U64 lparam = 0) { return 0; }

    virtual int DrawText(int dx, int dy,
        DUI_Surface surface = nullptr,
        DUI_Brush brush = nullptr,
        DUI_Brush brushSel = nullptr,
        DUI_Brush brushCaret = nullptr, U64 flag = 0)
    {
        return 0;
    };

    virtual int InitControl(void* ptr0 = nullptr, void* ptr1 = nullptr) = 0;

    int (*pfAction) (void* obj, U32 uMsg, U64 wParam, U64 lParam);
};

class XButton : public XControl
{
public:
    // all XBitmpas should have extactly the same size
    XBitmap* imgNormal;
    XBitmap* imgHover;
    XBitmap* imgPress;
    XBitmap* imgActive;
    int Draw(int dx = 0, int dy = 0)
    {
        if (nullptr != m_parentBuf)
        {
            U32* dst = m_parentBuf;
            U32* src;
            int w = m_right - m_left;
            int h = m_bottom - m_top;
            XBitmap* bitmap;

            assert(w > 0 && h > 0);
            assert(m_parentR - m_parentL > w);
            assert(m_parentB - m_parentT > h);
            switch (m_status)
            {
            case XCONTROL_STATE_PRESSED:
                bitmap = imgPress;
                break;
            case XCONTROL_STATE_HOVERED:
                bitmap = imgHover;
                break;
            case XCONTROL_STATE_ACTIVE:
                bitmap = imgActive;
                break;
            default:
                bitmap = imgNormal;
                break;
            }
            assert(nullptr != bitmap);
            assert(w == bitmap->w);
            assert(h == bitmap->h);

            src = bitmap->data;
            assert(nullptr != src);

            if (XCONTROL_PROP_ROUND & m_property)
                wt_ScreenDrawRectRound(dst, m_parentR - m_parentL, m_parentB - m_parentT, 
                    src, bitmap->w, bitmap->h, m_left, m_top, m_Color0, m_Color1);
            else
                wt_ScreenDrawRect(dst, m_parentR - m_parentL, m_parentB - m_parentT, src, bitmap->w, bitmap->h, m_left, m_top);
        }
        return 0;
    }

    int InitControl(void* ptr0 = nullptr, void* ptr1 = nullptr)
    {
        m_Cursor1 = dui_hCursorHand;
        m_Cursor2 = dui_hCursorArrow;
        return 0;
    }

    void setBitmap(XBitmap* n, XBitmap* h, XBitmap* p, XBitmap* a)
    {
        assert(nullptr != n);
        assert(nullptr != h);
        assert(nullptr != p);
        assert(nullptr != a);
        imgNormal = n; imgHover = h; imgPress = p; imgActive = a;

        if (0 == m_left && 0 == m_top && 0 == m_right && 0 == m_bottom)
            setSize(imgNormal->w, imgNormal->h);
    }

};

#define DUI_MAX_LABEL_STRING    128
class XLabel : public XControl
{
private:
    U32  m_bkgColor = 0xFFFFFFFF;
    U32  m_caretAnchor = 0;
    U32  m_caretPosition = 0;
    U32  m_caretPositionOffset = 0;
    U16  m_Text[DUI_MAX_LABEL_STRING + 1] = { 0 };
    U16  m_TextLen = 0;
    IDWriteFactory* m_pTextFactory = nullptr;
    IDWriteTextFormat* m_pTextFormat = nullptr;
    IDWriteTextLayout* m_pTextLayout = nullptr;
public:
    XLabel()
    {
        m_property = XCONTROL_PROP_TEXT;
    }

    int DrawText(int dx, int dy,
        DUI_Surface surface = nullptr,
        DUI_Brush brush = nullptr,
        DUI_Brush brushSel = nullptr,
        DUI_Brush brushCaret = nullptr, U64 flag = 0)
    {
        return 0;
    }

    int InitControl(void* ptr0 = nullptr, void* ptr1 = nullptr)
    {
        m_pTextFactory = static_cast<IDWriteFactory*>(ptr0);
        m_pTextFormat = static_cast<IDWriteTextFormat*>(ptr1);
        return 0;
    }

    void setText(U16* text, U16 len = 0)
    {
    }

};

// DUI Message that is OS independent;
#define DUI_ALLMESSAGE          0x00
#define DUI_NULL                0x00
#define DUI_CREATE              0x01
#define DUI_DESTROY             0x02
#define DUI_ERASEBKGND          0x03
#define DUI_PAINT         	    0x04
#define DUI_TIMER               0x05
#define DUI_MOUSEMOVE           0x06
#define DUI_MOUSEWHEEL          0x07
#define DUI_MOUSEFIRST          0x08
#define DUI_LBUTTONDOWN         0x0A
#define DUI_LBUTTONUP           0x0B
#define DUI_LBUTTONDBLCLK       0x0C
#define DUI_RBUTTONDOWN         0x0D
#define DUI_RBUTTONUP           0x0E
#define DUI_RBUTTONDBLCLK       0x0F
#define DUI_MBUTTONDOWN         0x10
#define DUI_MBUTTONUP           0x11
#define DUI_MBUTTONDBLCLK       0x12
#define DUI_CAPTURECHANGED      0x13
#define DUI_KEYFIRST            0x14
#define DUI_KEYDOWN             0x15
#define DUI_KEYUP               0x16
#define DUI_CHAR                0x17
#define DUI_DEADCHAR            0x18
#define DUI_SYSKEYDOWN          0x19
#define DUI_SYSKEYUP            0x1A
#define DUI_SYSCHAR             0x1B
#define DUI_SYSDEADCHAR         0x1C
#define DUI_GETMINMAXINFO       0x1D
#define DUI_SIZE                0x1E
#define DUI_SETCURSOR           0x1F
#define DUI_SETFOCUS            0x20
#define DUI_MOUSEACTIVATE       0x21
#define DUI_MOUSELEAVE          0x22
#define DUI_MOUSEHOVER          0x23
#define DUI_NCLBUTTONDOWN       0x24

// User defined message
#define DUI_XWINDOWS00          0xFF
#define DUI_XWINDOWS01          0xFE
#define DUI_XWINDOWS02          0xFD
#define DUI_XWINDOWS03          0xFC
#define DUI_XWINDOWS04          0xFB
#define DUI_XWINDOWS05          0xFA
#define DUI_XWINDOWS06          0xF9
#define DUI_XWINDOWS07          0xF8
#define DUI_XWINDOWS08          0xF7
#define DUI_XWINDOWS09          0xF6

#ifdef _WIN32
#define WM_XWINDOWSXX       (WM_USER + DUI_NULL)
#define WM_XWINDOWS00       (WM_USER + DUI_XWINDOWS00)
#define WM_XWINDOWS01       (WM_USER + DUI_XWINDOWS01)
#define WM_XWINDOWS02       (WM_USER + DUI_XWINDOWS02)
#define WM_XWINDOWS03       (WM_USER + DUI_XWINDOWS03)
#define WM_XWINDOWS04       (WM_USER + DUI_XWINDOWS04)
#define WM_XWINDOWS05       (WM_USER + DUI_XWINDOWS05)
#define WM_XWINDOWS06       (WM_USER + DUI_XWINDOWS06)
#define WM_XWINDOWS07       (WM_USER + DUI_XWINDOWS07)
#define WM_XWINDOWS08       (WM_USER + DUI_XWINDOWS08)
#define WM_XWINDOWS09       (WM_USER + DUI_XWINDOWS09)
#define WM_XWINDOWSPAINT    (WM_USER + DUI_PAINT)
#endif 

#define DUI_KEY_RETURN      0x0D

#define DUI_MINIMAL_THUMB_SIZE          64
#define DUI_ALLOCSET_DEFAULT_INITSIZE   (8 * 1024)
#define DUI_ALLOCSET_DEFAULT_MAXSIZE    (8 * 1024 * 1024)
#define DUI_ALLOCSET_SMALL_INITSIZE     (1 * 1024)
#define DUI_ALLOCSET_SMALL_MAXSIZE	    (8 * 1024)

#define DUI_MODE0       0
#define DUI_MODE1       1
#define DUI_MODE2       2
#define DUI_MODE3       3
#define DUI_MODE4       4
#define DUI_MODE5       5
#define DUI_MODE6       6
#define DUI_MODE7       7
#define DUI_MODETOTAL   8

#define DUI_MAX_CONTROLS            16 
#define DUI_MAX_BUTTON_BITMAPS      (DUI_MAX_CONTROLS << 2)

enum
{
    DUI_STATUS_NODRAW = 0x00000000,    // do not need to draw, any value not zero need redraw      
    DUI_STATUS_NEEDRAW = 0x00000001,    // does this virtual windows need to be redraw?
    DUI_STATUS_VSCROLL = 0x00000002,    // is this virtual window has vertical scroll bar?
    DUI_STATUS_HSCROLL = 0x00000004,    // is this virtual window has horizonal scroll bar?
    DUI_STATUS_ISFOCUS = 0x00000008,    // is the keyboard input redirected into this virutal window?
    DUI_STATUS_VISIBLE = 0x00000010    // is this virtual window visible?
};

enum
{
    DUI_PROP_NONE = 0x00000000,   // None Properties
    DUI_PROP_MOVEWIN = 0x00000001,   // Move the whole window while LButton is pressed
    DUI_PROP_BTNACTIVE = 0x00000002,   // have active button on this virtual window
    DUI_PROP_HASVSCROLL = 0x00000004,    // have vertical scroll bar
    DUI_PROP_HASHSCROLL = 0x00000008,
    DUI_PROP_HANDLEWHEEL = 0x00000010,   // does this window need to handle mouse wheel?
    DUI_PROP_HANDLETEXT = 0x00000020,
    DUI_PROP_HANDLETIMER = 0x00000040,
    DUI_PROP_HANDLEKEYBOARD = 0x00000080,
    DUI_PROP_LARGEMEMPOOL = 0x00000100,
    DUI_PROP_NONEMEMPOOL = 0x00000200
};

enum
{
    DEFAULT_BORDER_COLOR = 0xFFBBBBBB,
    DEFAULT_SCROLLBKG_COLOR = 0xFFF9F3F1,
    DEFAULT_SCROLLTHUMB_COLOR = 0xFFC0C1C4,
    DEFAULT_SCROLLTHUMB_COLORA = 0xFFAAABAD,
    DEFAULT_BACKGROUND_COLOR = 0xFFF0F0F0
};


template <class T>
class DUI_NO_VTABLE XViewT
{
private:
    enum class XDragMode
    {
        DragNone,
        DragVertical,
        DragHorizonal
    };

    XDragMode  m_DragMode = XDragMode::DragNone;

public:
    //typedef int (T::*ProcessOSMessage)(U32 uMsg, U64 wParam, U64 lParam, void* lpData);

#ifdef _WIN32
    HWND m_hWnd = nullptr;  // the pointer to point the host real/platform window
#else
    void* m_hWnd = nullptr;
#endif
    U32*    m_screen = nullptr;  // the memory block that will be rendered to the screen by the platform
    U32     m_size = 0;
    U8      m_Id[8] = { 0 }; // for debugging 
    XRECT   m_area = { 0 };  // the area of this window in the client area of parent window

    MemoryPoolContext m_pool = nullptr;

    XControl* m_ctlArray[DUI_MODETOTAL][DUI_MAX_CONTROLS];
    XBitmap   m_bmpArray[DUI_MODETOTAL][DUI_MAX_BUTTON_BITMAPS];
    int       m_maxCtl[DUI_MODETOTAL];

    int m_activeControl = -1;

    //ProcessOSMessage m_messageFuncPointerTab[256] = { 0 };  // we only handle 255 messages that should be enough

    const int m_scrollWidth = 8; // the scrollbar width in pixel

    XPOINT m_ptOffset = { 0 };
    XPOINT m_ptOffsetOld = { 0 };
    XSIZE  m_sizeAll = { 0 };
    XSIZE  m_sizeLine = { 0 };
    XSIZE  m_sizePage = { 0 };
    int    m_cxyDragOffset = 0;

    U32  m_status = DUI_STATUS_VISIBLE;
    U32  m_property = DUI_PROP_NONE;
    U32  m_message = DUI_NULL;
    U16  m_mode = DUI_MODE0;

    U32  m_backgroundColor = DEFAULT_BACKGROUND_COLOR;
    U32  m_scrollbarColor = DEFAULT_SCROLLBKG_COLOR;
    U32  m_thumbColor = DEFAULT_SCROLLTHUMB_COLOR;

public:
    XViewT()
    {
        int i, j;
        XBitmap* p;

        for (i = 0; i < DUI_MODETOTAL; i++)
            m_maxCtl[i] = 0;

        for (j = 0; j < DUI_MODETOTAL; j++)
            for (i = 0; i < DUI_MAX_CONTROLS; i++)
                m_ctlArray[j][i] = nullptr;

        for (j = 0; j < DUI_MODETOTAL; j++)
            for (i = 0; i < DUI_MAX_BUTTON_BITMAPS; i++)
            {
                p = &(m_bmpArray[j][i]);
                p->id = p->w = p->h = 0;
                p->data = nullptr;
            }

#if 0        
        for (int i = 0; i < DUI_MAX_CONTROLS; i++)
            m_controlArray[i] = nullptr;

        //m_messageFuncPointerTab[DUI_NULL] = nullptr;
        for (int i = 0; i < 256; i++)
            m_messageFuncPointerTab[i] = nullptr;

        ProcessOSMessage* pf = m_messageFuncPointerTab;
        pf[DUI_CREATE] = &T::OnCreate;
        pf[DUI_DESTROY] = &T::OnDestroy;
        pf[DUI_MOUSEMOVE] = &T::OnMouseMove;
        pf[DUI_LBUTTONDOWN] = &T::OnLButtonDown;
        pf[DUI_LBUTTONUP] = &T::OnLButtonUp;
        pf[DUI_SIZE] = &T::OnSize;
        pf[DUI_TIMER] = &T::OnTimer;
        pf[DUI_MOUSEWHEEL] = &T::OnMouseWheel;
        pf[DUI_KEYDOWN] = &T::OnKeyPress;
        pf[DUI_CHAR] = &T::OnChar;
        pf[DUI_SETCURSOR] = &T::OnSetCursor;
        pf[DUI_MOUSELEAVE] = &T::On_DUI_MOUSELEAVE;
        pf[DUI_MOUSEHOVER] = &T::On_DUI_MOUSEHOVER;
#endif
    }

    ~XViewT()
    {
        int i, j;
        XControl* xctl;

        for (j = 0; j < DUI_MODETOTAL; j++)
            for (i = 0; i < DUI_MAX_CONTROLS; i++)
            {
                xctl = m_ctlArray[j][i];
                if (xctl)
                    xctl->Term();
            }

        wt_mempool_destroy(m_pool);
        m_pool = nullptr;
    }

    U32* GetScreenBuff()
    {
        return m_screen;
    }

    int GetActiveButton()
    {
        return m_activeControl;
    }

#if 0

    void AfterSetMode() {}

    void SetMode(U16 mode)
    {
        assert(mode < DUI_MODETOTAL);
        m_mode = mode;

        T* pT = static_cast<T*>(this);
        pT->AfterSetMode();

        // because the mode is changed, this window needs to redraw
        m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
        InvalidateDUIWindow();           // set the gloabl redraw flag so next paint routine will do the paint work
    }

    U16 GetMode()
    {
        return m_mode;
    }
#endif
    static int XControlAction(void* obj, U32 uMsg, U64 wParam, U64 lParam)
    {
        int ret = 0;

        T* pT = static_cast<T*>(obj);
        if (nullptr != pT)
            ret = pT->NotifyParent(uMsg, wParam, lParam);

        return ret;
    }

    void InitControl() {}

    // < 0 : I do not handle this message
    // = 0 : I handled, but I do not need to upgrade the screen
    // > 0 : I handled this message, and need the host window to upgrade the screen
    int HandleOSMessage(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = -1;

        assert(uMsg <= 0xFFFF);

        U16 msgIdOS = (U16)(uMsg & 0xFFFF);
        U8  msgId = DUIMessageOSMap[msgIdOS];  // lookup the message map from Platform to our DUI message

        switch (msgId)
        {
        case DUI_PAINT:
            r = On_DUI_PAINT(msgId, wParam, lParam, lpData);
            break;
        case DUI_MOUSEMOVE:
            r = On_DUI_MOUSEMOVE(msgId, wParam, lParam, lpData);
            break;
#if 0
        case DUI_TIMER:
            r = On_DUI_TIMER(msgId, wParam, lParam, lpData);
            break;
        case DUI_CHAR:
            r = On_DUI_CHAR(msgId, wParam, lParam, lpData);
            break;
        case DUI_KEYDOWN:
            r = On_DUI_KEYDOWN(msgId, wParam, lParam, lpData);
            break;
        case DUI_MOUSEWHEEL:
            r = On_DUI_MOUSEWHEEL(msgId, wParam, lParam, lpData);
            break;
#endif
        case DUI_LBUTTONDOWN:
            r = On_DUI_LBUTTONDOWN(msgId, wParam, lParam, lpData);
            break;
        case DUI_LBUTTONUP:
            r = On_DUI_LBUTTONUP(msgId, wParam, lParam, lpData);
            break;
        case DUI_LBUTTONDBLCLK:
            r = On_DUI_LBUTTONDBLCLK(msgId, wParam, lParam, lpData);
            break;
        case DUI_RBUTTONUP:
            r = On_DUI_RBUTTONUP(msgId, wParam, lParam, lpData);
            break;
        case DUI_MOUSELEAVE:
            r = On_DUI_DUI_MOUSELEAVE(msgId, wParam, lParam, lpData);
            break;
        case DUI_MOUSEHOVER:
            r = On_DUI_DUI_MOUSEHOVER(msgId, wParam, lParam, lpData);
            break;
        case DUI_NCLBUTTONDOWN:
            r = On_DUI_NCLBUTTONDOWN(msgId, wParam, lParam, lpData);
            break;
        case DUI_CREATE:
            r = On_DUI_CREATE(msgId, wParam, lParam, lpData);
            break;
        case DUI_DESTROY:
            r = On_DUI_DESTROY(msgId, wParam, lParam, lpData);
            break;
        default:
            break;
        }
#if 0
        ProcessOSMessage pfMSG = m_messageFuncPointerTab[msgId];
        if (nullptr != pfMSG)
        {
            T* pT = static_cast<T*>(this);
            r = (pT->*pfMSG)(uMsg, wParam, lParam, lpData);
        }
#endif
        return r;
    }

    bool IsFocused()
    {
        return (DUI_STATUS_ISFOCUS & m_status);
    }

    void InvalidateScreen()
    {
        m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
        InvalidateDUIWindow();           // set the gloabl redraw flag so next paint routine will do the paint work
    }

    bool IsRealWindow(void* hWnd)
    {
#ifdef _WIN32
        return (::IsWindow((HWND)hWnd));
#else
        return false;
#endif
    }

    // Windows Id is used for debugging purpose
    void SetWindowId(const U8* id, U8 bytes)
    {
        if (bytes > 7)
            bytes = 7;

        for (U8 i = 0; i < bytes; i++)
            m_Id[i] = *id++;

        m_Id[bytes] = 0; // zero-terminated string

        assert(!m_pool);
        if ((DUI_PROP_NONEMEMPOOL & m_property) == 0)
        {
            T* pT = static_cast<T*>(this);

            U32 initSize = DUI_ALLOCSET_SMALL_INITSIZE;
            U32 maxSize = DUI_ALLOCSET_SMALL_MAXSIZE;
            if (DUI_PROP_LARGEMEMPOOL & m_property)
            {
                initSize = DUI_ALLOCSET_DEFAULT_INITSIZE;
                maxSize = DUI_ALLOCSET_DEFAULT_MAXSIZE;
            }
            m_pool = wt_mempool_create((const char*)m_Id, 0, initSize, maxSize);
            assert(m_pool);
            pT->InitControl();
        }
    }

    bool IsVisible() const
    {
        return (m_status & DUI_STATUS_VISIBLE);
    }

    void SetWindowShow(bool bShow = true)
    {
        if (!bShow)
            m_status &= ~DUI_STATUS_VISIBLE;
        else
            m_status |= DUI_STATUS_VISIBLE;

        InvalidateScreen();
    }

    XRECT* GetWindowArea()
    {
        return &m_area;
    }

    void SetScreenValide()
    {
        m_status &= ~DUI_STATUS_NEEDRAW;
    }

    bool PostWindowMessage(U32 message, U64 wParam = 0, U64 lParam = 0)
    {
        bool bRet = false;
        assert(IsRealWindow(m_hWnd));
#if defined(_WIN32)
        bRet = ::PostMessage((HWND)m_hWnd, (UINT)message, (WPARAM)wParam, (LPARAM)lParam);
#endif
        return bRet;
    }

    int NotifyParent(U32 uMsg, U64 wParam, U64 lParam)
    {
        PostWindowMessage(uMsg, wParam, lParam);
        return 0;
    }

    // scan the whole control array and reset them to the status one by one
    int SetAllControlStatus(U32 status, U8 mouse_event)
    {
        int r = 0;
        XControl* xctl;

        if (DUI_PROP_BTNACTIVE & m_property)
        {
            U32 ctlStatus,property;
            
            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                property = xctl->getProperty();
                if ((0 == (XCONTROL_PROP_HIDDEN & property)) && (XCONTROL_PROP_ACTIVE & property))
                {
                    ctlStatus = (i != m_activeControl) ? status : XCONTROL_STATE_ACTIVE;
                    r += xctl->setStatus(ctlStatus, mouse_event);
                }
            }
        }
        else
        {
            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                    r += xctl->setStatus(status, mouse_event);
            }
        }

        return r;
    }

    void UpdateControlPosition() {}

    int Draw(DUI_Surface surface, 
        DUI_Brush brushText = nullptr, 
        DUI_Brush brushSelText = nullptr, 
        DUI_Brush brushCaret = nullptr, 
        DUI_Brush brushBkg0 = nullptr, 
        DUI_Brush brushBkg1 = nullptr)
    {
        int r = 0;
        U32* buff = nullptr;
        U8 status = m_status & (DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW);

        if ((DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW) == status) // this window is visible and need to draw
            buff = m_screen;

        if (buff) // if this window has screen data to draw
        {
            int w = m_area.right - m_area.left;
            int h = m_area.bottom - m_area.top;

            assert(w > 0);
            assert(h > 0);

            ID2D1HwndRenderTarget* pD2DRenderTarget = static_cast<ID2D1HwndRenderTarget*>(surface);
            if (pD2DRenderTarget)
            {
                m_status &= ~DUI_STATUS_NEEDRAW; // prevent unnecessary draw
                ID2D1Bitmap* pBitmap = nullptr;
                HRESULT hr = pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), buff, (w << 2),
                    D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);
                if (S_OK == hr && nullptr != pBitmap)
                {
                    D2D1_RECT_F rect = D2D1::RectF(
                        static_cast<FLOAT>(m_area.left),
                        static_cast<FLOAT>(m_area.top),
                        static_cast<FLOAT>(m_area.right),
                        static_cast<FLOAT>(m_area.bottom)
                    );

                    pD2DRenderTarget->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                    pD2DRenderTarget->DrawBitmap(pBitmap, &rect); // do the real draw
                    ReleaseUnknown(pBitmap);
#if 0
                    if (DUI_PROP_HANDLETEXT & m_property) // if this virtual window has text to draw, we use directwrite to draw the text
                    {
                        DrawText(surface, brushText, brushSelText, brushCaret, brushBkg0, brushBkg1);
                    }
#endif 
                    pD2DRenderTarget->PopAxisAlignedClip();
                    r = 1;
                }
            }
        }
        return r;
    }

    int DoDrawText(DUI_Surface surface, DUI_Brush brushText, DUI_Brush brushSelText, DUI_Brush brushCaret, DUI_Brush brushBkg0, DUI_Brush brushBkg1)
    {
        return 0;
    }

    int DrawText(DUI_Surface surface, DUI_Brush brushText, DUI_Brush brushSelText, DUI_Brush brushCaret, DUI_Brush brushBkg0, DUI_Brush brushBkg1)
    {
        if (nullptr != surface)
        {
            U32 prop;
            XControl* xctl;

            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                    xctl->DrawText(m_area.left, m_area.top, surface, brushText, brushSelText, brushCaret);
            }
            // the derived class will draw its content
            T* pT = static_cast<T*>(this);
            pT->DoDrawText(surface, brushText, brushSelText, brushCaret, brushBkg0, brushBkg1);
        }
        return 0;
    }

    int Do_DUI_PAINT(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_PAINT(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        U8 status = m_status & (DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW);

        // We only draw this virtual window when 1: it is visible, and 2: it needs draw
        if (((DUI_STATUS_VISIBLE | DUI_STATUS_NEEDRAW) == status) && (nullptr != m_screen))
        {
            XControl* xctl;
            int w = m_area.right - m_area.left;
            int h = m_area.bottom - m_area.top;

            assert(w > 0);
            assert(h > 0);

            // fill the whole screen of this virutal window with a single color
            wt_ScreenClear(m_screen, m_size, m_backgroundColor);
#if 0
            if (DUI_STATUS_VSCROLL & m_status) // We have the vertical scroll bar to draw
            {
                int thumb_start, thumb_height, thumb_width;
                int vOffset = m_ptOffset.y;
                int vHeight = m_sizeAll.cy;
                assert(m_sizeAll.cy > h);

                thumb_width = m_scrollWidth - 2;
                thumb_start = (vOffset * h) / vHeight;
                thumb_height = (h * h) / vHeight;
                if (thumb_height < DUI_MINIMAL_THUMB_SIZE)
                    thumb_height = DUI_MINIMAL_THUMB_SIZE; // we keep the thumb mini size to some pixels

                // Draw the vertical scroll bar
                wt_ScreenFillRect(m_screen, w, h, m_scrollbarColor, m_scrollWidth, h, w - m_scrollWidth, 0);
                wt_ScreenFillRectRound(m_screen, w, h, m_thumbColor, thumb_width, thumb_height, w - m_scrollWidth + 1, thumb_start, m_scrollbarColor, 0xFFD6D3D2);
            }
#endif
            // draw the controls within this window
            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                    xctl->Draw(m_area.left, m_area.top);
            }
            // the derived class will draw its content
            T* pT = static_cast<T*>(this);
            pT->Do_DUI_PAINT(uMsg, wParam, lParam, lpData);
        }

        return 0;
    }

    int PostUpdateSize() { return 0; }
    int UpdateSize(XRECT* r, U32* screenbuf = nullptr)
    {
        m_screen = screenbuf;
        T* pT = static_cast<T*>(this);

        if (nullptr == m_screen) // this window does not have memory to draw, just return
        {
            m_area.left = m_area.top = m_area.right = m_area.bottom = 0;
            m_size = 0;
            m_status &= ~DUI_STATUS_VISIBLE;
        }
        else
        {
            XControl* xctl;

            assert(r);
            assert(r->right > r->left);
            assert(r->bottom > r->top);

            m_area.left = r->left;
            m_area.top = r->top;
            m_area.right = r->right;
            m_area.bottom = r->bottom;
            m_size = (U32)((r->right - r->left) * (r->bottom - r->top));

            m_status |= DUI_STATUS_VISIBLE;

            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                xctl->AttachParent(m_screen, m_area.left, m_area.top, m_area.right, m_area.bottom);
            }
            pT->UpdateControlPosition();
        }

        pT->PostUpdateSize();

        m_status |= DUI_STATUS_NEEDRAW;  // need to redraw this virtual window
        InvalidateDUIWindow();

        return DUI_STATUS_NEEDRAW;
    }

    int On_DUI_DUI_MOUSEHOVER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        bool bInMyArea = XWinPointInRect(xPos, yPos, &m_area); // is the point in my area?

        assert(m_mode < DUI_MODETOTAL);
        if (bInMyArea && m_maxCtl[m_mode]) // the mouse is hovering in my area and I also have the controls
        {
            int id = -1;
            XControl* xctl;
            int dx = xPos - m_area.left;
            int dy = yPos - m_area.top;

            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                {
                    id = xctl->DoMouseHover(dx, dy);
                    if (id >= 0)
                        break;    // we find the control that is hovering
                }
            }
        }

        return r;
    }

    int On_DUI_DUI_MOUSELEAVE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        XControl* xctl;

        assert(m_mode < DUI_MODETOTAL);
        for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
        {
            xctl = m_ctlArray[m_mode][i];
            assert(nullptr != xctl);
            assert(xctl->m_Id == ((m_mode << 8) | i));
            if (xctl->IsVisible())
                r += xctl->DoMouseLeave();
        }

        if (DUI_PROP_HASVSCROLL & m_property) // handle the vertical bar
        {
            U32 statusOld = m_status;
            m_status &= (~DUI_STATUS_VSCROLL); // let the vertical bar to disappear
            if (statusOld != m_status)
                r++;
        }

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }
        return r;
    }

    int On_DUI_NCLBUTTONDOWN(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        ClearDUIWindowLBtnDown();
        return 0;
    }

    int Do_DUI_MOUSEWHEEL(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_MOUSEWHEEL(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        if ((DUI_PROP_HANDLEWHEEL & m_property) && !DUIWindowInDragMode())
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            bool bInMyArea = XWinPointInRect(xPos, yPos, &m_area); // is the point in my area?

            if (bInMyArea)
            {
                int h = m_area.bottom - m_area.top;
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);

                if (m_sizeAll.cy > h)
                {
                    int yOld = m_ptOffset.y;

                    assert(m_sizeLine.cy > 0);
                    if (delta < 0)
                        m_ptOffset.y += m_sizeLine.cy;
                    else
                        m_ptOffset.y -= m_sizeLine.cy;

                    if (m_ptOffset.y < 0)
                        m_ptOffset.y = 0;
                    if (m_ptOffset.y > (m_sizeAll.cy - h))
                        m_ptOffset.y = m_sizeAll.cy - h;

                    if (yOld != m_ptOffset.y)
                        r++;
                }

                {  // let the derived class to do its stuff
                    T* pT = static_cast<T*>(this);
                    r += pT->Do_DUI_MOUSEWHEEL(uMsg, wParam, lParam, lpData);
                }
            }
        }

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }

        return r;
    }

    int Do_DUI_MOUSEMOVE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_MOUSEMOVE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int w = m_area.right - m_area.left;
        int h = m_area.bottom - m_area.top;
        bool bInMyArea = XWinPointInRect(xPos, yPos, &m_area); // is the point in my area?

        if (XDragMode::DragNone != m_DragMode) // this window is in drag mode
        {
            assert(DUIWindowInDragMode());
            m_status |= DUI_STATUS_VSCROLL;
            m_ptOffset.y = m_ptOffsetOld.y + ((yPos - m_cxyDragOffset) * m_sizeAll.cy) / h;
            if (m_ptOffset.y < 0)
                m_ptOffset.y = 0;
            if (m_ptOffset.y > (m_sizeAll.cy - h))
                m_ptOffset.y = m_sizeAll.cy - h;
            r++;
        }
        else  // this window is not in drag mode
        {
            int dx = -1;
            int dy = -1;
            XControl* xctl;

            if (bInMyArea) // the mosue is in my area
            {
                dx = xPos - m_area.left;
                dy = yPos - m_area.top;
#if 0
                if (DUI_PROP_HASVSCROLL & m_property) // handle the vertical bar
                {
                    U32 statusOld = m_status;
                    m_status &= (~DUI_STATUS_VSCROLL); // let the vertical bar to disappear at first
                    if (m_sizeAll.cy > h)   // the total page size is greater than the real window size, we need to show the vertical bar
                    {
                        int thumb_start = (m_ptOffset.y * h) / m_sizeAll.cy; // the position of the thumb relative to m_area
                        int thumb_height = (h * h) / m_sizeAll.cy;  // the thumb height, the bigger m_sizeAll.cy, the smaller of thumb height
                        if (thumb_height < DUI_MINIMAL_THUMB_SIZE)
                            thumb_height = DUI_MINIMAL_THUMB_SIZE; // we keep the thumb mini size to some pixels
                        m_status |= DUI_STATUS_VSCROLL; // show vertical bar
                        assert(m_ptOffset.y >= 0);
                        assert(m_ptOffset.y <= m_sizeAll.cy - h);  // the range of m_ptOffset.y is from 0 to m_sizeAll.cy - h
                        if ((statusOld & DUI_STATUS_VSCROLL) != DUI_STATUS_VSCROLL)
                            r++;
                    }
                }
#endif
            }
            else // the mouse is not in our area, we do a quick check to be sure this window need to be redraw
            {
                // hide the vertical bar
                if (DUI_STATUS_VSCROLL & m_status)
                {
                    m_status &= (~DUI_STATUS_VSCROLL); // we should not dispaly the vertical bar
                    r++;
                }
            }

            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                    r += xctl->DoMouseMove(dx, dy, m_activeControl);
            }
        }

        // let the derived class to do its stuff
        if (!DUIWindowInDragMode())
        {
            T* pT = static_cast<T*>(this);
            r += pT->Do_DUI_MOUSEMOVE(uMsg, wParam, lParam, lpData);
        }

        if (r) // determine whether this window needs to be redraw
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }
        return r;
    }

    int Do_DUI_LBUTTONDOWN(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }

    int On_DUI_LBUTTONDOWN(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        U32 statusOld = m_status;
        XControl* xctl;
        int dx = -1;
        int dy = -1;
        int idxActive = -1;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        SetDUIWindowLBtnDown();

        bool bInMyArea = XWinPointInRect(xPos, yPos, &m_area); // is the point in my area?

        if (bInMyArea)
        {
            // transfer the coordination from real window to local virutal window
            dx = xPos - m_area.left;
            dy = yPos - m_area.top;

            if (!DUIWindowInDragMode())
            {
                int hit = -1;
                int w = m_area.right - m_area.left;
                int h = m_area.bottom - m_area.top;

                m_status |= DUI_STATUS_ISFOCUS;  // the mouse click my area, so I have the focus
            }
        }
        else
        {
            m_status &= ~(DUI_STATUS_ISFOCUS | DUI_STATUS_VSCROLL); // this window lose focus, ad do not show the vertical bar
            if ((DUI_STATUS_VSCROLL & statusOld) != (DUI_STATUS_VSCROLL & m_status))
                r++;
        }

        idxActive = -1;
        assert(m_mode < DUI_MODETOTAL);
        for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
        {
            xctl = m_ctlArray[m_mode][i];
            assert(nullptr != xctl);
            assert(xctl->m_Id == ((m_mode << 8) | i));
            if (xctl->IsVisible())
            {
                r += xctl->DoMouseLBClickDown(dx, dy, &idxActive);
                if (idxActive >= 0) // hit me
                {
                    if ((xctl->getProperty() & XCONTROL_PROP_LBDOWN))
                    {
                        assert(xctl->pfAction);
                        xctl->pfAction(this, m_message, 0, (LPARAM)idxActive);
                    }
                }
            }
        }

        if (DUI_PROP_MOVEWIN & m_property)
        {
            if (bInMyArea && (-1 == idxActive))  // if the mouse does not hit the button, we can move the whole real window
                PostWindowMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
        }

        if (DUI_PROP_BTNACTIVE & m_property) // this window has active button, just like radio button group
        {
            if (idxActive >= 0)
            {
                U32 p0, p1;
                XControl* xctlNew;
                idxActive &= 0xFF;

                assert(idxActive < m_maxCtl[m_mode]);
                assert(m_activeControl >= 0);
                assert(m_activeControl < m_maxCtl[m_mode]);
                xctl = m_ctlArray[m_mode][m_activeControl];
                p0 = xctl->getProperty();
                xctlNew = m_ctlArray[m_mode][idxActive];
                p1 = xctlNew->getProperty();
                if ((p0 & XCONTROL_PROP_ACTIVE) && (p1 & XCONTROL_PROP_ACTIVE))
                {
                    xctl->setStatus(XCONTROL_STATE_NORMAL, XMOUSE_LBDOWN); // set the old active button to normal state
                    m_activeControl = idxActive;
                    xctlNew->setStatus(XCONTROL_STATE_PRESSED, XMOUSE_LBDOWN); // set the new active button to pressed state
                    r++;
                }
            }
        }

        // let the derived class to do its stuff
        {
            T* pT = static_cast<T*>(this);
            r += pT->Do_DUI_LBUTTONDOWN(uMsg, wParam, lParam, lpData);
        }

        if (r) // we need to draw this window
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }
        return r;
    }

    int Do_DUI_RBUTTONUP(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_RBUTTONUP(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        int dx = -1;
        int dy = -1;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        XControl* xctl;

        ClearDUIWindowLBtnDown();
        ClearDUIWindowDragMode();
        m_DragMode = XDragMode::DragNone;
        m_ptOffsetOld.x = -1, m_ptOffsetOld.y = -1;

        if (XWinPointInRect(xPos, yPos, &m_area))
        {
            dx = xPos - m_area.left;
            dy = yPos - m_area.top;
        }

        assert(m_mode < DUI_MODETOTAL);
        for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
        {
            xctl = m_ctlArray[m_mode][i];
            assert(nullptr != xctl);
            assert(xctl->m_Id == ((m_mode << 8) | i));
            if (xctl->IsVisible())
                r += xctl->DoMouseRBClickUp(dx, dy, xPos, yPos, m_hWnd);
        }

        {
            T* pT = static_cast<T*>(this);
            r += pT->Do_DUI_RBUTTONUP(uMsg, wParam, lParam, lpData);
        }

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }
        return r;
    }

    int Do_DUI_LBUTTONUP(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_LBUTTONUP(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        int dx = -1;
        int dy = -1;
        int idxActive = -1;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        XControl* xctl;

        ClearDUIWindowLBtnDown();

        ClearDUIWindowDragMode();
        m_DragMode = XDragMode::DragNone;
        m_ptOffsetOld.x = -1, m_ptOffsetOld.y = -1;

        if (XWinPointInRect(xPos, yPos, &m_area))
        {
            dx = xPos - m_area.left;
            dy = yPos - m_area.top;
        }

        assert(m_mode < DUI_MODETOTAL);
        for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
        {
            xctl = m_ctlArray[m_mode][i];
            assert(nullptr != xctl);
            assert(xctl->m_Id == ((m_mode << 8) | i));
            if (xctl->IsVisible())
                r += xctl->DoMouseLBClickUp(dx, dy, &idxActive);
        }

        if (idxActive >= 0)
        {
            U16 realId = ((U16)idxActive) & 0xFF;
            assert(realId < m_maxCtl[m_mode]);
            xctl = m_ctlArray[m_mode][realId];
            assert(xctl);
            if ((xctl->getProperty() & XCONTROL_PROP_LBDOWN) == 0)
            {
                assert(xctl->pfAction);
                xctl->pfAction(this, m_message, 0, (LPARAM)idxActive);
            }
        }

        // this window has active button, just like radio button group
        if ((DUI_PROP_BTNACTIVE & m_property) && (idxActive >= 0))
        {
            U32 p0, p1;
            XControl* xctlNew;
            idxActive &= 0xFF;

            assert(idxActive < m_maxCtl[m_mode]);
            assert(m_activeControl >= 0);
            assert(m_activeControl < m_maxCtl[m_mode]);
            xctl = m_ctlArray[m_mode][m_activeControl];
            p0 = xctl->getProperty();
            xctlNew = m_ctlArray[m_mode][idxActive];
            p1 = xctlNew->getProperty();
            if ((p0 & XCONTROL_PROP_ACTIVE) && (p1 & XCONTROL_PROP_ACTIVE))
            {
                xctlNew->setStatus(XCONTROL_STATE_ACTIVE, XMOUSE_LBUP);
                r++;
            }
        }

        {
            T* pT = static_cast<T*>(this);
            r += pT->Do_DUI_LBUTTONUP(uMsg, wParam, lParam, lpData);
        }

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }

        return r;
    }

    int Do_DUI_LBUTTONDBLCLK(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_LBUTTONDBLCLK(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        T* pT = static_cast<T*>(this);

        r = pT->Do_DUI_LBUTTONDBLCLK(uMsg, wParam, lParam, lpData);

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }

        return r;
    }

    int Do_DUI_DESTROY(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_DESTROY(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        T* pT = static_cast<T*>(this);
        int ret = pT->Do_DUI_DESTROY(uMsg, wParam, lParam, lpData);
        return ret;
    }

    int Do_DUI_CHAR(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_CHAR(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        if ((DUI_PROP_HANDLEKEYBOARD & m_property) && !DUIWindowInDragMode())
        {
            if (DUI_STATUS_ISFOCUS & m_status) // this window has the focus
            {
                XControl* xctl;
                assert(m_mode < DUI_MODETOTAL);
                for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
                {
                    xctl = m_ctlArray[m_mode][i];
                    assert(nullptr != xctl);
                    assert(xctl->m_Id == ((m_mode << 8) | i));
                    if (xctl->IsVisible())
                        r += xctl->OnKeyBoard(uMsg, wParam, lParam);
                }
                T* pT = static_cast<T*>(this);
                r += pT->Do_DUI_CHAR(uMsg, wParam, lParam, lpData);
                if (r)
                {
                    m_status |= DUI_STATUS_NEEDRAW;
                    InvalidateDUIWindow();
                }
            }
        }
        return r;
    }

    int Do_DUI_KEYDOWN(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_KEYDOWN(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        if ((DUI_PROP_HANDLEKEYBOARD & m_property) && !DUIWindowInDragMode())
        {
            if (DUI_STATUS_ISFOCUS & m_status)  // this window has the focus
            {
                XControl* xctl;
                assert(m_mode < DUI_MODETOTAL);
                for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
                {
                    xctl = m_ctlArray[m_mode][i];
                    assert(nullptr != xctl);
                    assert(xctl->m_Id == ((m_mode << 8) | i));
                    if (xctl->IsVisible())
                        r += xctl->OnKeyBoard(uMsg, wParam, lParam);
                }

                T* pT = static_cast<T*>(this);
                r += pT->Do_DUI_KEYDOWN(uMsg, wParam, lParam, lpData);
                if (r)
                {
                    m_status |= DUI_STATUS_NEEDRAW;
                    InvalidateDUIWindow();
                }
            }
        }
        return r;
    }

    int Do_DUI_TIMER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_TIMER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int r = 0;
        if ((DUI_PROP_HANDLETIMER & m_property))
        {
            XControl* xctl;

            assert(m_mode < DUI_MODETOTAL);
            for (U16 i = 0; i < m_maxCtl[m_mode]; i++)
            {
                xctl = m_ctlArray[m_mode][i];
                assert(nullptr != xctl);
                assert(xctl->m_Id == ((m_mode << 8) | i));
                if (xctl->IsVisible())
                    r += xctl->OnTimer();
            }
        }

        {
            T* pT = static_cast<T*>(this);
            r += pT->Do_DUI_TIMER(uMsg, wParam, lParam, lpData);
        }

        if (r)
        {
            m_status |= DUI_STATUS_NEEDRAW;
            InvalidateDUIWindow();
        }

        return r;
    }

    int Do_DUI_MOUSELEAVE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_MOUSELEAVE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        return 0;
    }

    int Do_DUI_MOUSEHOVER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_MOUSEHOVER(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        return 0;
    }

    int Do_DUI_SIZE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_SIZE(U32 uMsg, U64 wParam, U64 lParam, void* lpData)
    {
        T* pT = static_cast<T*>(this);
        int ret = pT->Do_DUI_SIZE(uMsg, wParam, lParam, lpData);
        return ret;
    }

    int Do_DUI_CREATE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr) { return 0; }
    int On_DUI_CREATE(U32 uMsg, U64 wParam, U64 lParam, void* lpData = nullptr)
    {
        int i, j;
        int r = 0;
        XControl* xctl;

#ifdef _WIN32
        m_hWnd = (HWND)wParam;
        assert(IsRealWindow(m_hWnd));
#else
        m_hWnd = nullptr;
#endif
        T* pT = static_cast<T*>(this);

        for (j = 0; j < DUI_MODETOTAL; j++)
        {
            for (i = 0; i < DUI_MAX_CONTROLS; i++)
            {
                xctl = m_ctlArray[j][i];
                if (xctl)
                    xctl->pfAction = XControlAction;
            }
        }

        r = pT->Do_DUI_CREATE(uMsg, wParam, lParam, lpData);
        if (r)
            SetDUIWindowInitFailed();

        return r;
    }
};
