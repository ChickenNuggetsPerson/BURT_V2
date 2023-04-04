# BURT_V2


This is just the initial starting code for Burt before the new 2023 - 2024 game is released

Just a simple framework to build off of once the game releases

Since the 2022 - 2023 game year was my first year programming in c++, I am slowly learning how to do this so please don't judge my lack of industry standards when programming ._.


# Framework Features:

## Brain Displayer:

### Pages:

The displayer is easily editable to show any debug information needed on the brain screen. Currently it can show information in two formats; progress bars or line graphs. Everything is handled by the Pages class. Once declared, the different graph types can be added to the page and their values can be updated. Each graph has an Id assigned to it. 

`.addHorzProgressBar(const char* barId, int x, int y, int width, int  height, const  char* name, bool middle = false, colorRange ranges[] = nullptr);`
    
 `.addVertProgressBar(const char* barId, int x, int y, int width, int  height, const  char*  name, bool  middle = false, colorRange ranges[] = nullptr)`
    
 `.addLineGraph(const char* graphId, int graphType, const char* graphName, int  xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, colorRange  colorRanges[] = nullptr, int maxDataPoints = 10)`

Once created, data can be added to the graphs or bars using the `.setProgressBarValue()` and `.setLineGraphValue()` methods. ( Using the ids of the bars and graphs)

### Rendering Pages:
To render the page, just call the `.render()` method. 

### Color Gradients:

For all display elements, they can be colored using a color gradient. All color gradients are comprised of an array of `colorRange` objects. ColorRanges are comprised of a min, max, and color. When drawing, the drawer checks if the value is in that range and if it is, it will draw it with that color. Color Ranges can either be "blocky" or "smooth". 

Non-linear gradients are manually created by the user defining their own array of colorRanges:

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

Using it in a horizontal progress bar:
`homePage.addHorzProgressBar("battery", 325, 15, 150, 30, "Battery: %d%%", false, batteryGradient.finalGradient);`

### Log Messages:
The logging system is controlled by the `Logger` object. When creating it, it is given the row and column that it is drawn at. The Logger then can be added to a `Page` by using the `.addLogger()` method.

    homePage.addLogger(&BrainLogs); // Add the pointer to the logger to the page

By default, the Logger object is not extended to the rest of the program so use the following functions to send messages to the brain screen from anywhere else in the program.

    void  brainError(const  char*  message); // Send with error color
    void  brainDebug(const  char*  message); // Send with debug color
    void  brainFancyDebug(const  char*  message, vex::color  messageColor); // Send with custom color
    void  brainFancyDebug(const  char*  message, vex::color  messageColor, int  data); // Send some data with the message ( use %d )

### Multiple Pages?
This is something that I need to work on. In the future, there will be some sort of class that handles all pages at once. It will have the ability to switch between pages and correctly render them all.

### Interactables?
Since the V5 Brain has a touch screen, I also plan on making a button system. Where the user can define buttons on the page and have them do something once pressed. They could change the current page or just run some random piece of code. 
