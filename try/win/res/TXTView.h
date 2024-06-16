// View.h : interface of the CTxtView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#if 0
#if 0
typedef D2D1_RECT_F RectF;

struct MakeDWriteTextRange : public DWRITE_TEXT_RANGE
{
	inline MakeDWriteTextRange(UINT32 startPosition, UINT32 length)
	{
		this->startPosition = startPosition;
		this->length = length;
	}

	// Overload to extend to end of text.
	inline MakeDWriteTextRange(UINT32 startPosition)
	{
		this->startPosition = startPosition;
		this->length = UINT32_MAX - startPosition;
	}
};

////////////////////////////////////////
// COM help.
// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline void SafeRelease(InterfaceType** currentObject)
{
	if (*currentObject != NULL)
	{
		(*currentObject)->Release();
		*currentObject = NULL;
	}
}


// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
	if (newObject != NULL)
		newObject->AddRef();

	return newObject;
}


// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
	SafeAcquire(newObject);
	SafeRelease(currentObject);
	*currentObject = newObject;
}


// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline InterfaceType* SafeDetach(InterfaceType** currentObject)
{
	InterfaceType* oldObject = *currentObject;
	*currentObject = NULL;
	return oldObject;
}


// Sets a new COM object, acquiring the reference.
template <typename InterfaceType>
inline void SafeAttach(InterfaceType** currentObject, InterfaceType* newObject)
{
	SafeRelease(currentObject);
	*currentObject = newObject;
}

// Generic COM base implementation for classes, since DirectWrite uses
// callbacks for several different kinds of objects, particularly the
// text renderer and inline objects.
//
// Example:
//
//  class RenderTarget : public ComBase<QiList<IDWriteTextRenderer> >
//
template <typename InterfaceChain>
class ComBase : public InterfaceChain
{
public:
	explicit ComBase() throw()
		: refValue_(0)
	{ }

	// IUnknown interface
	IFACEMETHOD(QueryInterface)(IID const& iid, OUT void** ppObject)
	{
		*ppObject = NULL;
		InterfaceChain::QueryInterfaceInternal(iid, ppObject);
		if (*ppObject == NULL)
			return E_NOINTERFACE;

		AddRef();
		return S_OK;
	}

	IFACEMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&refValue_);
	}

	IFACEMETHOD_(ULONG, Release)()
	{
		ULONG newCount = InterlockedDecrement(&refValue_);
		if (newCount == 0)
			delete this;

		return newCount;
	}

	virtual ~ComBase()
	{ }

protected:
	ULONG refValue_;

private:
	// No copy construction allowed.
	ComBase(const ComBase& b);
	ComBase& operator=(ComBase const&);
};

struct QiListNil
{
};

// When the QueryInterface list refers to itself as class,
// which hasn't fully been defined yet.
template <typename InterfaceName, typename InterfaceChain>
class QiListSelf : public InterfaceChain
{
public:
	inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
	{
		if (iid != __uuidof(InterfaceName))
			return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

		*ppObject = static_cast<InterfaceName*>(this);
	}
};

// When this interface is implemented and more follow.
template <typename InterfaceName, typename InterfaceChain = QiListNil>
class QiList : public InterfaceName, public InterfaceChain
{
public:
	inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
	{
		if (iid != __uuidof(InterfaceName))
			return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

		*ppObject = static_cast<InterfaceName*>(this);
	}
};


// When the this is the last implemented interface in the list.
template <typename InterfaceName>
class QiList<InterfaceName, QiListNil> : public InterfaceName
{
public:
	inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
	{
		if (iid != __uuidof(InterfaceName))
			return;

		*ppObject = static_cast<InterfaceName*>(this);
	}
};

class DECLSPEC_UUID("1CD7C44F-526B-492a-B780-EF9C4159B653") DrawingEffect
	: public ComBase<QiList<IUnknown> >
{
public:
	DrawingEffect(UINT32 color)
		: color_(color)
	{ }

	inline UINT32 GetColor() const throw()
	{
		// Returns the BGRA value for D2D.
		return color_;
	}

	inline COLORREF GetColorRef() const throw()
	{
		// Returns color as COLORREF.
		return GetColorRef(color_);
	}

	static inline COLORREF GetColorRef(UINT32 bgra) throw()
	{
		// Swaps color order (bgra <-> rgba) from D2D/GDI+'s to a COLORREF.
		// This also leaves the top byte 0, since alpha is ignored anyway.
		return RGB(GetBValue(bgra), GetGValue(bgra), GetRValue(bgra));
	}

	static inline COLORREF GetBgra(COLORREF rgb) throw()
	{
		// Swaps color order (bgra <-> rgba) from COLORREF to D2D/GDI+'s.
		// Sets alpha to full opacity.
		return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb)) | 0xFF000000;
	}

protected:
	// The color is stored as BGRA, with blue in the lowest byte,
	// then green, red, alpha; which is what D2D, GDI+, and GDI DIBs use.
	// GDI's COLORREF stores red as the lowest byte.
	UINT32 color_;
};



// Intermediate render target for UI to draw to either a D2D or GDI surface.
class DECLSPEC_UUID("4327AC14-3172-4807-BF40-02C7475A2520") RenderTarget
	: public ComBase<
	QiListSelf<RenderTarget,
	QiList<IDWriteTextRenderer>
	> >
{
public:
	virtual ~RenderTarget() {};

	virtual void BeginDraw() = NULL;
	virtual void EndDraw() = NULL;
	virtual void Clear(UINT32 color) = NULL;
	virtual void Resize(UINT width, UINT height) = NULL;
	virtual void UpdateMonitor() = NULL;

	virtual void SetTransform(DWRITE_MATRIX const& transform) = NULL;
	virtual void GetTransform(DWRITE_MATRIX & transform) = NULL;
	virtual void SetAntialiasing(bool isEnabled) = NULL;


	virtual void DrawTextLayout(
		IDWriteTextLayout * textLayout,
		const RectF & rect
	) = NULL;

	// Draws a single image, from the given coordinates, to the given coordinates.
	// If the height and width differ, they will be scaled, but mirroring must be
	// done via a matrix transform.
	virtual void DrawImage(
		IWICBitmapSource * image,
		const RectF & sourceRect,  // where in source atlas texture
		const RectF & destRect     // where on display to draw it
	) = NULL;

	virtual void FillRectangle(
		const RectF & destRect,
		const DrawingEffect & drawingEffect
	) = NULL;

protected:
	// This context is not persisted, only existing on the stack as it
	// is passed down through. This is mainly needed to handle cases
	// where runs where no drawing effect set, like those of an inline
	// object or trimming sign.
	struct Context
	{
		Context(RenderTarget* initialTarget, IUnknown* initialDrawingEffect)
			: target(initialTarget),
			drawingEffect(initialDrawingEffect)
		{ }

		// short lived weak pointers
		RenderTarget* target;
		IUnknown* drawingEffect;
	};

	IUnknown* GetDrawingEffect(void* clientDrawingContext, IUnknown * drawingEffect)
	{
		// Callbacks use this to use a drawing effect from the client context
		// if none was passed into the callback.
		if (drawingEffect != NULL)
			return drawingEffect;

		return (reinterpret_cast<Context*>(clientDrawingContext))->drawingEffect;
	}
};

////////////////////////////////////////////////////////////////////////////////
class RenderTargetD2D : public RenderTarget
{
public:
	RenderTargetD2D(ID2D1Factory* d2dFactory, IDWriteFactory* dwriteFactory, HWND hwnd)
		: hwnd_(hwnd),
		hmonitor_(NULL),
		d2dFactory_(SafeAcquire(d2dFactory)),
		dwriteFactory_(SafeAcquire(dwriteFactory)),
		target_(),
		brush_()
	{
	}

	HRESULT static Create(ID2D1Factory* d2dFactory, IDWriteFactory* dwriteFactory, HWND hwnd, OUT RenderTarget** renderTarget)
	{
		*renderTarget = NULL;
		HRESULT hr = S_OK;

		RenderTargetD2D* newRenderTarget = SafeAcquire(new(std::nothrow) RenderTargetD2D(d2dFactory, dwriteFactory, hwnd));
		if (newRenderTarget == NULL)
		{
			return E_OUTOFMEMORY;
		}

		hr = newRenderTarget->CreateTarget();
		if (FAILED(hr))
			SafeRelease(&newRenderTarget);

		*renderTarget = SafeDetach(&newRenderTarget);

		return hr;
	}

	virtual ~RenderTargetD2D()
	{
		SafeRelease(&brush_);
		SafeRelease(&target_);
#if 0
		SafeRelease(&d2dFactory_);
		SafeRelease(&dwriteFactory_);
#endif 
	}

	virtual void BeginDraw()
	{
		target_->BeginDraw();
		target_->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	virtual void EndDraw()
	{
		HRESULT hr = target_->EndDraw();
		// If the device is lost for any reason, we need to recreate it.
		if (hr == D2DERR_RECREATE_TARGET)
		{
			// Flush resources and recreate them.
			// This is very rare for a device to be lost,
			// but it can occur when connecting via Remote Desktop.
			//imageCache_.clear();
			hmonitor_ = NULL;
			CreateTarget();
		}
	}

	virtual void Clear(UINT32 color)
	{
		target_->Clear(D2D1::ColorF(color));
	}

	virtual void Resize(UINT width, UINT height)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;
		target_->Resize(size);
	}

	virtual void UpdateMonitor()
	{
		// Updates rendering parameters according to current monitor.
		HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
		if (monitor != hmonitor_)
		{
			// Create based on monitor settings, rather than the defaults of
			// gamma=1.8, contrast=.5, and clearTypeLevel=.5
			IDWriteRenderingParams* renderingParams = NULL;
			dwriteFactory_->CreateMonitorRenderingParams(
				monitor,
				&renderingParams
			);
			target_->SetTextRenderingParams(renderingParams);

			hmonitor_ = monitor;
			InvalidateRect(hwnd_, NULL, FALSE);

			SafeRelease(&renderingParams);
		}
	}

	virtual void SetTransform(DWRITE_MATRIX const& transform)
	{
		target_->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(&transform));
	}

	virtual void GetTransform(DWRITE_MATRIX& transform)
	{
		target_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(&transform));
	}

	virtual void SetAntialiasing(bool isEnabled)
	{
		target_->SetAntialiasMode(isEnabled ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
	}

	virtual void DrawTextLayout(
		IDWriteTextLayout* textLayout,
		const RectF& rect
	)
	{
		if (textLayout == NULL)
			return;

		Context context(this, NULL);
		textLayout->Draw(
			&context,
			this,
			rect.left,
			rect.top
		);
	}

	virtual void DrawImage(
		IWICBitmapSource* image,
		const RectF& sourceRect,  // where in source atlas texture
		const RectF& destRect     // where on display to draw it
	)
	{}

	void FillRectangle(
		const RectF& destRect,
		const DrawingEffect& drawingEffect
	)
	{
		ID2D1Brush* brush = GetCachedBrush(&drawingEffect);
		if (brush == NULL)
			return;

		// We will always get a strikethrough as a LTR rectangle
		// with the baseline origin snapped.
		target_->FillRectangle(destRect, brush);
	}

	// IDWriteTextRenderer implementation

	IFACEMETHOD(DrawGlyphRun)(
		void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		const DWRITE_GLYPH_RUN* glyphRun,
		const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
		IUnknown* clientDrawingEffect
		)
	{
		// If no drawing effect is applied to run, but a clientDrawingContext
		// is passed, use the one from that instead. This is useful for trimming
		// signs, where they don't have a color of their own.
		clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

		// Since we use our own custom renderer and explicitly set the effect
		// on the layout, we know exactly what the parameter is and can
		// safely cast it directly.
		DrawingEffect* effect = static_cast<DrawingEffect*>(clientDrawingEffect);
		ID2D1Brush* brush = GetCachedBrush(effect);
		if (brush == NULL)
			return E_FAIL;

		target_->DrawGlyphRun(
			D2D1::Point2(baselineOriginX, baselineOriginY),
			glyphRun,
			brush,
			measuringMode
		);

		return S_OK;
	}

	IFACEMETHOD(DrawUnderline)(
		void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		const DWRITE_UNDERLINE* underline,
		IUnknown* clientDrawingEffect
		)
	{
		clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

		DrawingEffect* effect = static_cast<DrawingEffect*>(clientDrawingEffect);
		ID2D1Brush* brush = GetCachedBrush(effect);
		if (brush == NULL)
			return E_FAIL;

		// We will always get a strikethrough as a LTR rectangle
		// with the baseline origin snapped.
		D2D1_RECT_F rectangle =
		{
			baselineOriginX,
			baselineOriginY + underline->offset,
			baselineOriginX + underline->width,
			baselineOriginY + underline->offset + underline->thickness
		};

		// Draw this as a rectangle, rather than a line.
		target_->FillRectangle(&rectangle, brush);

		return S_OK;
	}

	IFACEMETHOD(DrawStrikethrough)(
		void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		const DWRITE_STRIKETHROUGH* strikethrough,
		IUnknown* clientDrawingEffect
		)
	{
		clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

		DrawingEffect* effect = static_cast<DrawingEffect*>(clientDrawingEffect);
		ID2D1Brush* brush = GetCachedBrush(effect);
		if (brush == NULL)
			return E_FAIL;

		// We will always get an underline as a LTR rectangle
		// with the baseline origin snapped.
		D2D1_RECT_F rectangle =
		{
			baselineOriginX,
			baselineOriginY + strikethrough->offset,
			baselineOriginX + strikethrough->width,
			baselineOriginY + strikethrough->offset + strikethrough->thickness
		};

		// Draw this as a rectangle, rather than a line.
		target_->FillRectangle(&rectangle, brush);

		return S_OK;
	}

	IFACEMETHOD(DrawInlineObject)(
		void* clientDrawingContext,
		FLOAT originX,
		FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways,
		BOOL isRightToLeft,
		IUnknown* clientDrawingEffect
		)
	{
		// Inline objects inherit the drawing effect of the text
		// they are in, so we should pass it down (if none is set
		// on this range, use the drawing context's effect instead).
		Context subContext(*reinterpret_cast<RenderTarget::Context*>(clientDrawingContext));

		if (clientDrawingEffect != NULL)
			subContext.drawingEffect = clientDrawingEffect;

		inlineObject->Draw(
			&subContext,
			this,
			originX,
			originY,
			false,
			false,
			subContext.drawingEffect
		);

		return S_OK;
	}

	IFACEMETHOD(IsPixelSnappingDisabled)(
		void* clientDrawingContext,
		OUT BOOL* isDisabled
		)
	{
		// Enable pixel snapping of the text baselines,
		// since we're not animating and don't want blurry text.
		*isDisabled = FALSE;
		return S_OK;
	}

	IFACEMETHOD(GetCurrentTransform)(
		void* clientDrawingContext,
		OUT DWRITE_MATRIX* transform
		)
	{
		// Simply forward what the real renderer holds onto.
		target_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
		return S_OK;
	}

	IFACEMETHOD(GetPixelsPerDip)(
		void* clientDrawingContext,
		OUT FLOAT* pixelsPerDip
		)
	{
		// Any scaling will be combined into matrix transforms rather than an
		// additional DPI scaling. This simplifies the logic for rendering
		// and hit-testing. If an application does not use matrices, then
		// using the scaling factor directly is simpler.
		*pixelsPerDip = 1;
		return S_OK;
	}

public:
#if 0
	// For cached images, to avoid needing to recreate the textures each draw call.
	struct ImageCacheEntry
	{
		ImageCacheEntry(IWICBitmapSource* initialOriginal, ID2D1Bitmap* initialConverted)
			: original(SafeAcquire(initialOriginal)),
			converted(SafeAcquire(initialConverted))
		{ }

		ImageCacheEntry(const ImageCacheEntry& b)
		{
			original = SafeAcquire(b.original);
			converted = SafeAcquire(b.converted);
		}

		ImageCacheEntry& operator=(const ImageCacheEntry& b)
		{
			if (this != &b)
			{
				// Define assignment operator in terms of destructor and
				// placement new constructor, paying heed to self assignment.
				this->~ImageCacheEntry();
				new(this) ImageCacheEntry(b);
			}
			return *this;
		}

		~ImageCacheEntry()
		{
			SafeRelease(&original);
			SafeRelease(&converted);
		}

		IWICBitmapSource* original;
		ID2D1Bitmap* converted;
	};
#endif
protected:
	D2D1_SIZE_U GetSizeUFromRect(const RECT& rc, const int scaleFactor) noexcept
	{
		const long width = rc.right - rc.left;
		const long height = rc.bottom - rc.top;
		const UINT32 scaledWidth = width * scaleFactor;
		const UINT32 scaledHeight = height * scaleFactor;
		return D2D1::SizeU(scaledWidth, scaledHeight);
	}

	HRESULT CreateTarget()
	{
		// Creates a D2D render target set on the HWND.
		HRESULT hr = S_OK;
		// Get the window's pixel size.
		RECT rect = {};
		GetClientRect(hwnd_, &rect);
		D2D1_SIZE_U d2dSize = D2D1::SizeU(rect.right, rect.bottom);

		const int integralDeviceScaleFactor = 1.f;
		D2D1_RENDER_TARGET_PROPERTIES drtp{};
		drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
		drtp.dpiX = 96.f * integralDeviceScaleFactor;
		drtp.dpiY = 96.f * integralDeviceScaleFactor;
		// drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);
		drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

		D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
		dhrtp.hwnd = hwnd_;
		dhrtp.pixelSize = GetSizeUFromRect(rect, integralDeviceScaleFactor);
		dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

		// Create a D2D render target.
		ID2D1HwndRenderTarget* target = NULL;
		hr = d2dFactory_->CreateHwndRenderTarget(drtp, dhrtp, &target);
#if 0
		hr = d2dFactory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd_, d2dSize),
			&target
		);
#endif 
		if (SUCCEEDED(hr))
		{
			SafeSet(&target_, target);

			// Any scaling will be combined into matrix transforms rather than an
			// additional DPI scaling. This simplifies the logic for rendering
			// and hit-testing. If an application does not use matrices, then
			// using the scaling factor directly is simpler.
			target->SetDpi(96.0, 96.0);

			// Create a reusable scratch brush, rather than allocating one for
			// each new color.
			SafeRelease(&brush_);
			//hr = target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush_);
			hr = target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush_);
		}

		if (SUCCEEDED(hr))
		{
			// Update the initial monitor rendering parameters.
			UpdateMonitor();
		}

		SafeRelease(&target);

		return hr;
	}
#if 0
	ID2D1Bitmap* GetCachedImage(IWICBitmapSource* image);
#endif 
	ID2D1Brush* GetCachedBrush(const DrawingEffect* effect)
	{
		if (effect == NULL || brush_ == NULL)
			return NULL;
		// Update the D2D brush to the new effect color.
		UINT32 bgra = effect->GetColor();
		float alpha = (bgra >> 24) / 255.0f;
		brush_->SetColor(D2D1::ColorF(bgra, alpha));
		return brush_;
	}

protected:
	IDWriteFactory* dwriteFactory_ = nullptr;
	ID2D1Factory* d2dFactory_ = nullptr;
	ID2D1HwndRenderTarget* target_ = nullptr;     // D2D render target
	ID2D1SolidColorBrush* brush_ = nullptr;       // reusable scratch brush for current color

	//std::vector<ImageCacheEntry> imageCache_;

	HWND hwnd_;
	HMONITOR hmonitor_;
};
#endif 

// look for "https://"
bool IsLinkText(WCHAR* data, U32 length, U32 pos, U32& startPos, U32& endPos)
{
	bool bRet = false;
	startPos = 0;
	endPos = 0;
	if (data&& pos < length)
	{
		if (data[pos] != L' ' && data[pos] != L'\t' && data[pos] != L'\n' && data[pos] != L'\r')
		{
			U32 idx;
			for (idx = pos; idx > 0; idx--)
			{
				if (data[idx] == L' ' || data[idx] == L'\t' || data[idx] == L'\n' || data[idx] == L'\r')
					break;
			}
			if (data[idx] == L' ' || data[idx] == L'\t' || data[idx] == L'\n' || data[idx] == L'\r')
				idx++;
			if (idx < length - 10)
			{
				if (data[idx + 0] == L'h' &&
					data[idx + 1] == L't' &&
					data[idx + 2] == L't' &&
					data[idx + 3] == L'p' &&
					data[idx + 4] == L's' &&
					data[idx + 5] == L':' &&
					data[idx + 6] == L'/' &&
					data[idx + 7] == L'/')
				{
					startPos = idx;
					for (idx = startPos; idx < length; idx++)
					{
						if (data[idx] == L' ' || data[idx] == L'\t' || data[idx] == L'\n' || data[idx] == L'\r')
							break;
					}
					endPos = idx - 1;
					bRet = true;
				}
			}
		}
	}
	return bRet;
}

#define DECLARE_WND_CLASS_TEXT(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_IBEAM, TRUE, 0, _T("") \
	}; \
	return wc; \
}

class CTxtView : public CScrollWindowImpl<CTxtView>
{
	enum SetSelectionMode
	{
		SetSelectionModeLeft,               // cluster left
		SetSelectionModeRight,              // cluster right
		SetSelectionModeUp,                 // line up
		SetSelectionModeDown,               // line down
		SetSelectionModeLeftChar,           // single character left (backspace uses it)
		SetSelectionModeRightChar,          // single character right
		SetSelectionModeLeftWord,           // single word left
		SetSelectionModeRightWord,          // single word right
		SetSelectionModeHome,               // front of line
		SetSelectionModeEnd,                // back of line
		SetSelectionModeFirst,              // very first position
		SetSelectionModeLast,               // very last position
		SetSelectionModeAbsoluteLeading,    // explicit position (for mouse click)
		SetSelectionModeAbsoluteTrailing,   // explicit position, trailing edge
		SetSelectionModeAll                 // select all text
	};

	////////////////////
	// Selection/Caret navigation
	///
	// caretAnchor equals caretPosition when there is no selection.
	// Otherwise, the anchor holds the point where shift was held
	// or left drag started.
	//
	// The offset is used as a sort of trailing edge offset from
	// the caret position. For example, placing the caret on the
	// trailing side of a surrogate pair at the beginning of the
	// text would place the position at zero and offset of two.
	// So to get the absolute leading position, sum the two.
	UINT32 caretAnchor_ = 0;
	UINT32 caretPosition_ = 0;
	UINT32 caretPositionOffset_ = 0;    // > 0 used for trailing edge

	////////////////////
	// Mouse manipulation
	bool currentlySelecting_ : 1;
	bool currentlyPanning_ : 1;
	float previousMouseX = 0;
	float previousMouseY = 0;

	enum { MouseScrollFactor = 10 };

	////////////////////
	// Current view
	float scaleX_;          // horizontal scaling
	float scaleY_;          // vertical scaling
	float angle_;           // in degrees
	float originX_;         // focused point in document (moves on panning and caret navigation)
	float originY_;
	float contentWidth_;    // page size - margin left - margin right (can be fixed or variable)
	float contentHeight_;

	WCHAR* m_dataBuf = nullptr;
	U32    m_dataLen = 0;
	U32    m_dataMax = 0;
	HCURSOR m_hCursor;
	bool  m_CursorChanged = false;

	IDWriteTextFormat* m_textFormat = nullptr;
	IDWriteTextLayout* m_textLayout = nullptr;

	//RenderTarget* renderTarget_ = nullptr;
	IDWriteFactory* dwriteFactory_ = nullptr;
	ID2D1Factory* d2dFactory_ = nullptr;

	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap* m_pixelBitmap = nullptr;
	ID2D1SolidColorBrush* m_pBrush = nullptr;
	ID2D1SolidColorBrush* m_pBrushText = nullptr;
	float m_deviceScaleFactor = 1.f;

public:
	DECLARE_WND_CLASS_TEXT(NULL)

	CTxtView()
	{
		d2dFactory_ = g_pD2DFactory;
		dwriteFactory_ = g_pDWriteFactory;

		caretPosition_ = 0;
		caretAnchor_ = 0;
		caretPositionOffset_ = 0;

		currentlySelecting_ = false;
		currentlyPanning_ = false;
		previousMouseX = 0;
		previousMouseY = 0;

		scaleX_ = 1;
		scaleY_ = 1;
		angle_ = 0;
		originX_ = 0;
		originY_ = 0;

		m_dataLen = 0;
		m_dataMax = (1 << 20);
		m_dataBuf = (WCHAR*)VirtualAlloc(NULL, m_dataMax, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_dataBuf);
	}

	~CTxtView()
	{
		if (m_dataBuf)
		{
			VirtualFree(m_dataBuf, 0, MEM_RELEASE);
			m_dataBuf = nullptr;
			m_dataLen = 0;
			m_dataMax = 0;
		}
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	HRESULT CreateRenderTarget()
	{
		HRESULT hr = S_OK;
#if 0
		RenderTarget* renderTarget = NULL;
		ATLASSERT(d2dFactory_ != NULL);
		ATLASSERT(dwriteFactory_ != NULL);
		hr = RenderTargetD2D::Create(d2dFactory_, dwriteFactory_, m_hWnd, &renderTarget);
		// Set the new target.
		if (SUCCEEDED(hr))
		{
			SafeSet(&renderTarget_, renderTarget);
		}
		SafeRelease(&renderTarget);
#endif 
		return hr;
	}

#if 0
	void DrawPage(RenderTarget& target)
	{
		if (m_textLayout)
		{
			RECT rc;
			GetClientRect(&rc);
			RectF rf = {
				static_cast<float>(rc.left),
				static_cast<float>(rc.top),
				static_cast<float>(rc.right),
				static_cast<float>(rc.bottom)
			};
			target.DrawTextLayout(m_textLayout, rf);
		}
	}
#endif 
	int GetFirstIntegralMultipleDeviceScaleFactor() const noexcept
	{
		return static_cast<int>(std::ceil(m_deviceScaleFactor));
	}

	D2D1_SIZE_U GetSizeUFromRect(const RECT& rc, const int scaleFactor) noexcept
	{
		const long width = rc.right - rc.left;
		const long height = rc.bottom - rc.top;
		const UINT32 scaledWidth = width * scaleFactor;
		const UINT32 scaledHeight = height * scaleFactor;
		return D2D1::SizeU(scaledWidth, scaledHeight);
	}

	HRESULT CreateDeviceDependantResource(int left, int top, int right, int bottom)
	{
		U8 result = 0;
		HRESULT hr = S_OK;
		if (nullptr == m_pD2DRenderTarget)  // Create a Direct2D render target.
		{
			RECT rc = { 0 };
			const int integralDeviceScaleFactor = GetFirstIntegralMultipleDeviceScaleFactor();
			D2D1_RENDER_TARGET_PROPERTIES drtp{};
			drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
			drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
			drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
			drtp.dpiX = 96.f * integralDeviceScaleFactor;
			drtp.dpiY = 96.f * integralDeviceScaleFactor;
			// drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);
			drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

			rc.left = left; rc.top = top; rc.right = right; rc.bottom = bottom;
			D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
			dhrtp.hwnd = m_hWnd;
			dhrtp.pixelSize = GetSizeUFromRect(rc, integralDeviceScaleFactor);
			dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

			ReleaseUnknown(m_pBrush);
			ReleaseUnknown(m_pBrushText);
			ReleaseUnknown(m_pixelBitmap);

			ATLASSERT(nullptr != g_pD2DFactory);

			//hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTragetproperties, &m_pD2DRenderTarget);
			hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);
			if (hr == S_OK && m_pD2DRenderTarget)
			{
				//D2D1::ColorF::LightSkyBlue
				hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x111111), &m_pBrushText);
				if (hr == S_OK && m_pBrushText)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x87CEFA), &m_pBrush);
				}
			}
		}
		return hr;
	}

	DWRITE_TEXT_RANGE GetSelectionRange()
	{
		// Returns a valid range of the current selection,
		// regardless of whether the caret or anchor is first.
		caretAnchor_ = 130;
		caretPosition_ = 250;
		caretPositionOffset_ = 0;

		UINT32 caretBegin = caretAnchor_;
		UINT32 caretEnd = caretPosition_ + caretPositionOffset_;
		if (caretBegin > caretEnd)
			std::swap(caretBegin, caretEnd);

		// Limit to actual text length.
		UINT32 textLength = m_dataLen;
		caretBegin = std::min(caretBegin, textLength);
		caretEnd = std::min(caretEnd, textLength);

		DWRITE_TEXT_RANGE textRange = { caretBegin, caretEnd - caretBegin };
		return textRange;
	}

	void DoPaint(CDCHandle dc)
	{
		POINT pt = { 0 };
		RECT rc = { 0 };
		GetClientRect(&rc);
		GetScrollOffset(pt);

		HRESULT hr = CreateDeviceDependantResource(rc.left, rc.top, rc.right, rc.bottom);
		if (S_OK == hr && nullptr != m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pD2DRenderTarget->Clear(D2D1::ColorF(0xFFFFFF));
			if (m_textLayout)
			{
				DWRITE_TEXT_RANGE caretRange = GetSelectionRange();
				UINT32 actualHitTestCount = 0;
				if (caretRange.length > 0)
				{
					m_textLayout->HitTestTextRange(
						caretRange.startPosition,
						caretRange.length,
						0, // x
						0, // y
						NULL,
						0, // metrics count
						&actualHitTestCount
					);
				}
				// Allocate enough room to return all hit-test metrics.
				std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);

				if (caretRange.length > 0)
				{
					m_textLayout->HitTestTextRange(
						caretRange.startPosition,
						caretRange.length,
						0, // x
						0, // y
						&hitTestMetrics[0],
						static_cast<UINT32>(hitTestMetrics.size()),
						&actualHitTestCount
					);
				}

				// Draw the selection ranges behind the text.
				if (actualHitTestCount > 0)
				{
					// Note that an ideal layout will return fractional values,
					// so you may see slivers between the selection ranges due
					// to the per-primitive antialiasing of the edges unless
					// it is disabled (better for performance anyway).
					m_pD2DRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
					for (size_t i = 0; i < actualHitTestCount; ++i)
					{
						const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
						D2D1_RECT_F highlightRect = {
							htm.left,
							htm.top - pt.y,
							(htm.left + htm.width),
							(htm.top + htm.height - pt.y)
						};
						m_pD2DRenderTarget->FillRectangle(highlightRect, m_pBrush);
					}
					m_pD2DRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				}

				m_pD2DRenderTarget->DrawTextLayout(D2D1::Point2F(-pt.x, -pt.y), m_textLayout,
					m_pBrushText, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
			}
			hr = m_pD2DRenderTarget->EndDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				ReleaseUnknown(m_pD2DRenderTarget);
			}
		}

#if 0
		if (renderTarget_ != NULL) // in case event received before we have a target
		{
			renderTarget_->BeginDraw();
			renderTarget_->Clear(D2D1::ColorF::LightGray);
			DrawPage(*renderTarget_);
			renderTarget_->EndDraw();
		}
#endif 
	}

	BEGIN_MSG_MAP(CTxtView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CScrollWindowImpl<CTxtView>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ReleaseUnknown(m_textLayout);
		ReleaseUnknown(m_textFormat);
#if 1
		ReleaseUnknown(m_pBrush);
		ReleaseUnknown(m_pBrushText);
		ReleaseUnknown(m_pixelBitmap);
		ReleaseUnknown(m_pD2DRenderTarget);
#endif 
		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLASSERT(m_textFormat == nullptr);
		HRESULT hr = dwriteFactory_->CreateTextFormat(L"Courier New", NULL,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			15.5f, L"en-US", &m_textFormat);
#if 0
		ATLASSERT(m_dataBuf == nullptr);

		m_dataLen = 0;
		m_dataMax = (1 << 20);
		m_dataBuf = (WCHAR*)VirtualAlloc(NULL, m_dataMax, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_dataBuf);
		hr = CreateRenderTarget();
		ATLASSERT(hr == S_OK);
#endif
		m_hCursor = ::LoadCursor(NULL, IDC_HAND);
		ATLASSERT(m_hCursor);
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			RECT rc = { 0 };
			GetClientRect(&rc);

			ReleaseUnknown(m_textLayout);
			ATLASSERT(m_textFormat);
			if (m_dataLen && m_dataBuf)
			{
				dwriteFactory_->CreateTextLayout(m_dataBuf, m_dataLen, m_textFormat,
					static_cast<FLOAT>(rc.right - rc.left),
					static_cast<FLOAT>(rc.bottom - rc.top),
					&m_textLayout);
				MakeLinkTextUnderline();
#if 0
				if (m_textLayout)
				{
					DWRITE_TEXT_RANGE tr = { 22, 17 };
					m_textLayout->SetUnderline(TRUE, tr);
				}
#endif 
			}

			ReleaseUnknown(m_pD2DRenderTarget);
			Invalidate();
		}
		return 1;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { 0 };
		GetScrollOffset(pt);

		float xPos = static_cast<float>(GET_X_LPARAM(lParam) - pt.x);
		float yPos = static_cast<float>(GET_Y_LPARAM(lParam) - pt.y);

		m_CursorChanged = false;
		if (m_textLayout)
		{
			BOOL isTrailingHit = FALSE;
			BOOL isInside = FALSE;
			DWRITE_HIT_TEST_METRICS cm = { 0 };

			HRESULT hr = m_textLayout->HitTestPoint(xPos, yPos, &isTrailingHit, &isInside, &cm);
			if (hr == S_OK)
			{
				U32 s = 0, e = 0;
				if (IsLinkText(m_dataBuf, m_dataLen, cm.textPosition, s, e))
				{
					::SetCursor(m_hCursor);
					m_CursorChanged = true;
				}
#if 0
				if (e - s > 0)
				{
					U8 i = 0;
					for (i = 0; i <= (e - s); i++)
						title[i] = m_dataBuf[s + i];
					title[i] = L'\0';
				}
				else
				{
					swprintf((wchar_t*)title, MAX_PATH, L"XEdit - tp[%u],len[%u],left[%f],top[%f],w[%f],h[%f],bi[%u],it[%d],trim[%d]",
						cm.textPosition,
						cm.length,
						cm.left,
						cm.top,
						cm.width,
						cm.height,
						cm.bidiLevel,
						cm.isText,
						cm.isTrimmed
					);
				}
				::SetWindowTextW(GetParent(), title);
#endif 
			}
		}

		return 1;
	}

	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		if (m_CursorChanged == false)
		{
			bHandled = FALSE;
		}
		m_CursorChanged = false;
		return 0;
	}

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 1;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 1;
	}

	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 1;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 1;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		float xPos = static_cast<float>(GET_X_LPARAM(lParam));
		float yPos = static_cast<float>(GET_Y_LPARAM(lParam));

		m_CursorChanged = false;
		if (m_textLayout)
		{
			BOOL isTrailingHit = FALSE;
			BOOL isInside = FALSE;
			DWRITE_HIT_TEST_METRICS cm = { 0 };

			HRESULT hr = m_textLayout->HitTestPoint(xPos, yPos, &isTrailingHit, &isInside, &cm);
			if (hr == S_OK)
			{
				U32 s = 0, e = 0;
				if (IsLinkText(m_dataBuf, m_dataLen, cm.textPosition, s, e))
				{
					::SetCursor(m_hCursor);
					m_CursorChanged = true;
				}
				if (e - s > 0)
				{
#if 0
					U8 i = 0;
					for (i = 0; i <= (e - s); i++)
						title[i] = m_dataBuf[s + i];
					title[i] = L'\0';
#endif 
					MessageBox(L"LINK", L"LINK", MB_OK);
				}
			}
		}
		return 1;
	}

	int AppendText(const char* text, U32 length)
	{
		int r = 0;
		if (text && length && m_dataBuf)
		{
			U32 utf16len = 0;
			U32 status = wt_UTF8ToUTF16((U8*)text, length, NULL, &utf16len);
			if (status == WT_OK && utf16len && utf16len < m_dataMax - m_dataLen - 1)
			{
				RECT rc;
				GetClientRect(&rc);

				WCHAR* p = m_dataBuf + m_dataLen;
				status = wt_UTF8ToUTF16((U8*)text, length, (U16*)p, NULL);
				ATLASSERT(status == WT_OK);
				m_dataLen += utf16len;
				m_dataBuf[m_dataLen] = L'\0';

				ReleaseUnknown(m_textLayout);
				ATLASSERT(m_textFormat);
				dwriteFactory_->CreateTextLayout(m_dataBuf, m_dataLen, m_textFormat,
					static_cast<FLOAT>(rc.right - rc.left),
					static_cast<FLOAT>(1),
					&m_textLayout);

				MakeLinkTextUnderline();
				if (m_textLayout)
				{
					int hI;
					DWRITE_TEXT_METRICS textMetrics;
					m_textLayout->GetMetrics(&textMetrics);
					float hf = textMetrics.layoutHeight;
					if (hf < textMetrics.height)
						hf = textMetrics.height;

					hI = static_cast<int>(hf + 1);
					if(hI > rc.bottom - rc.top)
						SetScrollSize(1, hI);
#if 0
					DrawingEffect* drawingEffect1 = SafeAcquire(new(std::nothrow) DrawingEffect(0xFF1010D0));
					DrawingEffect* drawingEffect2 = SafeAcquire(new(std::nothrow) DrawingEffect(0xFF10D010));
					m_textLayout->SetDrawingEffect(drawingEffect1, MakeDWriteTextRange(0, 3));
					m_textLayout->SetDrawingEffect(drawingEffect2, MakeDWriteTextRange(3, 6));
					SafeRelease(&drawingEffect2);
					SafeRelease(&drawingEffect1);
#endif 
				}
				Invalidate();
			}
		}
		return r;
	}

	// search https://
	void MakeLinkTextUnderline()
	{
		if (m_textLayout && m_dataLen > 10 && m_dataBuf)
		{
			WCHAR* q;
			WCHAR* p = m_dataBuf;
			U32 i, pos = 0;
			DWRITE_TEXT_RANGE tr = { 0 };
			for (i = 0; i < m_dataLen - 10; i++)
			{
				if (p[i + 0] == L'h' &&
					p[i + 1] == L't' &&
					p[i + 2] == L't' &&
					p[i + 3] == L'p' &&
					p[i + 4] == L's' &&
					p[i + 5] == L':' &&
					p[i + 6] == L'/' &&
					p[i + 7] == L'/' &&
					(p[i + 8] != L' ' && p[i + 8] != L'\t' && p[i + 8] != L'\n' && p[i + 8] != L'\r')
					)
				{
					tr.startPosition = i;
					pos = 8;
					q = p + 8;
					while (*q != L'\0' && *q != L' ' && *q != L'\t' && *q != L'\n' && *q != L'\r')
					{
						q++;
						pos++;
					}
					tr.length = pos;
					m_textLayout->SetUnderline(TRUE, tr);
					i += pos;
				}
			}
		}
	}
};
#endif 

class CTxtView : public CScrollWindowImpl<CTxtView>
{
public:
	DECLARE_WND_CLASS(NULL)

		BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	void DoPaint(CDCHandle dc)
	{
		//TODO: Add your drawing code here
	}

	int AppendText(const char* text, U32 length)
	{
		return 0;
	}

	BEGIN_MSG_MAP(CTxtView)
		CHAIN_MSG_MAP(CScrollWindowImpl<CTxtView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
