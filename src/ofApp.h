#pragma once

#include "ofMain.h"
#include "ofxCV.h"
#include "ofMain.h"
#include "ofxGUI.h"

class ofApp : public ofBaseApp{

	public:

		//In case videos get used for a consistant testing environment
		const bool USE_WEB_CAM = true;
		const int HIDDEN_OBJECT = 0;
		const int SEARCH_OBJECT = 1;
		const float RANGE_OF_DETECTION = 0.5f;
		ofVideoGrabber m_webcam;

		ofxPanel m_hiddenGUI;
		ofxPanel m_searchGUI;

		ofParameter<int> m_threshold[2];
		ofParameter<float> m_minRadius[2];	//low pass filter
		ofParameter<float> m_maxRadius[2];	//high pass filter
		ofParameter<float> m_hiddenObjectWidth;
		ofParameter<float> m_searchObjectWidth;

		ofColor m_hiddenObjectColour;
		ofColor m_searchObjectColour;
		int selectedColour = HIDDEN_OBJECT; //default colour being picked first is the hiddenobject

		ofxCv::ContourFinder m_contourFinder;
		ofxCv::ContourFinder m_hiddenContourFinder;


		float m_focalLength[2];
		float m_objectDistance;
		int m_indexLargestContour[2];

		ofxButton m_hideButton;
		ofxButton m_calibrateButton;
		ofVec3f m_hiddenObjectPosition[2];
		ofVec3f m_searchObjectPosition[2];
		bool m_doubleTracking;
		
		float m_pastPositions[10];
		string m_hint;

		void setup();
		void update();
		void draw();

		void buttonPressed();
		void getObjectPosition(cv::Rect object, ofVec3f objectPosition[]);
		void mousePressed(int x, int y, int button);
		void drawContour(int index, ofxCv::ContourFinder contourFinder);
		void findDistance(float focalLength, int index, ofxCv::ContourFinder contourFinder);
		void calibrateFocalLength();
		void checkCollision(ofxCv::ContourFinder contourFinder, int object);
};
