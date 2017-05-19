//
//  ofxAtem.cpp
//  Created by Yuya Hanai on 2016/09/13.
//
// based on:
// (c) 2012 telematique
// www.telematique.de
// telematique@web.de

#include "ofxAtem.h"


namespace ofxAtem
{
    static string getCStringFromCFString(CFStringRef myCFStringRef){
        CFRange rangeToProcess;
        const char *bytes;
        bytes = CFStringGetCStringPtr(myCFStringRef, kCFStringEncodingMacRoman);
        if (bytes == NULL) {
            char localBuffer[10];
            Boolean success;
            success = CFStringGetCString(myCFStringRef, localBuffer, 10, kCFStringEncodingMacRoman);
        }
        return bytes;
    }
    
    static string toVideoModeString(BMDSwitcherVideoMode mode) {
        /*
         bmdSwitcherVideoMode525i5994NTSC                             = 'ntsc',
         bmdSwitcherVideoMode625i50PAL                                = 'pall',
         bmdSwitcherVideoMode525i5994Anamorphic                       = 'ntsA',
         bmdSwitcherVideoMode625i50Anamorphic                         = 'palA',
         bmdSwitcherVideoMode720p50                                   = '72p0',
         bmdSwitcherVideoMode720p5994                                 = '72p9',
         bmdSwitcherVideoMode1080i50                                  = '10i0',
         bmdSwitcherVideoMode1080i5994                                = '10i9',
         bmdSwitcherVideoMode1080p2398                                = '10p3',
         bmdSwitcherVideoMode1080p24                                  = '10p4',
         bmdSwitcherVideoMode1080p25                                  = '10p5',
         bmdSwitcherVideoMode1080p2997                                = '10p7',
         bmdSwitcherVideoMode1080p50                                  = '10p0',
         bmdSwitcherVideoMode1080p5994                                = '10p9',
         bmdSwitcherVideoMode4KHDp2398                                = '4H23',
         bmdSwitcherVideoMode4KHDp24                                  = '4H24',
         bmdSwitcherVideoMode4KHDp25                                  = '4H25',
         bmdSwitcherVideoMode4KHDp2997                                = '4H29',
         bmdSwitcherVideoMode4KHDp50                                  = '4H50',
         bmdSwitcherVideoMode4KHDp5994                                = '4H59'
         */
        switch (mode) {
            case bmdSwitcherVideoMode1080i5994:
                return "1080i5994";
            case bmdSwitcherVideoMode1080p2997:
                return "1080p2997";
            case bmdSwitcherVideoMode1080p5994:
                return "1080p5994";
            default:
                const char* str = reinterpret_cast<const char*>(&mode);
                return (string) str; // TODO
        }
    }
    
    Controller::Controller()
    {
        init();
    }

    Controller::~Controller()
    {
        disconnect();
    }
    
    void Controller::init()
    {
        mSwitcherDiscovery = NULL;
        mMixEffectBlock = NULL;
        mSwitcher = NULL;
        program = -1;
        preview = -1;
        connectionStatus = 0;
        
        mSwitcherMonitor = new SwitcherMonitor(this);
        this->mSwitcherDiscovery = CreateBMDSwitcherDiscoveryInstance();
        if (!mSwitcherDiscovery)
        {
        }
    }
    
    bool Controller::connect(string ip)
    {
        disconnect();
        this->ip_ = ip;
        
        BMDSwitcherConnectToFailure			failReason;
        
        CFStringRef cf;
        cf = CFStringCreateWithCString(kCFAllocatorDefault,ip.c_str(),kCFStringEncodingMacRoman);
        
        HRESULT hr = mSwitcherDiscovery->ConnectTo(cf, &mSwitcher, &failReason);
        
        if (SUCCEEDED(hr))
        {
            connectionStatus = 1;
            cout << "NET CONNECTION OK" << endl;
            {
                HRESULT result;
                IBMDSwitcherMixEffectBlockIterator* iterator = NULL;
                IBMDSwitcherInputIterator* inputIterator = NULL;
                
                CFStringRef productName;
                CFStringRef ID;
                
                {
                    if (FAILED(mSwitcher->GetString(bmdSwitcherPropertyIdProductName, (CFStringRef*)&productName)))
                    {
                        cout << "Could not get switcher product name" << endl;
                        return;
                    }
                    this->product_name = getCStringFromCFString(productName);
                }
                
                {
                    int64_t val = 0;
                    if (FAILED(mSwitcher->GetInt(bmdSwitcherPropertyIdVideoMode, &val)))
                    {
                        cout << "Could not get switcher video mode" << endl;
                        return;
                    }
                    BMDSwitcherVideoMode mode = (BMDSwitcherVideoMode) val;
                    this->video_mode = toVideoModeString(mode);
                }
                
                BMDSwitcherInputId id;
                mSwitcher->AddCallback(mSwitcherMonitor);
                
                result = mSwitcher->CreateIterator(IID_IBMDSwitcherInputIterator, (void**)&inputIterator);
                if (SUCCEEDED(result))
                {
                    IBMDSwitcherInput* input = NULL;
                    while (S_OK == inputIterator->Next(&input))
                    {
                        InputMonitor* inputMonitor = new InputMonitor(input, this);
                        IBMDSwitcherInputAux* auxObj;
                        result = input->QueryInterface(IID_IBMDSwitcherInputAux, (void**)&auxObj);
                        input->GetInputId(&id);
                        if (SUCCEEDED(result))
                        {
                            BMDSwitcherInputId auxId;
                            result = auxObj->GetInputSource(&auxId);
                            mSwitcherInputAuxList.push_back(auxObj);
                        }
                        input->Release();
                        mInputMonitors.push_back(inputMonitor);
                    }
                    inputIterator->Release();
                    inputIterator = NULL;
                }
                
                result = mSwitcher->CreateIterator(IID_IBMDSwitcherMixEffectBlockIterator, (void**)&iterator);
                if (FAILED(result))
                {
                    cout << "Could not create IBMDSwitcherMixEffectBlockIterator iterator" << endl;
                    if (iterator)
                        iterator->Release();
                } else if (S_OK != iterator->Next(&mMixEffectBlock))
                {
                    cout << "Could not get the first IBMDSwitcherMixEffectBlock" << endl;
                    if (iterator)
                        iterator->Release();
                }
            }
            return true;
        }
        else
        {
            connectionStatus = 0;
            cout << "NET CONNECTION ERROR: ";
            CFStringRef* reason;
            switch (failReason)
            {
                case bmdSwitcherConnectToFailureNoResponse:
                    cout << "No response from Switcher"<< endl;
                    break;
                case bmdSwitcherConnectToFailureIncompatibleFirmware:
                    cout << "Switcher has incompatible firmware" << endl;
                    break;
                default:
                    cout << "Connection failed for unknown reason"<< endl;
            }
        }
        return false;
    }
    
    void Controller::reconnect()
    {
        connect(ip_);
        b_reconnect = connectionStatus == 0;
    }

    void Controller::disconnect()
    {
        // cleanup resources created when switcher was connected
        for (auto& p : mInputMonitors) {
            p->Release();
        }
        mInputMonitors.clear();
        
        if (mMixEffectBlock)
        {
            mMixEffectBlock->Release();
            mMixEffectBlock = NULL;
        }
        
        if (mSwitcher)
        {
            mSwitcher->RemoveCallback(mSwitcherMonitor);
            mSwitcher->Release();
            mSwitcher = NULL;
        }
        
        while (mSwitcherInputAuxList.size())
        {
            mSwitcherInputAuxList.back()->Release();
            mSwitcherInputAuxList.pop_back();
        }
    }
    
    void Controller::update()
    {
        if (b_reconnect && b_autoreconnect) {
            reconnect();
        }
        if (mMixEffectBlock) {
            mMixEffectBlock->GetInt(bmdSwitcherMixEffectBlockPropertyIdProgramInput, &program);
            mMixEffectBlock->GetInt(bmdSwitcherMixEffectBlockPropertyIdPreviewInput, &preview);
        }
    }
    
    void Controller::performCut()
    {
        if (mMixEffectBlock) {
            mMixEffectBlock->PerformCut();
        }
    }
    
    void Controller::performAuto()
    {
        if (mMixEffectBlock) {
            mMixEffectBlock->PerformAutoTransition();
        }
    }
    
    void Controller::setProgramId(int id)
    {
        if (mMixEffectBlock) {
            mMixEffectBlock->SetInt(bmdSwitcherMixEffectBlockPropertyIdProgramInput, id);
        }
    }
    
    void Controller::setPreviewId(int id)
    {
        if (mMixEffectBlock) {
            mMixEffectBlock->SetInt(bmdSwitcherMixEffectBlockPropertyIdPreviewInput, id);
        }
    }
    
    void Controller::setAutoRate(uint32_t frames)
    {
        if (mMixEffectBlock) {
            HRESULT result;
            IBMDSwitcherTransitionMixParameters* mTransitionMixParameters=NULL;
            result = mMixEffectBlock->QueryInterface(IID_IBMDSwitcherTransitionMixParameters, (void**)&mTransitionMixParameters);
            if (SUCCEEDED(result))
            {
                mTransitionMixParameters->SetRate(frames);
            }
        }
    }
    
    void Controller::setTransitionPosition(float pos)
    {
        if (mMixEffectBlock) {
            HRESULT result;
            IBMDSwitcherTransitionMixParameters* mTransitionMixParameters=NULL;
            result = mMixEffectBlock->SetFloat(bmdSwitcherMixEffectBlockPropertyIdTransitionPosition, pos);
        }
    }
    
    bool Controller::isInTransition() const
    {
        bool inTransition;
        mMixEffectBlock->GetFlag(bmdSwitcherMixEffectBlockPropertyIdInTransition, &inTransition);
        return inTransition;
    }
}

