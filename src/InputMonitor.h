#pragma once

#include "BMDSwitcherAPI.h"
#include <string>
#include <map>

namespace ofxAtem
{
static auto getStdStringFromCFString(CFStringRef myCFStringRef){
    
    const CFIndex kCStringSize = 512;
    char temporaryCString[kCStringSize];
    bzero(temporaryCString,kCStringSize);
    
    CFStringGetCString(myCFStringRef, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    return std::string(temporaryCString);
}

class Controller;
    class InputMonitor : public IBMDSwitcherInputCallback {
        
        inline static const std::map<BMDSwitcherPortType, std::string> kSwitcherPortTypes =
        {
            { bmdSwitcherPortTypeExternal,              "External" },
            { bmdSwitcherPortTypeBlack,                 "Black Video" },
            { bmdSwitcherPortTypeColorBars,             "Color-Bars" },
            { bmdSwitcherPortTypeColorGenerator,        "Color Generator" },
            { bmdSwitcherPortTypeMediaPlayerFill,       "Media-Player Fill" },
            { bmdSwitcherPortTypeMediaPlayerCut,        "Media-Player Cut" },
            { bmdSwitcherPortTypeSuperSource,           "Super-Source" },
            { bmdSwitcherPortTypeMixEffectBlockOutput,  "Mix-Effect Block Output" },
            { bmdSwitcherPortTypeAuxOutput,             "Auxiliary Output" },
            { bmdSwitcherPortTypeKeyCutOutput,          "Key Cut Output"},
            { bmdSwitcherPortTypeMultiview,             "MultiView"},
            { bmdSwitcherPortTypeExternalDirect,        "External Direct"}
        };
        
    public:
        InputMonitor(IBMDSwitcherInput* input, Controller* uiDelegate);
        auto getName() {
            CFStringRef str;
            mInput->GetLongName(&str);
            return getStdStringFromCFString(str);
        }
        
        auto getPortType() {
            BMDSwitcherPortType val;
            mInput->GetPortType(&val);
            return val;
        }
        
        auto getPortTypeName() {
            auto port_type = getPortType();
            if (kSwitcherPortTypes.count(port_type)) {
                return kSwitcherPortTypes.at(port_type);
            } else {
                return std::string{"unknown port type"};
            }
        }
        
    protected:
        ~InputMonitor();
        
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);
        //HRESULT PropertyChanged(BMDSwitcherInputPropertyId propertyId);
        HRESULT Notify (BMDSwitcherInputEventType eventType) { return S_OK; };

        IBMDSwitcherInput* input();
        
    private:
        IBMDSwitcherInput*			mInput;
        Controller*					mUiDelegate;
        std::atomic<std::int32_t> 	mRefCount;
    };
}
