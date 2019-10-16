#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//Initializing variables
	m_doubleTracking = false;
	for (int i = 0; i < 10; i++) {
		m_pastPositions[i] = 0;
	}

	//Creating buttons to calibrate the objects and set hidden position
	*m_hideButton.setup("Hide Object");
	m_hideButton.setPosition(500, 150);
	*m_calibrateButton.setup("Calibrate Object");
	m_calibrateButton.setPosition(500, 200);

	//Adding functions to the buttons
	m_hideButton.addListener(this, &ofApp::buttonPressed);
	m_calibrateButton.addListener(this, &ofApp::calibrateFocalLength);

	//Settinig webcam dimensions
	//USE_WEB_CAM is not really necessary since video processing is not implemented, though it was planned
	if (USE_WEB_CAM) {
		m_webcam.setup(480, 240);
	}

	//Creating controls for the colours being tracked
	m_hiddenGUI.setup();
	m_searchGUI.setup();

	m_hiddenGUI.add(m_threshold[HIDDEN_OBJECT].set("Hidden Threshold", 128, 0, 255));
	m_hiddenGUI.add(m_minRadius[HIDDEN_OBJECT].set("Hidden Min Rad.", 10.0f, 0.0f, 100.f));
	m_hiddenGUI.add(m_maxRadius[HIDDEN_OBJECT].set("Hidden Max Rad.", 150.0f, 0.0f, 1000.0f));

	m_searchGUI.add(m_threshold[SEARCH_OBJECT].set("Threshold", 128, 0, 255));
	m_searchGUI.add(m_minRadius[SEARCH_OBJECT].set("Min Rad.", 10.0f, 0.0f, 100.f));
	m_searchGUI.add(m_maxRadius[SEARCH_OBJECT].set("Max Rad.", 150.0f, 0.0f, 1000.0f));

	m_hiddenGUI.add(m_hiddenObjectWidth.set("Hidden Obj Width", 0.0f, 0.0f, 10.0f));
	m_searchGUI.add(m_searchObjectWidth.set("Search Obj Width", 0.0f, 0.0f, 10.0f));
	m_hiddenGUI.setPosition(500,0);
	m_searchGUI.setPosition(750, 0);

}

//--------------------------------------------------------------
void ofApp::update(){
	//Checking which object is being tracked
	if (selectedColour == SEARCH_OBJECT) {

		//Applying filters onto the contour
		m_contourFinder.setThreshold(m_threshold[SEARCH_OBJECT]);
		m_contourFinder.setMinAreaRadius(m_minRadius[SEARCH_OBJECT]); 
		m_contourFinder.setMaxAreaRadius(m_maxRadius[SEARCH_OBJECT]);

		//Targeting the colour
		m_contourFinder.setTargetColor(m_searchObjectColour, ofxCv::TRACK_COLOR_RGB);
		m_hiddenContourFinder.setTargetColor(m_hiddenObjectColour, ofxCv::TRACK_COLOR_RGB);

		//Checking if the hidden object has been found
		checkCollision(m_contourFinder, SEARCH_OBJECT);

		//Finding contours on screen
		if (USE_WEB_CAM) {
			m_webcam.update();
			m_contourFinder.findContours(m_webcam);
			m_hiddenContourFinder.findContours(m_webcam);
			
		}

	}//Processing the hidden object
	else {
		//Applying filters
		m_hiddenContourFinder.setThreshold(m_threshold[HIDDEN_OBJECT]);
		m_hiddenContourFinder.setMinAreaRadius(m_minRadius[HIDDEN_OBJECT]);   //Filter out the blobs that are too small
		m_hiddenContourFinder.setMaxAreaRadius(m_maxRadius[HIDDEN_OBJECT]);	//Filter out the high area blobs

		//Setting colour
		m_hiddenContourFinder.setTargetColor(m_hiddenObjectColour, ofxCv::TRACK_COLOR_RGB);

		//Finding the colours on the screen
		if (USE_WEB_CAM) {
			m_webcam.update();
			m_hiddenContourFinder.findContours(m_webcam);
			
		}
	}

}

void ofApp::draw(){
	
	if (USE_WEB_CAM) {
		m_webcam.draw(0, 0);
	}
	//Reseting the largest contour on screen
	double areaLargest = 0.0f;
	m_indexLargestContour[HIDDEN_OBJECT] = 0;
	m_indexLargestContour[SEARCH_OBJECT] = 0;

	//Getting the area of the largest countour (if there are contours)
	if (selectedColour == SEARCH_OBJECT) {

		const int numContours = m_contourFinder.getContours().size();
		const int numHiddenContours = m_hiddenContourFinder.getContours().size();


		if (numContours > 0) {
			for (int i = 0; i < numContours; i++) {
				double area = m_contourFinder.getContourArea(i);

				if (area > areaLargest) {
					m_indexLargestContour[SEARCH_OBJECT] = i;
				}

			}
			//Drawing the contour and finding the position on screen
			drawContour(m_indexLargestContour[SEARCH_OBJECT], m_contourFinder);
			findDistance(m_focalLength[1], m_indexLargestContour[SEARCH_OBJECT], m_contourFinder);


		}
		//If the seconnd object is also being tracked
		if (numHiddenContours > 0) {
			m_doubleTracking = true;

			for (int i = 0; i < numHiddenContours; i++) {
				double area = m_hiddenContourFinder.getContourArea(i);

				if (area > areaLargest) {
					m_indexLargestContour[HIDDEN_OBJECT] = i;
				}

			}
			
			//Drawing the contours and finding the position on screen
			drawContour(m_indexLargestContour[HIDDEN_OBJECT], m_hiddenContourFinder);
			findDistance(m_focalLength[0], m_indexLargestContour[HIDDEN_OBJECT], m_hiddenContourFinder);

			
		}
		else {
			m_doubleTracking = false;
		}

	}//If the hidden button is select
	else {
		const int numContours = m_hiddenContourFinder.getContours().size();


		if (numContours > 0) {
			for (int i = 0; i < numContours; i++) {
				double area = m_hiddenContourFinder.getContourArea(i);

				if (area > areaLargest) {
					m_indexLargestContour[HIDDEN_OBJECT] = i;
				}

			}
			//Drawing the contour and finding the position of the object on screen
			drawContour(m_indexLargestContour[HIDDEN_OBJECT], m_hiddenContourFinder);
			findDistance(m_focalLength[0], m_indexLargestContour[HIDDEN_OBJECT], m_hiddenContourFinder);


		}
	}



	//Drawing the tracked colours
	ofSetColor(m_hiddenObjectColour);
	ofDrawRectangle(750, 150, 60, 40);
	ofSetColor(m_searchObjectColour);
	ofDrawRectangle(750, 190, 60, 40);
	//Showing which colour is selected
	if (selectedColour == SEARCH_OBJECT) {
		ofSetColor(255,0,0);
		ofSetLineWidth(2);
		ofNoFill();
		ofDrawRectangle(750, 190, 60, 40);
		ofFill();
		ofSetColor(255, 255, 255);
	}
	else {
		ofSetColor(255, 0, 0);
		ofSetLineWidth(2);
		ofNoFill();
		ofDrawRectangle(750, 150, 60, 40);
		ofFill();
		ofSetColor(255, 255, 255);
	}
	ofSetColor((0, 0, 0));
	ofDrawBitmapString(m_hint, 200, 600);
	ofSetColor((255,255,255));
	ofDrawBitmapString("Hidden Colour", 820, 170);
	ofDrawBitmapString("Search Colour", 820, 200);
	//Drawing the GUI
	m_hideButton.draw();
	m_calibrateButton.draw();
	m_hiddenGUI.draw();
	m_searchGUI.draw();

}


void ofApp::checkCollision(ofxCv::ContourFinder contourFinder, int object) {
	//Check if the tracked object area intersects hidden object
	//We need the position (x,y,z) and the cubic area that takes up (x+width, h+height, z+depth)
	//Note that depth will just be the depth line that the object sits on. 

	//If there are objects to track
	if (contourFinder.getContours().size() > 0) {

		//Getting the position of the object
		cv::Rect searchObject = contourFinder.getBoundingRect(m_indexLargestContour[object]);
		getObjectPosition(searchObject, m_searchObjectPosition);
		float euclidianDistance[2];

		//Determining the distance from the top left and bottom right points
		euclidianDistance[0] = sqrt(pow(m_searchObjectPosition[0].x - m_hiddenObjectPosition[0].x, 2.0f) + pow(m_searchObjectPosition[0].z - m_hiddenObjectPosition[0].z, 2.0f) + pow(m_searchObjectPosition[0].z - m_hiddenObjectPosition[0].z, 2.0f));
		euclidianDistance[1] = sqrt(pow(m_searchObjectPosition[1].x - m_hiddenObjectPosition[1].x, 2.0f) + pow(m_searchObjectPosition[1].z - m_hiddenObjectPosition[1].z, 2.0f) + pow(m_searchObjectPosition[1].z - m_hiddenObjectPosition[1].z, 2.0f));


		//Pushing a new position onto the list of previous positions 
		for (int i = 0; i < 9; i++) {
			m_pastPositions[i] = m_pastPositions[i+1];
		}
		m_pastPositions[9] = euclidianDistance[0];

		//Checking if the objects intersect, and are both showing
		if ((euclidianDistance[0] < 4.0f) || (euclidianDistance[1] < 4.0f) && m_doubleTracking) {
			m_hint = "You Found it!!";
		}//Checking if the objects intersect, but the player hasn't found it
		else if ((euclidianDistance[0] <  4.0f) || (euclidianDistance[1] < 4.0f)) {
			
			m_hint = "HOT HOT HOT! So close!";
		}
		else {
			//Checking the object's tending path (toward or away from the object
			int successfulChecks[2] = {0,0};
			for (int i = 0; i < 9; i++) {

				if (m_pastPositions[9] < m_pastPositions[i]) {
					successfulChecks[0] += 1;
				}
				else {
					successfulChecks[1] += 1;
				}
			}

			if (successfulChecks[0] == 9) {
				m_hint = "You are gettin closer!";
			}
			else if ((successfulChecks[1] == 9)) {
				m_hint = "Colder. You're moving away from the object!";

			}

		}
	}
}

void ofApp::buttonPressed() {
	//We want to save the coordinates of where the user has hidden the object
	//We need the area that it takes up and it's center	
	cv::Rect object;
	if (selectedColour == SEARCH_OBJECT) {
		object = m_contourFinder.getBoundingRect(m_indexLargestContour[SEARCH_OBJECT]);
	}
	else {
		object = m_hiddenContourFinder.getBoundingRect(m_indexLargestContour[HIDDEN_OBJECT]);
	}
	//Getting the position of the object (this is so we can save the coordinates)
	getObjectPosition(object,m_hiddenObjectPosition);

}

void ofApp::getObjectPosition(cv::Rect object, ofVec3f objectPosition[]) {
	//Saving the position of the object on the screen
	objectPosition[0].x = object.x;
	objectPosition[0].y = object.y;
	objectPosition[0].z = m_objectDistance - RANGE_OF_DETECTION;

	//Saving the are the blox fills
	objectPosition[1].x = object.x + object.width;
	objectPosition[1].y = object.y + object.height;
	//Since we don't know the 3D dimension of the object we are using, this will just have to match up with the z value of the other object
	//This removed 3D collision, but still gives the idea of depth
	objectPosition[1].z = m_objectDistance +RANGE_OF_DETECTION;

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (button == OF_MOUSE_BUTTON_RIGHT) {
		if (USE_WEB_CAM) {


			//This is converting 2D space into 1D array, and then multiplying by the bytes (3 for RGB and 1 for Greyscale)
			const int index = (x + y * m_webcam.getWidth()) * m_webcam.getPixels().getBytesPerPixel();

			//Getting the RBG values for each object
			if (selectedColour == HIDDEN_OBJECT) {
				//This is the hidden object Colour

				m_hiddenObjectColour.r = m_webcam.getPixels()[index];
				m_hiddenObjectColour.g = m_webcam.getPixels()[index + 1];
				m_hiddenObjectColour.b = m_webcam.getPixels()[index + 2];

			}
			else if (selectedColour == SEARCH_OBJECT) {
				//This is the searched object colour

				m_searchObjectColour.r = m_webcam.getPixels()[index];
				m_searchObjectColour.g = m_webcam.getPixels()[index + 1];
				m_searchObjectColour.b = m_webcam.getPixels()[index + 2];

			}



		}
	} //Interaction with the colour boxes
	if (button == OF_MOUSE_BUTTON_LEFT) {
		if (USE_WEB_CAM) {
			int pos[2] = { x, y };
			if ((pos[0] > 750) && (pos[0] < 810)) {
				if ((pos[1] < 230) && (pos[1] > 190)) {
					//This is the hidden object Colour

					selectedColour = SEARCH_OBJECT;
				}
				else if ((pos[1] < 190) && (pos[1] > 150)) {
					//This is the searched object colour

					selectedColour = HIDDEN_OBJECT;
				}
				
			}

			
		}
	}
}

void ofApp::drawContour(int index, ofxCv::ContourFinder contourFinder) {

	cv::Point center = contourFinder.getCenter(index);
	cv::Rect rect = contourFinder.getBoundingRect(index);

	ofFill();
	ofSetColor(255, 255, 255);
	ofDrawCircle(center.x, center.y, 5.0f);

	//bounding box
	ofNoFill();
	ofSetColor(255, 0, 0);
	ofDrawRectangle(rect.x, rect.y, rect.width, rect.height);

	//drawing hull
	ofSetColor(255, 255, 0);
	const ofPolyline hull = ofxCv::toOf(contourFinder.getConvexHull(index));
	hull.draw();

	ofSetColor(255, 255, 255);
	
	//Filling so rectangles are with user colours are filled
	ofFill();
}


void ofApp::calibrateFocalLength() {
	//Calibrating will require the user to fill the frame with the object. 
	//We call this distance 1. We do this to start the coordinate system at that point
	//so we can say focalLength = blob.width * 1 / m_hiddenObjectWidth;
	//or focalLength = blob.width / m_hiddenObjectWidth;
	

	if (selectedColour == HIDDEN_OBJECT) {
		cv::Rect blob = m_hiddenContourFinder.getBoundingRect(m_indexLargestContour[HIDDEN_OBJECT]);

		m_focalLength[HIDDEN_OBJECT] = blob.width / m_hiddenObjectWidth;
	}
	else {
		cv::Rect blob = m_contourFinder.getBoundingRect(m_indexLargestContour[SEARCH_OBJECT]);

		m_focalLength[SEARCH_OBJECT] = blob.width / m_searchObjectWidth;

	}

}

void ofApp::findDistance(float focalLength, int index, ofxCv::ContourFinder contourFinder) {
	//D’ = (Width x Focal) / PixelWidth
	//This finds the distance
	cv::Rect blob = contourFinder.getBoundingRect(index);

	if (selectedColour == HIDDEN_OBJECT) {
		m_objectDistance = (m_hiddenObjectWidth * focalLength) / blob.width;
	}
	else {
		m_objectDistance = (m_searchObjectWidth * focalLength) / blob.width;
	}
	
	//ofDrawBitmapString("Distance Calc: " + ofToString(m_objectDistance*10.0), 600, 300);
}