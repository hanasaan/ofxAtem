#include "SwitcherMonitor.h"
#include <libkern/OSAtomic.h>

#include "ofxAtem.h"

static inline bool	operator== (const REFIID& iid1, const REFIID& iid2)
{ 
	return CFEqual(&iid1, &iid2);
}

using namespace ofxAtem;

HRESULT STDMETHODCALLTYPE SwitcherMonitor::QueryInterface(REFIID iid, LPVOID *ppv)
{
	if (!ppv)
		return E_POINTER;
	
	if (iid == IID_IBMDSwitcherCallback)
	{
		*ppv = static_cast<IBMDSwitcherCallback*>(this);
		AddRef();
		cout << "SWITCH connected" << endl;
		return S_OK;
	}
	
	if (CFEqual(&iid, IUnknownUUID))
	{
		*ppv = static_cast<IUnknown*>(this);
		AddRef();
		cout << "SWITCH connected" << endl;
		return S_OK;
	}
	cout << "ERROR SWITCH disconnected" << endl;
	*ppv = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE SwitcherMonitor::AddRef(void)
{
	return mRefCount++;
}

ULONG STDMETHODCALLTYPE SwitcherMonitor::Release(void)
{
	auto newCount = mRefCount++;
	if (newCount == 0)
		delete this;
	return newCount;
}

HRESULT STDMETHODCALLTYPE SwitcherMonitor::Notify (BMDSwitcherEventType eventType,  BMDSwitcherVideoMode coreVideoMode) {
    return S_OK;
}

//HRESULT STDMETHODCALLTYPE	SwitcherMonitor::Disconnected(void)
//{
//    mUiDelegate->connectionStatus = 0;
//    if (mUiDelegate->isAutoReconnect()) {
//        mUiDelegate->reconnect();
//    }
//	return S_OK;
//}
