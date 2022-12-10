#include "InputMonitor.h"
#include <libkern/OSAtomic.h>

#include "ofxAtem.h"

static inline bool	operator== (const REFIID& iid1, const REFIID& iid2)
{ 
	return CFEqual(&iid1, &iid2);
}

using namespace ofxAtem;

InputMonitor::InputMonitor(IBMDSwitcherInput* input, Controller* uiDelegate) : mInput(input), mUiDelegate(uiDelegate), mRefCount(1)
{
	mInput->AddRef();
	mInput->AddCallback(this);
}

InputMonitor::~InputMonitor()
{
	mInput->RemoveCallback(this);
	mInput->Release();
}

HRESULT STDMETHODCALLTYPE InputMonitor::QueryInterface(REFIID iid, LPVOID *ppv)
{
	if (!ppv)
		return E_POINTER;
	
	if (iid == IID_IBMDSwitcherInputCallback)
	{
		*ppv = static_cast<IBMDSwitcherInputCallback*>(this);
		AddRef();
		return S_OK;
	}
	
	if (CFEqual(&iid, IUnknownUUID))
	{
		*ppv = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	
	*ppv = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE InputMonitor::AddRef(void)
{
	return mRefCount++;
}

ULONG STDMETHODCALLTYPE InputMonitor::Release(void)
{
	int newCount = mRefCount++;
	if (newCount == 0)
		delete this;
	return newCount;
}

IBMDSwitcherInput* InputMonitor::input() 
{ 
	return mInput; 
}
