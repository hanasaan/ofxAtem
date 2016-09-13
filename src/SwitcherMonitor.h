#pragma once

#include "BMDSwitcherAPI.h"

namespace ofxAtem {
    class Controller;
    
    class SwitcherMonitor : public IBMDSwitcherCallback
    {
    public:
        SwitcherMonitor(Controller* uiDelegate) :	mUiDelegate(uiDelegate), mRefCount(1){};
        
    protected:
        virtual ~SwitcherMonitor(){};
        
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
        
        ULONG STDMETHODCALLTYPE AddRef(void);
        
        ULONG STDMETHODCALLTYPE Release(void);
        
        // Switcher Property changes ignored by this sample app
        HRESULT STDMETHODCALLTYPE	PropertyChanged(BMDSwitcherPropertyId propId);
        HRESULT STDMETHODCALLTYPE	Disconnected(void);
        
    private:
        Controller*	mUiDelegate;
        int							mRefCount;
    };
}

