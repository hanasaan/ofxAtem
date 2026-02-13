#pragma once

#include "BMDSwitcherAPI.h"
#include <atomic>

namespace ofxAtem {
    class Controller;
    
    class SwitcherMonitor : public IBMDSwitcherCallback
    {
    public:
        SwitcherMonitor(Controller* uiDelegate) :	mUiDelegate(uiDelegate), mRefCount(1){};
        
    protected:
        virtual ~SwitcherMonitor(){};
        
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) override;
        ULONG STDMETHODCALLTYPE AddRef(void) override;
        ULONG STDMETHODCALLTYPE Release(void) override;
        HRESULT STDMETHODCALLTYPE Notify (BMDSwitcherEventType eventType,  BMDSwitcherVideoMode coreVideoMode) override;

    private:
        Controller*	mUiDelegate;
        std::atomic<std::int32_t> mRefCount;
    };
}

