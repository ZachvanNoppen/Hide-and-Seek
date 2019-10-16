#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
/*
PLAN

{OVERVIEW}

Object one is NOT hidden
Track the object being detected by color
Get the distance from the camera to detect the depth of the object
Store object position as the hiding spot in 3d space
	NOTE THAT THIS WILL NOT REPRESENT 3D coordinates, only x,y and distance relative to the CAMERA

Once the object is not found on screen
	Change status to hidden

Track the object two
get the position in 3D space
If it approaches the hidden object coordinates, display message
ElSE show it is moving away

When the coordinates overlap AND object one is NOT hidden (when the objects touch and are seen)
	End game


{DEPTH PLAN}

Depth should be determined by finding focal distance of the camera
So F = PixelWidth * Distance from camera/ Width of object
Or D’ = (Width x Focal) / PixelWidth

We will need to calibrate the program so the object size can varry
Place object as close to camera as we can (while keeping it in frame). This will be the zero distance.
Distance = 0 
Pixel Width = width of image in frame
Width of object = user input

NOTE that the values don't need to be exact, they just need to be consistant. 
The program works as long as the coordinate system is consistant with itself.
Limitting user input to just the estimawted width of the object should be fine to start

Now we have Focal length (from variables above), and we can find D = (W x F) / P for the object

This will be done for the object being hidden and the object searching.
If this gets done in the next day the second object will be replaced with head tracking.

{POTENTIAL ISSUE}
The size of the tracked item is going to change, this algorithm relies on things staying the same shape. 
Object that are not uniform sizes will cause errors in the depth finding
This project is small scale enough I will just use balls as the objects being searched for.

*/

//TODO: Implement a timer feature so buttons are not used.