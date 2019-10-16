Hide and Seek
Zachary van Noppen

The project is meant to replicate hide and seek, with the computer giving hints to where the location of the hidden items is. The game starts by the user selecting a the colour of the object to be hidden. Then the object is calibrated. To calibrate, do the following:
1. Enter the width of the tracked object (inches) on the GUI
2. Right click on the colour to track in the image
3. Adjust threshold
4. Click the "Calibrate" button

Then hide the object! Once placed, click the "Hide Object" button and the coordinates will save. Be sure to keep it in sight of the camera while clicking the button. You can cover it after the position is saved. Calibrate a searching object with the steps above and move it around the room. The program will print out text to the screen that directs you away or towards the object. Once the item is uncovered and the second object intersects, it has been found!

The hardest part of this project was tracking the objects, As the object regresses into the room it gets smaller, which often causes it to be replaced with a bigger contour. Additionally, reflections and shadows cause objects to lose tracking as well.

I learned a lot about proper tracking while looking for solutions to the problem above, but none really gave any performance differences. I tried working in other colour spaces but the tracking was actually worse (HSB was unusably poor). Additionally, building custom planes based on object size is much easier than anticipated. I was able to rebuild this project a few times with different methods very quickly. The submitted project is what proved to work best.
