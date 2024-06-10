// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "Implementation.h"

#include "Handle.h"
#include "../buffer/out/TextAttribute.hpp"
#include "../renderer/base/renderer.hpp"

#pragma warning(disable : 4100) // '...': unreferenced formal parameter

using namespace Microsoft::Console::TSF;

static void TfSelectionClose(const TF_SELECTION* sel)
{
    if (const auto r = sel->range)
    {
        r->Release();
    }
}
using unique_tf_selection = wil::unique_struct<TF_SELECTION, decltype(&TfSelectionClose), &TfSelectionClose>;

static void TfPropertyvalClose(TF_PROPERTYVAL* val)
{
    VariantClear(&val->varValue);
}
using unique_tf_propertyval = wil::unique_struct<TF_PROPERTYVAL, decltype(&TfPropertyvalClose), &TfPropertyvalClose>;

void Implementation::Initialize()
{
    _categoryMgr = wil::CoCreateInstance<ITfCategoryMgr>(CLSID_TF_CategoryMgr, CLSCTX_INPROC_SERVER);
    _displayAttributeMgr = wil::CoCreateInstance<ITfDisplayAttributeMgr>(CLSID_TF_DisplayAttributeMgr);

    // There's no point in calling TF_GetThreadMgr. ITfThreadMgr is a per-thread singleton.
    _threadMgrEx = wil::CoCreateInstance<ITfThreadMgrEx>(CLSID_TF_ThreadMgr, CLSCTX_INPROC_SERVER);

    THROW_IF_FAILED(_threadMgrEx->ActivateEx(&_clientId, TF_TMAE_CONSOLE));
    THROW_IF_FAILED(_threadMgrEx->CreateDocumentMgr(_documentMgr.addressof()));

    TfEditCookie ecTextStore;
    THROW_IF_FAILED(_documentMgr->CreateContext(_clientId, 0, static_cast<ITfContextOwnerCompositionSink*>(this), _context.addressof(), &ecTextStore));

    _contextSource = _context.query<ITfSource>();
    THROW_IF_FAILED(_contextSource->AdviseSink(IID_ITfContextOwner, static_cast<ITfContextOwner*>(this), &_cookieContextOwner));
    THROW_IF_FAILED(_contextSource->AdviseSink(IID_ITfTextEditSink, static_cast<ITfTextEditSink*>(this), &_cookieTextEditSink));

    THROW_IF_FAILED(_documentMgr->Push(_context.get()));
}

void Implementation::Uninitialize() noexcept
{
    _provider.reset();

    if (_associatedHwnd)
    {
        wil::com_ptr<ITfDocumentMgr> prev;
        std::ignore = _threadMgrEx->AssociateFocus(_associatedHwnd, nullptr, prev.addressof());
    }

    if (_cookieTextEditSink != TF_INVALID_COOKIE)
    {
        std::ignore = _contextSource->UnadviseSink(_cookieTextEditSink);
    }
    if (_cookieContextOwner != TF_INVALID_COOKIE)
    {
        std::ignore = _contextSource->UnadviseSink(_cookieContextOwner);
    }

    if (_documentMgr)
    {
        std::ignore = _documentMgr->Pop(TF_POPF_ALL);
    }
    if (_threadMgrEx)
    {
        std::ignore = _threadMgrEx->Deactivate();
    }
}

HWND Implementation::FindWindowOfActiveTSF() const noexcept
{
    wil::com_ptr<IEnumTfDocumentMgrs> enumDocumentMgrs;
    if (FAILED_LOG(_threadMgrEx->EnumDocumentMgrs(enumDocumentMgrs.addressof())))
    {
        return nullptr;
    }

    wil::com_ptr<ITfDocumentMgr> document;
    if (FAILED_LOG(enumDocumentMgrs->Next(1, document.addressof(), nullptr)))
    {
        return nullptr;
    }

    wil::com_ptr<ITfContext> context;
    if (FAILED_LOG(document->GetTop(context.addressof())))
    {
        return nullptr;
    }

    wil::com_ptr<ITfContextView> view;
    if (FAILED_LOG(context->GetActiveView(view.addressof())))
    {
        return nullptr;
    }

    HWND hwnd;
    if (FAILED_LOG(view->GetWnd(&hwnd)))
    {
        return nullptr;
    }

    return hwnd;
}

void Implementation::AssociateFocus(IDataProvider* provider)
{
    _provider = provider;
    _associatedHwnd = _provider->GetHwnd();

    wil::com_ptr<ITfDocumentMgr> prev;
    THROW_IF_FAILED(_threadMgrEx->AssociateFocus(_associatedHwnd, _documentMgr.get(), prev.addressof()));
}

void Implementation::Focus(IDataProvider* provider)
{
    _provider = provider;

    THROW_IF_FAILED(_threadMgrEx->SetFocus(_documentMgr.get()));
}

void Implementation::Unfocus(IDataProvider* provider)
{
    if (!_provider || _provider != provider)
    {
        return;
    }

    {
        const auto renderer = _provider->GetRenderer();
        const auto renderData = renderer->GetRenderData();

        renderData->LockConsole();
        const auto unlock = wil::scope_exit([&]() {
            renderData->UnlockConsole();
        });

        if (!renderData->activeComposition.text.empty())
        {
            auto& comp = renderData->activeComposition;
            comp.text.clear();
            comp.attributes.clear();
            renderer->NotifyPaintFrame();
        }
    }

    _provider.reset();

    if (_compositions > 0)
    {
        if (const auto svc = _context.try_query<ITfContextOwnerCompositionServices>())
        {
            svc->TerminateComposition(nullptr);
        }
    }
}

bool Implementation::HasActiveComposition() const noexcept
{
    return _compositions > 0;
}

#pragma region IUnknown

STDMETHODIMP Implementation::QueryInterface(REFIID riid, void** ppvObj) noexcept
{
    if (!ppvObj)
    {
        return E_POINTER;
    }

    if (IsEqualGUID(riid, IID_ITfContextOwner))
    {
        *ppvObj = static_cast<ITfContextOwner*>(this);
    }
    else if (IsEqualGUID(riid, IID_ITfContextOwnerCompositionSink))
    {
        *ppvObj = static_cast<ITfContextOwnerCompositionSink*>(this);
    }
    else if (IsEqualGUID(riid, IID_ITfTextEditSink))
    {
        *ppvObj = static_cast<ITfTextEditSink*>(this);
    }
    else if (IsEqualGUID(riid, IID_IUnknown))
    {
        *ppvObj = static_cast<IUnknown*>(static_cast<ITfContextOwner*>(this));
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE Implementation::AddRef() noexcept
{
    return InterlockedIncrement(&_referenceCount);
}

ULONG STDMETHODCALLTYPE Implementation::Release() noexcept
{
    const auto r = InterlockedDecrement(&_referenceCount);
    if (r == 0)
    {
        delete this;
    }
    return r;
}

#pragma endregion IUnknown

#pragma region ITfContextOwner

STDMETHODIMP Implementation::GetACPFromPoint(const POINT* ptScreen, DWORD dwFlags, LONG* pacp) noexcept
{
    assert(false);
    return E_NOTIMPL;
}

// This returns rectangle of current command line edit area.
// When a user types in East Asian language, candidate window is shown at this position.
// Emoji and more panel (Win+.) is shown at the position, too.
STDMETHODIMP Implementation::GetTextExt(LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) noexcept
try
{
    if (prc)
    {
        *prc = _provider ? _provider->GetCursorPosition() : RECT{};
    }

    if (pfClipped)
    {
        *pfClipped = FALSE;
    }

    return S_OK;
}
CATCH_RETURN()

// This returns Rectangle of the text box of whole console.
// When a user taps inside the rectangle while hardware keyboard is not available, touch keyboard is invoked.
STDMETHODIMP Implementation::GetScreenExt(RECT* prc) noexcept
try
{
    if (prc)
    {
        *prc = _provider ? _provider->GetViewport() : RECT{};
    }

    return S_OK;
}
CATCH_RETURN()

STDMETHODIMP Implementation::GetStatus(TF_STATUS* pdcs) noexcept
{
    if (pdcs)
    {
        pdcs->dwDynamicFlags = 0;
        // The use of TF_SS_TRANSITORY / TS_SS_TRANSITORY is incredibly important...
        // ...and it has the least complete description:
        // > TS_SS_TRANSITORY: The document is expected to have a short usage cycle.
        //
        // Proper documentation about the flag has been lost and can only be found via archive.org:
        //   http://web.archive.org/web/20140520210042/http://blogs.msdn.com/b/tsfaware/archive/2007/04/25/transitory-contexts.aspx
        // It states:
        // > The most significant difference is that Transitory contexts don't retain state - once you end the composition [...],
        // > any knowledge of the document (or any previous insertions/modifications/etc.) is gone.
        // In other words, non-transitory contexts expect access to previously completed contents, which is something we cannot provide.
        // Because once some text has finished composition we'll immediately send it to the shell via HandleOutput(), which we cannot undo.
        // It's also the primary reason why we cannot use the WinRT CoreTextServices APIs, as they don't set TS_SS_TRANSITORY.
        //
        // Additionally, "short usage cycle" also significantly undersells another importance of the flag:
        // If set, it enables CUAS, the Cicero Unaware Application Support, which is an emulation layer that fakes IMM32.
        // Cicero is the internal code name for TSF. In other words, "TS_SS_TRANSITORY" = "Disable modern TSF".
        // This results in a couple modern composition features not working (Korean reconversion primarily),
        // but it's a trade-off we're forced to make, because otherwise it doesn't work at all.
        //
        // TS_SS_NOHIDDENTEXT tells TSF that we don't support TS_RT_HIDDEN, which is used if a document contains hidden markup
        // inside the text. For instance an HTML document contains tags which aren't visible, but nonetheless exist.
        // It's not publicly documented, but allegedly specifying this flag results in a minor performance uplift.
        // Ironically, the only two places that mention this flag internally state:
        // > perf: we could check TS_SS_NOHIDDENTEXT for better perf
        pdcs->dwStaticFlags = TS_SS_TRANSITORY | TS_SS_NOHIDDENTEXT;
    }

    return S_OK;
}

STDMETHODIMP Implementation::GetWnd(HWND* phwnd) noexcept
{
    *phwnd = _provider ? _provider->GetHwnd() : nullptr;
    return S_OK;
}

STDMETHODIMP Implementation::GetAttribute(REFGUID rguidAttribute, VARIANT* pvarValue) noexcept
{
    return E_NOTIMPL;
}

#pragma endregion ITfContextOwner

#pragma region ITfContextOwnerCompositionSink

STDMETHODIMP Implementation::OnStartComposition(ITfCompositionView* pComposition, BOOL* pfOk) noexcept
try
{
    _compositions++;
    *pfOk = TRUE;
    return S_OK;
}
CATCH_RETURN()

STDMETHODIMP Implementation::OnUpdateComposition(ITfCompositionView* pComposition, ITfRange* pRangeNew) noexcept
{
    return S_OK;
}

STDMETHODIMP Implementation::OnEndComposition(ITfCompositionView* pComposition) noexcept
try
{
    if (_compositions <= 0)
    {
        return E_FAIL;
    }

    _compositions--;
    if (_compositions == 0)
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/msctf/nf-msctf-itfcontext-requesteditsession
        // > A text service can request an edit session within the context of an existing edit session,
        // > provided a write access session is not requested within a read-only session.
        // --> Requires TF_ES_ASYNC to work properly. TF_ES_ASYNCDONTCARE randomly fails because... TSF.
        std::ignore = _request(_editSessionCompositionUpdate, TF_ES_READWRITE | TF_ES_ASYNC);
    }

    return S_OK;
}
CATCH_RETURN()

#pragma endregion ITfContextOwnerCompositionSink

#pragma region ITfTextEditSink

STDMETHODIMP Implementation::OnEndEdit(ITfContext* pic, TfEditCookie ecReadOnly, ITfEditRecord* pEditRecord) noexcept
try
{
    if (_compositions == 1)
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/msctf/nf-msctf-itfcontext-requesteditsession
        // > A text service can request an edit session within the context of an existing edit session,
        // > provided a write access session is not requested within a read-only session.
        // --> Requires TF_ES_ASYNC to work properly. TF_ES_ASYNCDONTCARE randomly fails because... TSF.
        std::ignore = _request(_editSessionCompositionUpdate, TF_ES_READWRITE | TF_ES_ASYNC);
    }

    return S_OK;
}
CATCH_RETURN()

#pragma endregion ITfTextEditSink

Implementation::EditSessionProxyBase::EditSessionProxyBase(Implementation* self) noexcept :
    self{ self }
{
}

STDMETHODIMP Implementation::EditSessionProxyBase::QueryInterface(REFIID riid, void** ppvObj) noexcept
{
    if (!ppvObj)
    {
        return E_POINTER;
    }

    if (IsEqualGUID(riid, IID_ITfEditSession))
    {
        *ppvObj = static_cast<ITfEditSession*>(this);
    }
    else if (IsEqualGUID(riid, IID_IUnknown))
    {
        *ppvObj = static_cast<IUnknown*>(this);
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE Implementation::EditSessionProxyBase::AddRef() noexcept
{
    return InterlockedIncrement(&referenceCount);
}

ULONG STDMETHODCALLTYPE Implementation::EditSessionProxyBase::Release() noexcept
{
    FAIL_FAST_IF(referenceCount == 0);
    return InterlockedDecrement(&referenceCount);
}

[[nodiscard]] HRESULT Implementation::_request(EditSessionProxyBase& session, DWORD flags) const
{
    // Some of the sessions are async, and we don't want to send another request if one is still in flight.
    if (session.referenceCount)
    {
        return S_FALSE;
    }

    HRESULT hr = S_OK;
    THROW_IF_FAILED(_context->RequestEditSession(_clientId, &session, flags, &hr));
    RETURN_IF_FAILED(hr);
    return S_OK;
}

void Implementation::_doCompositionUpdate(TfEditCookie ec)
{
    wil::com_ptr<ITfRange> fullRange;
    LONG fullRangeLength;
    THROW_IF_FAILED(_context->GetStart(ec, fullRange.addressof()));
    THROW_IF_FAILED(fullRange->ShiftEnd(ec, LONG_MAX, &fullRangeLength, nullptr));

    std::wstring finalizedString;
    std::wstring activeComposition;
    til::small_vector<Render::CompositionRange, 2> activeCompositionRanges;
    bool activeCompositionEncountered = false;

    const GUID* guids[] = { &GUID_PROP_COMPOSING, &GUID_PROP_ATTRIBUTE };
    wil::com_ptr<ITfReadOnlyProperty> props;
    THROW_IF_FAILED(_context->TrackProperties(&guids[0], ARRAYSIZE(guids), nullptr, 0, props.addressof()));

    wil::com_ptr<IEnumTfRanges> enumRanges;
    THROW_IF_FAILED(props->EnumRanges(ec, enumRanges.addressof(), fullRange.get()));

    // IEnumTfRanges::Next returns S_FALSE when it has reached the end of the list.
    // This includes any call where the number of returned items is less than what was requested.
    for (HRESULT nextResult = S_OK; nextResult == S_OK;)
    {
        ITfRange* ranges[8];
        ULONG rangesCount;
        nextResult = enumRanges->Next(ARRAYSIZE(ranges), &ranges[0], &rangesCount);

        const auto cleanup = wil::scope_exit([&] {
            for (ULONG i = 0; i < rangesCount; ++i)
            {
                ranges[i]->Release();
            }
        });

        for (ULONG i = 0; i < rangesCount; ++i)
        {
            const auto range = ranges[i];

            bool composing = false;
            TfGuidAtom atom = TF_INVALID_GUIDATOM;
            {
                wil::unique_variant var;
                THROW_IF_FAILED(props->GetValue(ec, range, var.addressof()));

                wil::com_ptr<IEnumTfPropertyValue> propVal;
                wil::com_query_to(var.punkVal, propVal.addressof());

                unique_tf_propertyval propVals[2];
                THROW_IF_FAILED(propVal->Next(2, propVals[0].addressof(), nullptr));

                for (const auto& val : propVals)
                {
                    if (IsEqualGUID(val.guidId, GUID_PROP_COMPOSING))
                    {
                        composing = V_VT(&val.varValue) == VT_I4 && V_I4(&val.varValue) != 0;
                    }
                    else if (IsEqualGUID(val.guidId, GUID_PROP_ATTRIBUTE))
                    {
                        atom = V_VT(&val.varValue) == VT_I4 ? static_cast<TfGuidAtom>(V_I4(&val.varValue)) : TF_INVALID_GUIDATOM;
                    }
                }
            }

            size_t totalLen = 0;
            for (;;)
            {
                // GetText() won't throw if the range is empty. It'll simply return len == 0.
                // However, you'll likely never see this happen with a bufCap this large (try 16 instead or something).
                // It seems TSF doesn't support such large compositions in any language.
                static constexpr ULONG bufCap = 128;
                WCHAR buf[bufCap];
                ULONG len = bufCap;
                THROW_IF_FAILED(range->GetText(ec, TF_TF_MOVESTART, buf, len, &len));

                // Since we can't un-finalize finalized text, we only finalize text that's at the start of the document.
                // In other words, don't put text that's in the middle between two active compositions into the finalized string.
                if (!composing && !activeCompositionEncountered)
                {
                    finalizedString.append(buf, len);
                }
                else
                {
                    activeComposition.append(buf, len);
                }

                totalLen += len;

                if (len < bufCap)
                {
                    break;
                }
            }

            const auto attr = _textAttributeFromAtom(atom);
            activeCompositionRanges.emplace_back(totalLen, attr);

            activeCompositionEncountered |= composing;
        }
    }

    LONG cursorPos = LONG_MAX;
    {
        // According to the docs this may result in TF_E_NOSELECTION. While I haven't actually seen that happen myself yet,
        // I don't want this to result in log-spam, which is why this doesn't use SUCCEEDED_LOG().
        unique_tf_selection sel;
        ULONG selCount;
        if (SUCCEEDED(_context->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &sel, &selCount)) && selCount == 1)
        {
            wil::com_ptr<ITfRange> start;
            THROW_IF_FAILED(_context->GetStart(ec, start.addressof()));

            TF_HALTCOND hc{
                .pHaltRange = sel.range,
                .aHaltPos = sel.style.ase == TF_AE_START ? TF_ANCHOR_START : TF_ANCHOR_END,
            };
            THROW_IF_FAILED(start->ShiftEnd(ec, LONG_MAX, &cursorPos, &hc));
        }

        // Compensate for the fact that we'll be erasing the start of the string below.
        cursorPos -= static_cast<LONG>(finalizedString.size());
        cursorPos = std::clamp(cursorPos, 0l, static_cast<LONG>(activeComposition.size()));
    }

    if (!finalizedString.empty())
    {
        // Erase the text that's done with composition from the context.
        wil::com_ptr<ITfRange> range;
        LONG cch;
        THROW_IF_FAILED(_context->GetStart(ec, range.addressof()));
        THROW_IF_FAILED(range->ShiftEnd(ec, static_cast<LONG>(finalizedString.size()), &cch, nullptr));
        THROW_IF_FAILED(range->SetText(ec, 0, nullptr, 0));
    }

    if (_provider)
    {
        {
            const auto renderer = _provider->GetRenderer();
            const auto renderData = renderer->GetRenderData();

            renderData->LockConsole();
            const auto unlock = wil::scope_exit([&]() {
                renderData->UnlockConsole();
            });

            auto& comp = renderData->activeComposition;
            comp.text = std::move(activeComposition);
            comp.attributes = std::move(activeCompositionRanges);
            // The code block above that calculates the `cursorPos` will clamp it to a positive number.
            comp.cursorPos = static_cast<size_t>(cursorPos);
            renderer->NotifyPaintFrame();
        }

        if (!finalizedString.empty())
        {
            _provider->HandleOutput(finalizedString);
        }
    }
}

TextAttribute Implementation::_textAttributeFromAtom(TfGuidAtom atom) const
{
    TextAttribute attr;

    // You get TF_INVALID_GUIDATOM by (for instance) using the Vietnamese Telex IME.
    // A dashed underline is used because that's what Firefox used at the time and it
    // looked kind of neat. In the past, conhost used a blue background and white text.
    if (atom == TF_INVALID_GUIDATOM)
    {
        attr.SetUnderlineStyle(UnderlineStyle::DashedUnderlined);
        return attr;
    }

    GUID guid;
    if (FAILED_LOG(_categoryMgr->GetGUID(atom, &guid)))
    {
        return attr;
    }

    wil::com_ptr<ITfDisplayAttributeInfo> dai;
    if (FAILED_LOG(_displayAttributeMgr->GetDisplayAttributeInfo(guid, dai.addressof(), nullptr)))
    {
        return attr;
    }

    TF_DISPLAYATTRIBUTE da;
    THROW_IF_FAILED(dai->GetAttributeInfo(&da));

    if (da.crText.type != TF_CT_NONE)
    {
        attr.SetForeground(_colorFromDisplayAttribute(da.crText));
    }
    if (da.crBk.type != TF_CT_NONE)
    {
        attr.SetBackground(_colorFromDisplayAttribute(da.crBk));
    }
    if (da.lsStyle >= TF_LS_NONE && da.lsStyle <= TF_LS_SQUIGGLE)
    {
        static constexpr UnderlineStyle lut[] = {
            /* TF_LS_NONE     */ UnderlineStyle::NoUnderline,
            /* TF_LS_SOLID    */ UnderlineStyle::SinglyUnderlined,
            /* TF_LS_DOT      */ UnderlineStyle::DottedUnderlined,
            /* TF_LS_DASH     */ UnderlineStyle::DashedUnderlined,
            /* TF_LS_SQUIGGLE */ UnderlineStyle::CurlyUnderlined,
        };
        attr.SetUnderlineStyle(lut[da.lsStyle]);
    }
    // You can reproduce bold lines with the Japanese IME by typing "kyouhaishaheiku" and pressing space.
    // The IME will allow you to navigate between the 3 parts of the composition and the current one is
    // marked as fBoldLine. We don't support bold lines so we just use a double underline instead.
    if (da.fBoldLine)
    {
        attr.SetUnderlineStyle(UnderlineStyle::DoublyUnderlined);
    }
    if (da.crLine.type != TF_CT_NONE)
    {
        attr.SetUnderlineColor(_colorFromDisplayAttribute(da.crLine));
    }

    return attr;
}

COLORREF Implementation::_colorFromDisplayAttribute(TF_DA_COLOR color)
{
    switch (color.type)
    {
    case TF_CT_SYSCOLOR:
        return GetSysColor(color.nIndex);
    case TF_CT_COLORREF:
        return color.cr;
    default:
        // If you get here you either called this when .type is TF_CT_NONE
        // (don't call in that case; there's no color to be had), or
        // there's a new .type which you need to add.
        assert(false);
        return 0;
    }
}
