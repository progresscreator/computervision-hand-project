#pragma once

#include "Rect.h"
#include "Color.h"

/**************
 * Helper class which represents an image. The class contains
 * an pointer to a type of IplImage from the OpenCV imaging library.
 * This class allows simple access as well as some helper processing
 * operations.
 * 
 * !WARNING!
 * When creating an instance of the image class, you'll need to pass it
 * a pointer to an instance of the IplImage* class. Note that you should
 * NEVER pass in the exact IplImage* from a captured video. Instead copy
 * the video using cvCopy.
 *
 * Sam Epstein
 **************/
class Image
{
private:

	//Pointer to obect containing all the information
	IplImage *img;

public:
	
	//Constructs an empty instance
	Image(void);

	//Constructs and instance from the specified IplImage file
	Image(IplImage *img);
	~Image(void);

	//Raw data accessor
	IplImage* getImage(){return img;}

	//Raw data modifier
	void setImage(IplImage* img){this->img=img;}
	
	int getWidth(){return img->width;}
	int getHeight(){return img->height;}

	Color get(int x,int y);
	int getR(int x,int y);
	int getG(int x,int y);
	int getB(int x,int y);

	void set(int x,int y, Color &c);
	void set(int x,int y, int r, int g, int b);

	//Creates a new instane of the image
	Image* copy();	

	//Returns the X gradients of the image
	Image* getXGradient();

	//Returns the y gradients of the image
	Image* getYGradient();	

	//Smooths the image by convolving with a gaussian.
	//The parameters represent the level of x and y blurring.
	void smooth(int xNeighborhood, int yNeighborhood);

	//Scales down the image by a factor represented by
	//scaleFactor
	void scaleDown(int scaleFactor);

	//Draws a rectangle with the provided color and width
	//Note that the width is expected to be odd
	//Out of bounds are automatically taken care of.
	void drawRect(Rect r, Color c, int width);
};


