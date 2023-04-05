# BURT_V2


This is just the initial starting code for Burt before the new 2023 - 2024 game is released

Just a simple framework to build off of once the game releases

Since the 2022 - 2023 game year was my first year programming in c++, I am slowly learning how to do this so please don't judge my lack of industry standards when programming.


# Framework Features:

## Brain Displayer:

### Menu System:
Everything is rendered through the `MenuSystem` class. Add pages to the object and they will be rendered to the screen using the `MenuSystem::render()` function. 

Pages can be added using:<br>
`MenuSystem::addPage(const char* pageId, Page* page)`<br>
and you can go to a page using:<br>
`MenuSystem::gotoPage(const char* pageId)`
<br><br>
Outside of the displayer.cpp file, you can use the following functions to edit data and change pages:<br>
`brainChangePage();`<br>
`brainPageChangeData();`<br>
`brainPageChangeText();`

The Menu System also automatically handles starting and stopping update tasks ( see bellow ).

### Pages:

The display system is easily editable to show any information needed on the brain screen. Currently it can show information in three formats; progress bars, line graphs, or simple text. The `Page` class is used for building individual pages. Once declared, the different graph types can be added to the page and their values can be updated. Each graph has an Id assigned to it. 

`.addHorzProgressBar(barId, x, y, width, height, name, middle, ranges[]);`
    
 `.addVertProgressBar(barId, x, y, width, height, name, middle, ranges[]);`
    
 `.addLineGraph(graphId, graphType, graphName, xPos, yPos, graphWidth, graphHeight, drawMiddle, colorRanges[], maxDataPoints)`

`.addText(text, x, y, displayColor, id)`

Once created, data can be changed using:<br>
`.setProgressBarValue()`<br>
`.setLineGraphValue()` <br>
`.setTextData()`

### Continuously Updating Data:
 
 Page data can be continually updated using the `.addDataUpdaterCB()` function. First, declare your update function as follows:

    int updateDebug(Page* self) {
    
	    return  1;
    }
When run, the update function is given the pointer to the page object. This means that any aspect of the page object can be edited through this update function. In this example, we are setting the progressbars on the page to the temperature of the four different motors.

	int updateDebug(Page* self) {
		
	    self->setProgressBarValue("fl", leftMotorA.temperature(fahrenheit));
	    self->setProgressBarValue("fr", rightMotorA.temperature(fahrenheit));
	    self->setProgressBarValue("bl", leftMotorB.temperature(fahrenheit));
	    self->setProgressBarValue("br", rightMotorB.temperature(fahrenheit));
		
	    return 1;
	}

Once the update function is defined, it is added using the `.addDataUpdaterCB()` function on the page. ( You can also define the time between updates in seconds )

	debugPage.addDataUpdaterCB(updateDebug, 1); // Update the page every second


### Color Gradients:

For all display elements, they can be colored using a color gradient. All color gradients are comprised of an array of `colorRange` objects. ColorRanges are comprised of a min, max, and color. When drawing, the drawer checks if the value is in that range and if it is, it will draw it with that color. Color Ranges can either be "blocky" or "smooth". 

"Blocky" gradients are manually created by the user defining their own array of colorRanges:

    colorRange  myColorRange[4] = { // Make sure to make the array 1 bigger than the amount of colors
	    colorRange(0, 60, green),
	    colorRange(61, 80, yellow),
	    colorRange(81, 100, red),
    };

Or they can be created using the `Gradient` object to make a smooth transition. Gradients can be made with either RGB values using the `rgbColor` object or HSV values.

    Gradient(double startHue, double endHue, int startOfRange, int endOfRange)

    Gradient(rgbColor startColor, rgbColor endColor, int startOfRange, int endOfRange)

Example:

    Gradient myGradient = Gradient(10, 300, 10, 100);
    Gradient myOtherGradient = Gradient(rgbColor("red"), rgbColor(237, 212, 252), 0, 100);
Once created, pass the `.finalGradient` of the gradient to the function of choice.

Using it in a horizontal progress bar:<br>
`homePage.addHorzProgressBar("battery", 325, 15, 150, 30, "Battery: %d%%", false, batteryGradient.finalGradient);`

### Log Messages:
The logging system is controlled by the `Logger` object. When creating a logger, it is given the row and column that it is drawn at. The Logger then can be added to a `Page` by using the `.addLogger()` method.

    homePage.addLogger(&BrainLogs); // Add the pointer to the logger to the page

By default, the Logger object is not extended to the rest of the program so use the following functions to send messages to the brain screen from anywhere else in the program.

    void brainError(const char* message); // Send with error color
    void brainDebug(const char* message); // Send with debug color
    void brainFancyDebug(const char* message, vex::color messageColor); // Send with custom color
    void brainFancyDebug(const char* message, vex::color messageColor, int data); // Send some data with the message ( use %d )



### Interactables?
Since the V5 Brain has a touch screen, I also plan on making a button system. The user will be able to define buttons on the page and have them do something once pressed. They could change the current page or just run some random piece of code. 

### Remove data from pages?
I do plan on adding the ability to remove items from pages. The problem is that everything is stored in arrays for the pages. This means that removing an item on the page can be a pain. I do plan on experimenting with a data structure called Linked Lists. Using Linked Lists would allow for more easy data manipulation but it will take a bit to implement since it is something that is not as standard as arrays.
