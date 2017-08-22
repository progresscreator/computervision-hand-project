#pragma once 

/******
 * Simple color class
 *
 * Sam Epstein
 ******/
class Color
{
public:

	Color(){}
	Color(int r, int g, int b)
	{
		this->r=r;
		this->g=g;
		this->b=b;
	}

	int r;
	int g;
	int b;
};