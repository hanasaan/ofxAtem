#include "ofMain.h"
#include "ofxAtem.h"

class ofApp : public ofBaseApp{
    ofxAtem::Controller atem;
public:
    void setup()
    {
        ofSetVerticalSync(true);
        ofSetFrameRate(60);
        
        atem.connect("10.0.0.250");
    }
    
    void update()
    {
        atem.update();
    }
    void draw()
    {
        ofClear(0);
        
        ofDrawBitmapStringHighlight(atem.getProductName(), 10, 20);
        ofDrawBitmapStringHighlight(atem.getVideoModeString(), 10, 40);
        
        ofDrawBitmapStringHighlight("Program : " + ofToString(atem.getProgramId()), 10, 60);
        ofDrawBitmapStringHighlight("Preview : " + ofToString(atem.getPreviewId()), 10, 80);
        
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 100);
        
        if (atem.isConnected()) {
            ofPushStyle();
            ofSetColor(0, 255, 0);
            ofDrawRectangle(400, 10, 32, 32);
            ofPopStyle();
        }
    }
    
    void keyPressed(int key)
    {
        if (key >= '1' && key <= '8') {
            atem.setPreviewId(key - '0');
        }
        if (key == OF_KEY_RETURN) {
            atem.performCut();
        }
        if (key == 'a') {
            atem.performAuto();
        }
    
    }
};

//========================================================================
int main( ){
    ofSetupOpenGL(640,360,OF_WINDOW);            // <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}
