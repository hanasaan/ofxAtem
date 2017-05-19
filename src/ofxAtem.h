//
//  ofxAtem.h
//  Created by Yuya Hanai on 2016/09/13.
//
// based on:
// (c) 2012 telematique
// www.telematique.de
// telematique@web.de

#pragma once

#include "ofMain.h"

#include "BMDSwitcherAPI.h"
#include "InputMonitor.h"
#include "SwitcherMonitor.h"

namespace ofxAtem
{
    class Controller
    {
    public:
        friend class SwitcherMonitor;
        
        Controller();
        ~Controller();
        
        bool connect(string ip);
        void reconnect();
        void disconnect();
        void update();
        
        void performCut();
        void performAuto();
        void setProgramId(int id);
        void setPreviewId(int id);
        void setAutoRate(uint32_t frames);
        void setTransitionPosition(float pos);
        
        int getProgramId() const { return program; }
        int getPreviewId() const { return preview; }
        string getProductName() const { return product_name; }
        string getVideoModeString() const { return video_mode; }
        bool isInTransition() const;
        bool isConnected() const { return connectionStatus; }
        
        void setAutoReconnect(bool b) { b_autoreconnect = b; }
        bool isAutoReconnect() const { return b_autoreconnect; }
    protected:
        void init();
        
        int connectionStatus = 0;
        int64_t program = -1;
        int64_t preview = -1;
        string product_name;
        string video_mode;
        string ip_;
        bool b_autoreconnect = false;
        bool b_reconnect = false;
        
        IBMDSwitcherDiscovery*		mSwitcherDiscovery;
        IBMDSwitcher*				mSwitcher;
        IBMDSwitcherMixEffectBlock*	mMixEffectBlock;

        SwitcherMonitor*			mSwitcherMonitor;
        std::vector<InputMonitor*>	mInputMonitors;
        std::vector<IBMDSwitcherInputAux*>	mSwitcherInputAuxList;
        std::vector<BMDSwitcherInputId*>	mInputIds;
    };
}

