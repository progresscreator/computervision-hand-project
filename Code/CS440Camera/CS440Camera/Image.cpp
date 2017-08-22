#include "StdAfx.h"
#include "Image.h"

Image::Image(void)
{
	this->img=NULL;
}

Image::Image(IplImage *img)
{
	this->img=img;
}


Image::~Image(void)
{
	if(this->img!=NULL)
	{
		cvRelease((void**)&this->img);
	}
}

int Image::getR(int x,int y)
{
	if(img==NULL)
		throw "Image is NULL";

	return CV_IMAGE_ELEM( img, uchar, y, 3*x+2);
}

int Image::getG(int x,int y)
{
	if(img==NULL)
		throw "Image is NULL";
	
	return CV_IMAGE_ELEM( img, uchar, y, 3*x+1);
}

int Image::getB(int x,int y)
{
	if(img==NULL)
		throw "Image is NULL";
	
	return CV_IMAGE_ELEM( img, uchar, y, 3*x);
}

Color Image::get(int x,int y)
{
	Color c;
	c.r = CV_IMAGE_ELEM( img, uchar, y, 3*x+2);
	c.g = CV_IMAGE_ELEM( img, uchar, y, 3*x+1);
	c.b = CV_IMAGE_ELEM( img, uchar, y, 3*x+0);
	return c;
}

void Image::set(int x,int y, Color& c)
{
	set(x,y,c.r,c.g,c.b);
}

void Image::set(int x,int y, int r, int g, int b)
{
	if(img==NULL)
		throw "Image is NULL";

	CV_IMAGE_ELEM( img, uchar, y, 3*x+2) = r;
	CV_IMAGE_ELEM( img, uchar, y, 3*x+1) = g;
	CV_IMAGE_ELEM( img, uchar, y, 3*x) = b;
}


Image* Image::copy()
{
	if(img==NULL)
		throw "Image is NULL";

	CvSize size;
	size.width = img->width;
	size.height = img->height;

	IplImage* newImg = cvCreateImage(size, IPL_DEPTH_8U, 3);	
	cvCopy(this->img,newImg);
	return new Image(newImg);

}

Image* Image::getXGradient()
{
	if(img==NULL)
		throw "Image is NULL";

	CvSize size;
	size.width = img->width;
	size.height = img->height;

	IplImage* newImg = cvCreateImage(size, IPL_DEPTH_8U, 3);	
	cvSobel( img, newImg, 1, 0,5);
	return new Image(newImg);
}

Image* Image::getYGradient()
{
	if(img==NULL)
		throw "Image is NULL";

	CvSize size;
	size.width = img->width;
	size.height = img->height;

	IplImage* newImg = cvCreateImage(size, IPL_DEPTH_8U, 3);	
	cvSobel( img, newImg, 0, 1, 5);
	return new Image(newImg);
	
}


void Image::smooth(int xNeighborhood, int yNeighborhood)
{
	if(img==NULL)
		throw "Image is NULL";

	CvSize size;
	size.width = img->width;
	size.height = img->height;

	IplImage* newImg = cvCreateImage(size, IPL_DEPTH_8U, 3);		
	cvSmooth( img, newImg, CV_GAUSSIAN,xNeighborhood,yNeighborhood);

	cvRelease((void**)&img);
	img=newImg;
}

void Image::scaleDown(int scaleFactor)
{
	if(scaleFactor <= 1)
		throw "invalid scaleFactor:"+scaleFactor;


	for(int i = 1; i < scaleFactor; i++)
	{
		int width = img->width;
		if(width %2==1)
			width++;
		int height = img->height;
		if(height %2==1)
			height++;
		width/=2;
		height/=2;
		
		IplImage* newImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);		
	
		cvPyrDown(img, newImg, CV_GAUSSIAN_5x5);
		cvRelease((void**)&img);
		img=newImg;
	}
}

void Image::drawRect( Rect r, Color c,int width)
{
	if(this->img==NULL)
		throw "Image is null";

	int imgWidth = img->width;
	int imgHeight = img->height;
	
	if(width%2==0)
		width++;

	long i=0;

	for(int y = r.top-width/2; y <= r.bottom+width/2; y++)
	{
		i++;

		if(y<0 || y>=imgHeight)
			continue;
		for(int x = r.left-width/2; x <= r.left+width/2; x++)
		{
			if(x<0 || x>=imgWidth)
				continue;
			set(x,y,c);
		}	
		for(int x = r.right-width/2; x <= r.right+width/2; x++)
		{
			if(x<0 || x>=imgWidth)
				continue;
			set(x,y,c);
		}	

	}

	for(int x = r.left-width/2; x<= r.right+width/2; x++)
	{
		i++;
	
		if(x<0 || x>=imgWidth)
			continue;

		for(int y = r.top-width/2; y <= r.top+width/2; y++)
		{
			if(y<0 || y>=imgHeight)
				continue;
			set(x,y,c);
		}
		for(int y = r.bottom-width/2; y <= r.bottom+width/2; y++)
		{
			if(y<0 || y>=imgHeight)
				continue;
			set(x,y,c);
		}
	}
}
