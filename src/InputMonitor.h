#pragma once

#include "BMDSwitcherAPI.h"

namespace ofxAtem
{
    class Controller;
    class InputMonitor : public IBMDSwitcherInputCallback {
        
    public:
        InputMonitor(IBMDSwitcherInput* input, Controller* uiDelegate);
        
    protected:
        ~InputMonitor();
        
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);
        HRESULT PropertyChanged(BMDSwitcherInputPropertyId propertyId);
        IBMDSwitcherInput* input();
        
    private:
        IBMDSwitcherInput*			mInput;
        Controller*					mUiDelegate;
        int							mRefCount;
    };
}




