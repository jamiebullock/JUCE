/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/




/*
class CHTMLWriter
{
protected:
	IWebBrowser2 * m_pWB;
	::IDispatch * m_pDoc;
	bool m_bReleaseIWebBrowserPtr;

public:
	CHTMLWriter(IWebBrowser2 * pWB)
	{
		m_bReleaseIWebBrowserPtr = false;
		m_pDoc = NULL;
		m_pWB = pWB;
	}


	~CHTMLWriter(void)
	{
		if (m_pDoc)
			m_pDoc->Release();
		if (m_bReleaseIWebBrowserPtr && m_pWB)
			m_pWB->Release();
	}
	bool Write(LPCTSTR pszHTMLContent)
	{
		if (!pszHTMLContent)
			return false;
		if (!GetDocumentPtr())
			return false;

		IStream * pStream = NULL;
		IPersistStreamInit * pPSI = NULL;
		HGLOBAL hHTMLContent;
		HRESULT hr;
		bool bResult = false;

		// allocate global memory to copy the HTML content to
		hHTMLContent = ::GlobalAlloc(GPTR, (::_tcslen(pszHTMLContent) + 1) * sizeof(TCHAR));
		if (!hHTMLContent)
			return false;

		::_tcscpy((TCHAR *)hHTMLContent, pszHTMLContent);

		// create a stream object based on the HTML content
		hr = ::CreateStreamOnHGlobal(hHTMLContent, TRUE, &pStream);
		if (SUCCEEDED(hr))
		{
			// request the IPersistStreamInit interface
			hr = m_pDoc->QueryInterface(IID_IPersistStreamInit, (void **)&pPSI);

			if (SUCCEEDED(hr))
			{
				// initialize the persist stream object
				hr = pPSI->InitNew();

				if (SUCCEEDED(hr))
				{
					// load the data into it
					hr = pPSI->Load(pStream);

					if (SUCCEEDED(hr))
						bResult = true;
				}

				pPSI->Release();
			}

			// implicitly calls ::GlobalFree to free the global memory
			pStream->Release();
		}

		return bResult;
	}
	bool Add(LPCTSTR pszHTMLContent)
	{
		if (!pszHTMLContent)
			return false;
		if (!GetDocumentPtr())
			return false;

		IHTMLDocument2 * pHTMLDoc = NULL;
		::IHTMLElement * pElem = NULL;
		HRESULT hr;
		bool bResult = false;

		// get an interface to the document object
		hr = m_pDoc->QueryInterface(IID_IHTMLDocument2, (void **)&pHTMLDoc);

		if (SUCCEEDED(hr))
		{
			// get the body element of the document
			hr = pHTMLDoc->get_body(&pElem);

			if (SUCCEEDED(hr))
			{
				// retrieve all the HTML content of the body object
				BSTR bstr;
				hr = pElem->get_innerHTML(&bstr);

				if (SUCCEEDED(hr))
				{
					// append the desired content to the HTML content of the document
					_bstr_t bstrContent(bstr);
					bstrContent += pszHTMLContent;

					// set the new HTML content of the document
					hr = pElem->put_innerHTML(bstrContent);

					if (SUCCEEDED(hr))
						bResult = true;

					::SysFreeString(bstr);
				}

				pElem->Release();
			}

			pHTMLDoc->Release();
		}
		return bResult;

	}

private:
	CHTMLWriter(void) {};
	bool GetDocumentPtr(void)
	{
		if (!m_pWB)
			return false;
		if (m_pDoc)
		{
			m_pDoc->Release();
			m_pDoc = NULL;
		}

		IDispatch * pDisp = NULL;
		HRESULT hr;

		// get the document's IDispatch*
		hr = m_pWB->get_Document(&pDisp);
		if (SUCCEEDED(hr))
		{
			m_pDoc = pDisp;
			return true;
		}
		else
			return false;
	}
};
*/

JUCE_COMCLASS (DWebBrowserEvents2,        "34A715A0-6587-11D0-924A-0020AFC7AC4D")
JUCE_COMCLASS (IConnectionPointContainer, "B196B284-BAB4-101A-B69C-00AA00341D07")
JUCE_COMCLASS (IWebBrowser2,              "D30C1661-CDAF-11D0-8A3E-00C04FC9E26E")
JUCE_COMCLASS (WebBrowser,                "8856F961-340A-11D0-A96B-00C04FD705A2")

static HHOOK mouseWheelHook2 = 0, keyboardHook2 = 0;


class WebBrowserComponent::Pimpl   : public ActiveXControlComponent
{
public:
    Pimpl()
      : browser (nullptr),
        connectionPoint (nullptr),
        adviseCookie (0)
    {
		
		

		keyboardHook2 = SetWindowsHookEx(WH_GETMESSAGE, keyboardHookCallback,
			(HINSTANCE)Process::getCurrentModuleInstanceHandle(),
			GetCurrentThreadId());
	/*	
		mouseWheelHook2 = SetWindowsHookEx(WH_MOUSE, mouseWheelHookCallback,
			NULL,
			GetCurrentThreadId());
		*/	

    }
	
    ~Pimpl()
    {
		if (keyboardHook2 != 0)
		{
			UnhookWindowsHookEx(keyboardHook2);
			keyboardHook2 = 0;
		}

        if (connectionPoint != nullptr)
            connectionPoint->Unadvise (adviseCookie);

        if (browser != nullptr)
            browser->Release();
    }

	static bool handleKeyDown(const WPARAM key, HWND target)
	{
		bool used = false;

		switch (key)
		{
		/*case VK_TAB:
		{
			
			}
			used = true;
				break;*/

		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_CONTROL:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
		case VK_LWIN:
		case VK_RWIN:
		case VK_CAPITAL:
		case VK_NUMLOCK:
		case VK_SCROLL:
		case VK_APPS:
		//	used = false;
			break;

		case VK_ESCAPE:
		case VK_TAB:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_HOME:
		case VK_END:
		case VK_DELETE:
		case VK_INSERT:
		case VK_BACK:
		//case VK_F1:
		//case VK_F2:
		//case VK_F3:
		//case VK_F4:
		//case VK_F5:
		//case VK_F6:
		//case VK_F7:
		//case VK_F8:
		//case VK_F9:
		//case VK_F10:
		//case VK_F11:
		//case VK_F12:
		//case VK_F13:
		//case VK_F14:
		//case VK_F15:
		//case VK_F16:
			//used = handleKeyUpOrDown(true);
			//used = handleKeyPress(extendedKeyModifier | (int)key, 0) || used;
			//SendMessage(target, WM_KEYDOWN, (WPARAM)key, 0);
			used = true;
			break;

		default:
			//used = handleKeyUpOrDown(true);

			{
				MSG msg;
				if (!PeekMessage(&msg, NULL, WM_CHAR, WM_DEADCHAR, PM_NOREMOVE))
				{
					// if there isn't a WM_CHAR or WM_DEADCHAR message pending, we need to
					// manually generate the key-press event that matches this key-down.
					const UINT keyChar = MapVirtualKey((UINT)key, 2);
					const UINT scanCode = MapVirtualKey((UINT)key, 0);
					BYTE keyState[256];
					GetKeyboardState(keyState);

					WCHAR text[16] = { 0 };
					if (ToUnicode((UINT)key, scanCode, keyState, text, 8, 0) != 1)
						text[0] = 0;
					SendMessage(target, WM_CHAR, *text, 0);
					used = true;

					//used = handleKeyPress((int)LOWORD(keyChar), (juce_wchar)text[0]) || used;
				}
			}

			break;
		}
		
		return used;
	}

	static bool handleKeyUp(const WPARAM key, HWND target)
	{
		bool used = false;

		switch (key)
		{
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_CONTROL:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
		case VK_LWIN:
		case VK_RWIN:
		case VK_CAPITAL:
		case VK_NUMLOCK:
		case VK_SCROLL:
		case VK_APPS:
			break;
		default:
			used = true;
		}

		return used;
	}

	static bool offerKeyMessageToBrowserWindow(MSG& m)
	{
		if (m.message == WM_KEYDOWN || m.message == WM_KEYUP)
		{
			if (sBrowser != nullptr)
			{
				HWND newHandle = GetFocus();
				
				if (newHandle != NULL)
				{
					char class_name[256];
					GetClassNameA(newHandle, class_name, 256);

					if (String(class_name) != "Internet Explorer_Server")
					{
						return false;
					}

					IOleInPlaceActiveObject* pIOIPAO;

					// Needed to make Tab key work
					HRESULT hr = sBrowser->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pIOIPAO);
					if (SUCCEEDED(hr))
					{
						if (pIOIPAO->TranslateAccelerator(&m) == 0)
						{
							TranslateMessage(&m);
							//DispatchMessage(&m);
						}
						pIOIPAO->Release();
					}

					return m.message == WM_KEYDOWN ? handleKeyDown(m.wParam, newHandle) : handleKeyUp(m.wParam, newHandle);
				}
			}
		}
		return false;
	}
	
	static LRESULT CALLBACK mouseWheelHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode >= 0)
		{
			DBG("PARAM: " << wParam);

			if (wParam == WM_MOUSEWHEEL || wParam == 0x020E)
			{
				// using a local copy of this struct to support old mingw libraries
				struct MOUSEHOOKSTRUCTEX_ : public MOUSEHOOKSTRUCT { DWORD mouseData; };

				const MOUSEHOOKSTRUCTEX_& hs = *(MOUSEHOOKSTRUCTEX_*)lParam;

				if (sBrowser != nullptr)
				{
					HWND newHandle = GetFocus();

					if (newHandle != NULL)
					{
						char class_name[256];
						GetClassNameA(newHandle, class_name, 256);

						DBG("CLASS: " << String(class_name));

						//if (String(class_name) != "Internet Explorer_Server")
						//{
							//return false;
						//}
						//else
						//{
							DBG("SCROLLY X: " << hs.pt.x << " Y: " << hs.pt.y);

							MOUSEHOOKSTRUCTEX *pMhs = (MOUSEHOOKSTRUCTEX *)lParam;
							short zDelta = HIWORD(pMhs->mouseData);
							POINT pt;
							GetCursorPos(&pt);
							LPARAM mlParam = MAKELPARAM(pt.x, pt.y);

							HWND hWnd = WindowFromPoint(pt);

							//b = FALSE;
							return SendMessage(newHandle, WM_MOUSEWHEEL, zDelta, mlParam);


						//	return PostMessage(newHandle, WM_MOUSEWHEEL, hs.mouseData & 0xffff0000, (hs.pt.x & 0xffff) | (hs.pt.y << 16));
						//}


						//if (Component* const comp = Desktop::getInstance().findComponentAt(Point<int>(hs.pt.x, hs.pt.y)))
						//{
						//	if (comp->getWindowHandle() != 0)
							//{
								//return PostMessage((HWND)comp->getWindowHandle(), WM_MOUSEWHEEL,
									//hs.mouseData & 0xffff0000, (hs.pt.x & 0xffff) | (hs.pt.y << 16));
					}
				}
			}
		}

		return CallNextHookEx(mouseWheelHook2, nCode, wParam, lParam);
	}

	static LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
	{
		MSG& msg = *(MSG*)lParam;

		
		if (nCode == HC_ACTION && wParam == PM_REMOVE
			&& offerKeyMessageToBrowserWindow(msg))
		{
				zerostruct(msg);
				msg.message = WM_USER;
				return 1;
		}

		return CallNextHookEx(keyboardHook2, nCode, wParam, lParam);
	}

	

	

    void createBrowser()
    {
        CLSID webCLSID = __uuidof (WebBrowser);
        createControl (&webCLSID);

    GUID iidWebBrowser2              = __uuidof (IWebBrowser2);
    GUID iidConnectionPointContainer = __uuidof (IConnectionPointContainer);

        browser = (IWebBrowser2*) queryInterface (&iidWebBrowser2);
		sBrowser = browser;
		sThis = this;
		
		// Prevent browser from showing popup dialogs
		browser->put_Silent(VARIANT_TRUE);

		WebBrowserComponent* const owner = dynamic_cast<WebBrowserComponent*> (getParentComponent());
		owner->setBrowserHidden(true);


        if (IConnectionPointContainer* connectionPointContainer
            = (IConnectionPointContainer*) queryInterface (&iidConnectionPointContainer))
        {
            connectionPointContainer->FindConnectionPoint (__uuidof (DWebBrowserEvents2), &connectionPoint);

            if (connectionPoint != nullptr)
            {
                WebBrowserComponent* const owner = dynamic_cast<WebBrowserComponent*> (getParentComponent());
                jassert (owner != nullptr);

                EventHandler* handler = new EventHandler (*owner);
                connectionPoint->Advise (handler, &adviseCookie);
                handler->Release();
            }
        }
    }

	void setUserAgent(const String& userAgent)
	{
		UrlMkSetSessionOption(URLMON_OPTION_USERAGENT, (LPVOID)userAgent.toStdString().c_str(), userAgent.length(), 0);
	}

	void deleteCookie(const String& domain,
		const String& name)
	{
		/*
		String uri = "https://" + domain;
		LPCTSTR uriw = uri.toWideCharPointer();
		LPCTSTR namew = name.toWideCharPointer();
		DWORD wsize = 8192;
		LPWSTR data = new WCHAR[wsize];

		InternetSetOption(0, 42, NULL, 0);

		InternetGetCookie(uriw, namew, data, &wsize);

		std::wstring newvalue = std::wstring(data) + L";expires=Mon, 01 Jan 0001 00:00:00 GMT";
		LPCWSTR newvaluew = newvalue.c_str();

		std::wcout << data;
		DBG("COOOKIE: " << String(newvalue.c_str()));

		BOOL success = InternetSetCookie(uri.toWideCharPointer(), name.toWideCharPointer(), newvaluew);
		if (success)
		{
			DBG("IT WORKED");

		}
		else
		{
			int errorCode = GetLastError();

			DBG("IT NO WORKED: " << errorCode);
		}
		*/
	}

	void loadHTML(const String& jhtml, const String& baseURL)
	{

		String before = jhtml.upToFirstOccurrenceOf("<head>", true, false);
		String after = jhtml.fromFirstOccurrenceOf("<head>", false, false);

		String modHTML = before + "<base href=\"" + baseURL + "\">" + after;
		
		const wchar_t* html = modHTML.toWideCharPointer();
		const wchar_t* baseURL_ = baseURL.toWideCharPointer();
		IDispatch* pHtmlDoc = NULL;
		HRESULT hr;
		hr = browser->get_Document(&pHtmlDoc);
		if (!pHtmlDoc)
			return;
		CComPtr<IHTMLDocument2> doc2;
		doc2.Attach((IHTMLDocument2*)pHtmlDoc);
		if (!doc2)
			return;
		// Creates a new one-dimensional array
		SAFEARRAY* psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		if (!psaStrings)
			return;
		BSTR bstr = SysAllocString(html);
		if (bstr)
		{
			::VARIANT* param;
			HRESULT hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
			if (SUCCEEDED(hr))
			{
				param->vt = VT_BSTR;
				param->bstrVal = bstr;
				hr = SafeArrayUnaccessData(psaStrings);
				if (SUCCEEDED(hr))
				{
					doc2->put_URL(CComBSTR(baseURL_));
					doc2->write(psaStrings);
					doc2->close();
				}
			}
		}
		// SafeArrayDestroy calls SysFreeString for each BSTR!
		if (psaStrings)
			SafeArrayDestroy(psaStrings);
	}
	

    void goToURL (const String& url,
                  const StringArray* headers,
                  const MemoryBlock* postData)
    {
        if (browser != nullptr)
        {
            LPSAFEARRAY sa = nullptr;

            VARIANT headerFlags, frame, postDataVar, headersVar;  // (_variant_t isn't available in all compilers)
            VariantInit (&headerFlags);
            VariantInit (&frame);
            VariantInit (&postDataVar);
            VariantInit (&headersVar);

            if (headers != nullptr)
            {
                V_VT (&headersVar) = VT_BSTR;
                V_BSTR (&headersVar) = SysAllocString ((const OLECHAR*) headers->joinIntoString ("\r\n").toWideCharPointer());
            }

            if (postData != nullptr && postData->getSize() > 0)
            {
                sa = SafeArrayCreateVector (VT_UI1, 0, (ULONG) postData->getSize());

                if (sa != nullptr)
                {
                    void* data = nullptr;
                    SafeArrayAccessData (sa, &data);
                    jassert (data != nullptr);

                    if (data != nullptr)
                    {
                        postData->copyTo (data, 0, postData->getSize());
                        SafeArrayUnaccessData (sa);

                        VARIANT postDataVar2;
                        VariantInit (&postDataVar2);
                        V_VT (&postDataVar2) = VT_ARRAY | VT_UI1;
                        V_ARRAY (&postDataVar2) = sa;

                        postDataVar = postDataVar2;
                    }
                }
            }

            BSTR urlBSTR = SysAllocString ((const OLECHAR*) url.toWideCharPointer());
            browser->Navigate (urlBSTR, &headerFlags, &frame, &postDataVar, &headersVar);
            SysFreeString (urlBSTR);

            if (sa != nullptr)
                SafeArrayDestroy (sa);

            VariantClear (&headerFlags);
            VariantClear (&frame);
            VariantClear (&postDataVar);
            VariantClear (&headersVar);
        }
    }

    //==============================================================================
    IWebBrowser2* browser;

private:
    IConnectionPoint* connectionPoint;
    DWORD adviseCookie;
	static IWebBrowser2* sBrowser;
	static ActiveXControlComponent* sThis;




    //==============================================================================
    struct EventHandler  : public ComBaseClassHelper<IDispatch>,
                           public ComponentMovementWatcher
    {
        EventHandler (WebBrowserComponent& w)  : ComponentMovementWatcher (&w), owner (w) {}

        JUCE_COMRESULT GetTypeInfoCount (UINT*)                                  { return E_NOTIMPL; }
        JUCE_COMRESULT GetTypeInfo (UINT, LCID, ITypeInfo**)                     { return E_NOTIMPL; }
        JUCE_COMRESULT GetIDsOfNames (REFIID, LPOLESTR*, UINT, LCID, DISPID*)    { return E_NOTIMPL; }

        JUCE_COMRESULT Invoke (DISPID dispIdMember, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS* pDispParams,
                               VARIANT* /*pVarResult*/, EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/)
        {
            if (dispIdMember == DISPID_BEFORENAVIGATE2)
            {
                *pDispParams->rgvarg->pboolVal
                    = owner.pageAboutToLoad (getStringFromVariant (pDispParams->rgvarg[5].pvarVal)) ? VARIANT_FALSE
                                                                                                    : VARIANT_TRUE;
                return S_OK;
            }

            if (dispIdMember == 273 /*DISPID_NEWWINDOW3*/)
            {
                owner.newWindowAttemptingToLoad (pDispParams->rgvarg[0].bstrVal);
                *pDispParams->rgvarg[3].pboolVal = VARIANT_TRUE;
                return S_OK;
            }

            if (dispIdMember == DISPID_DOCUMENTCOMPLETE)
            {
				owner.setBrowserHidden(false);
				owner.pageFinishedLoading (getStringFromVariant (pDispParams->rgvarg[0].pvarVal));
                return S_OK;
            }

            if (dispIdMember == 263 /*DISPID_WINDOWCLOSING*/)
            {
                owner.windowCloseRequest();

                // setting this bool tells the browser to ignore the event - we'll handle it.
                if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == (VT_BYREF | VT_BOOL))
                    *pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;

                return S_OK;
            }

            return E_NOTIMPL;
        }

        void componentMovedOrResized (bool, bool) override   {}
        void componentPeerChanged() override                 {}
        void componentVisibilityChanged() override           { owner.visibilityChanged(); }

    private:
        WebBrowserComponent& owner;

        static String getStringFromVariant (VARIANT* v)
        {
            return (v->vt & VT_BYREF) != 0 ? *v->pbstrVal
                                           : v->bstrVal;
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EventHandler)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

IWebBrowser2 *WebBrowserComponent::Pimpl::sBrowser = nullptr;
ActiveXControlComponent *WebBrowserComponent::Pimpl::sThis = nullptr;

//==============================================================================
WebBrowserComponent::WebBrowserComponent (const bool unloadPageWhenBrowserIsHidden_)
    : browser (nullptr),
      blankPageShown (false),
      unloadPageWhenBrowserIsHidden (unloadPageWhenBrowserIsHidden_),
	browserIsHidden(false)
{
    setOpaque (true);
    addAndMakeVisible (browser = new Pimpl());
}

WebBrowserComponent::~WebBrowserComponent()
{
    delete browser;
}

//==============================================================================
void WebBrowserComponent::goToURL (const String& url,
                                   const StringArray* headers,
                                   const MemoryBlock* postData)
{
    lastURL = url;

    if (headers != nullptr)
        lastHeaders = *headers;
    else
        lastHeaders.clear();

    if (postData != nullptr)
        lastPostData = *postData;
    else
        lastPostData.reset();

    blankPageShown = false;

    if (browser->browser == nullptr)
        checkWindowAssociation();

    browser->goToURL (url, headers, postData);
}

void WebBrowserComponent::loadHTML(const String& jhtml, const String& baseURL)
{
	browser->loadHTML(jhtml, baseURL);
}

void WebBrowserComponent::deleteCookie(const String& domain,
	const String& name)
{
	browser->deleteCookie(domain, name);
}

void WebBrowserComponent::setUserAgent(const String& userAgent, bool append)
{
	browser->setUserAgent(userAgent);
}

void WebBrowserComponent::stop()
{
    if (browser->browser != nullptr)
        browser->browser->Stop();
}

void WebBrowserComponent::goBack()
{
    lastURL.clear();
    blankPageShown = false;

    if (browser->browser != nullptr)
        browser->browser->GoBack();
}

void WebBrowserComponent::goForward()
{
    lastURL.clear();

    if (browser->browser != nullptr)
        browser->browser->GoForward();
}

void WebBrowserComponent::refresh()
{
    if (browser->browser != nullptr)
        browser->browser->Refresh();
}

//==============================================================================
void WebBrowserComponent::paint (Graphics& g)
{
    if (browser->browser == nullptr)
    {
        g.fillAll (Colours::white);
        checkWindowAssociation();
    }
}

void WebBrowserComponent::checkWindowAssociation()
{
    if (isShowing())
    {
        if (browser->browser == nullptr && getPeer() != nullptr)
        {
            browser->createBrowser();
            reloadLastURL();
        }
        else
        {
            if (blankPageShown)
                goBack();
        }
    }
    else
    {
        if (browser != nullptr && unloadPageWhenBrowserIsHidden && ! blankPageShown)
        {
            // when the component becomes invisible, some stuff like flash
            // carries on playing audio, so we need to force it onto a blank
            // page to avoid this..

            blankPageShown = true;
            browser->goToURL ("about:blank", 0, 0);
        }
    }
}

String WebBrowserComponent::getCurrentURL()
{
	BSTR url_buffer;

	HRESULT result = browser->browser->get_LocationURL(&url_buffer);
	//std::wstring ws(url_buffer, SysStringLen(url_buffer));


	return String(url_buffer);
}

void WebBrowserComponent::reloadLastURL()
{
    if (lastURL.isNotEmpty())
    {
        goToURL (lastURL, &lastHeaders, &lastPostData);
        lastURL.clear();
    }
}

void WebBrowserComponent::parentHierarchyChanged()
{
    checkWindowAssociation();
}

void WebBrowserComponent::resized()
{
    browser->setSize (getWidth(), getHeight());
}

void WebBrowserComponent::visibilityChanged()
{
    checkWindowAssociation();
}

void WebBrowserComponent::setBrowserHidden(bool hidden)
{
	if (hidden && browserIsHidden || !hidden && !browserIsHidden) return;


	HWND hwnd;

	GUID iidOleObject = __uuidof (IOleObject);
	GUID iidOleWindow = __uuidof (IOleWindow);

	if (IOleObject* oleObject = (IOleObject*)browser->queryInterface(&iidOleObject))
	{
		if (IOleWindow* oleWindow = (IOleWindow*)browser->queryInterface(&iidOleWindow))
		{
			IOleClientSite* oleClientSite = nullptr;

			if (SUCCEEDED(oleObject->GetClientSite(&oleClientSite)))
			{
				HWND hwnd;
				oleWindow->GetWindow(&hwnd);

				if (hidden)
				{
					ShowWindow(hwnd, SW_HIDE);
					browserIsHidden = true;
				}
				else
				{
					ShowWindow(hwnd, SW_SHOW);
					browserIsHidden = false;
				}
				oleClientSite->Release();
			}

			oleWindow->Release();
		}

		oleObject->Release();
	}
}


void WebBrowserComponent::focusGained (FocusChangeType)
{
    GUID iidOleObject = __uuidof (IOleObject);
    GUID iidOleWindow = __uuidof (IOleWindow);

    if (IOleObject* oleObject = (IOleObject*) browser->queryInterface (&iidOleObject))
    {
        if (IOleWindow* oleWindow = (IOleWindow*) browser->queryInterface (&iidOleWindow))
        {
            IOleClientSite* oleClientSite = nullptr;

            if (SUCCEEDED (oleObject->GetClientSite (&oleClientSite)))
            {
                HWND hwnd;
                oleWindow->GetWindow (&hwnd);
                oleObject->DoVerb (OLEIVERB_UIACTIVATE, nullptr, oleClientSite, 0, hwnd, nullptr);

                oleClientSite->Release();
            }

            oleWindow->Release();
        }

        oleObject->Release();
    }
}
