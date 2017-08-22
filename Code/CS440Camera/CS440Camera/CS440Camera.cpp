// CS440Camera.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Image.h"

/******
 * This is the main entry point for the students to 
 * change. The inputted image should be deleted at
 * the end of the function. The image can be 
 * displayed in the "One" window. To create other windows,
 * edit the main function. 
 *
 * VideoLoop returns whether or not the program should stop.
 ******/
bool videoLoop(Image* image)
{
	// Display the Current Frame
	cvShowImage("One",image->getImage());
	
	// Delete Image
	int c = cvWaitKey( 5);
	delete image;
	return (c == 'q');

}

/************
 * The entry point for the program. This initializes
 * the camera input and the displaying window "One".
 ************/
int _tmain(int argc, _TCHAR* argv[])
{
		// Create Capture
		CvCapture* capture = cvCreateCameraCapture(0);
		// Create Window
		cvNamedWindow("One",1);

		// Get First Frame
		IplImage* firstFrame;
		for(int z=0; z<5; z++)
		firstFrame = cvQueryFrame(capture);

		CvSize sz;
		sz.height = firstFrame->height;
		sz.width = firstFrame->width;
		IplImage* firstImg = cvCreateImage(sz, IPL_DEPTH_8U, 3);	
		cvCopy(firstFrame,firstImg);
		cvFlip(firstImg);
		Image* firstImage = new Image(firstImg);

		int gesture_array[51] = {0};
		int framenumber = 0;
		int index = 0;
		int pulse_color = 0;

		for(int i=0; i<firstImage->getHeight(); i++)
				for(int j=0; j<firstImage->getWidth(); j++)
					firstImage->set(j,i,255,255,255);

		// forever
		for(;;)
		{
			framenumber++;
			index = framenumber%50;
			// Get frame 
			IplImage* frame = cvQueryFrame(capture);
			if(!frame)
				break;

			// Set frame size
			CvSize size;
			size.height = frame->height;
			size.width = frame->width;

			// Normalize Image
			IplImage* img = cvCreateImage(size, IPL_DEPTH_8U, 3);	
			cvCopy(frame,img);
			cvFlip(img);
			Image* image = new Image(img);

			// -- Background Subtraction --

			// Perform Difference Calculation
			int THRESHOLD = 10;
			int intensityChange = 0;
			int backgroundChange = 0;
			int previousIntensity = 0;
			int currentIntensity = 0;
			int backgroundIntensity = 0;
			int p_red = 0;
			int p_green = 0;
			int p_blue = 0;
			float backgroundRatio = 0;
			float newRatio = 0;
			for(int i=0; i<image->getHeight(); i++)
				for(int j=0; j<image->getWidth(); j++){

					// Store current frame r,g,b values
					p_red = image->getR(j,i);
					p_green = image->getG(j,i);
					p_blue = image->getB(j,i);

					// Find background frame intensity
					backgroundIntensity = (firstImage->getR(j,i) + firstImage->getG(j,i) + firstImage->getB(j,i))/3;

					// Find current intensity
					currentIntensity = (p_red + p_green + p_blue)/3;

					// Calculate change from background intensity
					backgroundChange = abs(backgroundIntensity - currentIntensity);

					// Find color ratios
					if(firstImage->getG(j,i) != 0)
						backgroundRatio = ((float)firstImage->getR(j,i)/(float)firstImage->getG(j,i))/255;
					else
						backgroundRatio = 1;

					if(image->getG(j,i) != 0)
						newRatio = ((float)image->getR(j,i)/(float)image->getG(j,i))/255;
					else
						newRatio = 1;


					// -- Method 1 -- //
					
			/*		if(backgroundChange > 15 && (image->getR(j,i)>70))
						image->set(j,i,255,10,136);
					

					if(image->getR(j,i)!=255)
						image->set(j,i,0,250,255);
					
				*/
					// -- Method 2 -- //				

					if(image->getR(j,i)< 200)
						image->set(j,i,0,250,255);
										
					if(image->getR(j,i)!=0)
						image->set(j,i,255,10,136);

					
				}

				// -- Smooth and Dilate-- //
	/*			cvDilate( img, img, NULL, 1);
				cvSmooth(img,img,CV_GAUSSIAN, 5, 5 );
				for(int i=0; i<image->getHeight(); i++)
				for(int j=0; j<image->getWidth(); j++){
					if(image->getR(j,i)!=0)
						image->set(j,i,255,10,136);
				}

*/



			// -- Compute Initial Axis of Orientation, Area

			// -- Compute Whole Area -- //
			float wholeMeanY = 0;
			float wholeMeanX = 0;
			float wholeArea = 0;
			int ysmall=INT_MAX;
			int ylarge=INT_MIN;
			int xsmall=INT_MAX;
			int xlarge=INT_MIN;
			for (int i=0; i < image->getHeight(); i++)
				for (int j=0; j < image->getWidth(); j++){

					if(image->getR(j,i)!=0){
						wholeMeanY+=i;
						wholeMeanX+=j;
						wholeArea++;

						if(i<ysmall)
							ysmall=i;
						if(i>ylarge)
							ylarge=i;
						if(j<xsmall)
							xsmall=j;
						if(j>xlarge)
							xlarge=j;
						}

				}

			// -- Compute Whole Centroid -- //
			int wholeCentroidx = 0;
			int wholeCentroidy = 0;
			if(wholeArea!=0){
				wholeCentroidx=(wholeMeanX/wholeArea);
				wholeCentroidy=(wholeMeanY/wholeArea);
			}

			// -- Compute Whole Orientation -- //
			double wA=0, wB=0, wC=0;
			for (int i = 0; i < image->getHeight(); i++) {
				for (int j = 0; j < image->getWidth(); j++) {
					if (image->getR(j,i)!=0) {
						wA += pow((double)(j - wholeCentroidx), 2);
						wB += (j - wholeCentroidx) * (i - wholeCentroidy);
						wC += pow((double)(i - wholeCentroidy), 2);
					}
				}
			}
			wB = wB * 2;

			// Calculate Angle
			double wAlpha = 0;
			if (wC - wA == 0) {}
			else {
				wAlpha = 0.5 * atan(wB/(wC - wA));  //c-a
			}

			float wG=-wholeCentroidx*sin(wAlpha)+wholeCentroidy*cos(wAlpha);

			wAlpha=wAlpha*180/(3.14159);
			if(wB<0 && wAlpha<0)
					wAlpha+=-90;
			else if(wB>0 && wAlpha>0)
				wAlpha+=90;
			wAlpha=wAlpha*(3.14159)/180;




		// Draw Orientation
		/*	for(int y=0; y<25; y++) {
				int x=static_cast<int>(tan(wAlpha)*y);
				if(wholeCentroidx+y>=0 && wholeCentroidx+y<image->getWidth() && wholeCentroidy-x>=0 && wholeCentroidy-x<image->getHeight())
					image->set(wholeCentroidx+y, wholeCentroidy-x, 255, 255, 0);
				if(wholeCentroidx-y>=0 && wholeCentroidx-y<image->getWidth() && wholeCentroidy+x>=0 && wholeCentroidy+x<image->getHeight())
					image->set(wholeCentroidx-y,wholeCentroidy+x,255,255,0);
							  }
		*/
			float arm_alpha = wAlpha;

			// -- End Whole Arm Measurements -- //


			// -- Find Palm Centroid -- //
			float palm_alpha = arm_alpha + (3.14159/2);

			// -- Search Direction A
			int searchCentroidx = wholeCentroidx;
			int searchCentroidy = wholeCentroidy;
			int it = 0;
			int point_count = 0;
			int widestCentroidx = searchCentroidx;
			int widestCentroidy = searchCentroidy;
			while((image->getR(searchCentroidx,searchCentroidy)==255) && it<500){

				int current_points = 0;

				// Check Boundary Condition
				if(searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0)
				{
					// Change Centroid Along Direction A
					searchCentroidx += it;
					searchCentroidy -= tan(arm_alpha)*it;

				}

				// Check Boundary Condition
				if(! (searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0))
					break;

				// Visualize Search Lines
				for(int y=0; y<500; y++) {
					int x=static_cast<int>(tan(palm_alpha)*y);
					if(searchCentroidx+y>=0 && searchCentroidx+y<image->getWidth() && searchCentroidy-x>=0 && searchCentroidy-x<image->getHeight())
						if(image->getR(searchCentroidx+y, searchCentroidy-x) == 255){
					//		image->set(searchCentroidx+y, searchCentroidy-x, 255, 255, 0);
							current_points++;
						}
					if(searchCentroidx-y>=0 && searchCentroidx-y<image->getWidth() && searchCentroidy+x>=0 && searchCentroidy+x<image->getHeight())
						if(image->getR(searchCentroidx-y, searchCentroidy+x) == 255){
						//	image->set(searchCentroidx-y,searchCentroidy+x,255,255,0);
							current_points++;
						}
				}

				// Compare Line Width
				if(current_points>point_count){
					point_count = current_points;
					widestCentroidx = searchCentroidx;
					widestCentroidy = searchCentroidy;
				}


				it++;

				// Check Boundary Condition
				if(! (searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0))
					break;


			}


			// -- Search Direction B
			searchCentroidx = wholeCentroidx;
			searchCentroidy = wholeCentroidy;
			it = 0;
			while((image->getR(searchCentroidx,searchCentroidy)==255) && it<500){

				int current_points = 0;

				// Check Boundary Condition
				if(searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0)
				{
					// Change Centroid Along Direction B
					searchCentroidx -= it;
					searchCentroidy += tan(arm_alpha)*it;

				}

				// Check Boundary Condition
				if(! (searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0))
					break;

				// Visualize Search Lines
				for(int y=0; y<500; y++) {
				int x=static_cast<int>(tan(palm_alpha)*y);
				if(searchCentroidx+y>=0 && searchCentroidx+y<image->getWidth() && searchCentroidy-x>=0 && searchCentroidy-x<image->getHeight())
					if(image->getR(searchCentroidx+y, searchCentroidy-x) == 255){
					//	image->set(searchCentroidx+y, searchCentroidy-x, 255, 255, 0);
						current_points++;
					}
				if(searchCentroidx-y>=0 && searchCentroidx-y<image->getWidth() && searchCentroidy+x>=0 && searchCentroidy+x<image->getHeight())
					if(image->getR(searchCentroidx-y, searchCentroidy+x) == 255){
					//	image->set(searchCentroidx-y,searchCentroidy+x,255,255,0);
						current_points++;
					}
				}

				// Compare Line Width
				if(current_points>point_count){
					point_count = current_points;
					widestCentroidx = searchCentroidx;
					widestCentroidy = searchCentroidy;
				}


				it++;

				// Check Boundary Condition
				if(! (searchCentroidx < image->getHeight() && searchCentroidy < image->getHeight() && searchCentroidx < image->getWidth() && searchCentroidy < image->getWidth() && searchCentroidx > 0 && searchCentroidy > 0))
					break;

			}

			// Place palm_centroid at intersection
			// of arm_elongation and widestCentroid

			int palm_centroidx = widestCentroidx;
			int palm_centroidy = widestCentroidy;
			image->set(palm_centroidx,palm_centroidy,0,0,0);

			// -- Draw Centroid -- //
			/*int drawX = 0;
			int drawY = 0;
			int count = 0;
			int type = 0;
			bool flag = true;
			while(count<9 && flag){

				if (type==0){
						drawX = palm_centroidx - count;
						drawY = palm_centroidy - count;
				}

				if (type==1){
						drawX = palm_centroidx + count;
						drawY = palm_centroidy - count;
				}

				if (type==2){
						drawX = palm_centroidx - count;
						drawY = palm_centroidy + count;
				}

				if (type==3){
						drawX = palm_centroidx + count;
						drawY = palm_centroidy + count;
				}

				if( (drawX < image->getWidth()) && (drawX >= 0) && (drawY < image->getHeight()) && (drawY >=0) )
					image->set(drawX,drawY,255,255,255);

				count++;
				if(count==8){
					count=0;
					type++;
					if(type==4)
						flag=false;
				}
			}
			*/

			// -- Find Fingertip Direction
			bool fingertip_directionA = false;
			bool fingertip_directionB = false;
			int bluecountA=0;
			int bluecountB=0;

			// Find Direction of Axis of Elongation with Most Blackspace Pixels
			searchCentroidx = wholeCentroidx;
			searchCentroidy = wholeCentroidy;
			for(int y=0; y<500; y++) {
				int x=static_cast<int>(tan(arm_alpha)*y);
				if((searchCentroidx+y)>=0 && (searchCentroidx+y)<image->getWidth() && (searchCentroidy-x)>=0 && (searchCentroidy-x)<image->getHeight())
					if(image->getR(searchCentroidx+y,searchCentroidy-x)==0){
						bluecountA++;
						//image->set(searchCentroidx+y, searchCentroidy-x, 0, 0, 0);
					}
				if((searchCentroidx-y)>=0 && (searchCentroidx-y)<image->getWidth() && (searchCentroidy+x)>=0 && (searchCentroidy+x)<image->getHeight())
					if(image->getR(searchCentroidx-y,searchCentroidy+x)==0){
						bluecountB++;
					 // image->set(searchCentroidx-y,searchCentroidy+x,0,0,0);
					}
						
			}

			// Set Direction of Most Blackspace Pixels
			if(bluecountA < bluecountB)
				fingertip_directionB = true;
			else
				fingertip_directionA = true;

			// Draw Fingertip Direction
			// Visualize Search Lines
			searchCentroidx = wholeCentroidx;
			searchCentroidy = wholeCentroidy;
			int last_x = 0;
			int last_y = 0;
			for(int y=0; y<500; y++) {
				int x=static_cast<int>(tan(arm_alpha)*y);
				if(fingertip_directionA && (searchCentroidx+y)>=0 && (searchCentroidx+y)<image->getWidth() && (searchCentroidy-x)>=0 && (searchCentroidy-x)<image->getHeight()){
				//	image->set(searchCentroidx+y, searchCentroidy-x, 0, 0, 0);
					last_x = searchCentroidx+y;
					last_y = searchCentroidy-x;
				}
				if(fingertip_directionB && (searchCentroidx-y)>=0 && (searchCentroidx-y)<image->getWidth() && (searchCentroidy+x)>=0 && (searchCentroidy+x)<image->getHeight()){
					//image->set(searchCentroidx-y,searchCentroidy+x,0,0,0);
					last_x = searchCentroidx-y;
					last_y = searchCentroidy+x;
				}
			
			}


			// -- Find Fingertip --

			// Find point at which fingertip_direction goes off-screen.
			// Find point on hand with shortest euclidean distance to that point.
			// Mark this point as current_fingertip.


			// Find Offscreen Point Location of Fingertip Line
			int fingertip_goalx = last_x;
			int fingertip_goaly = last_y;

			// Find Hand Point with Shortest Euclidean Dist to fingertip_goal
			int shortest_dist = INT_MAX;
			int current_fingertipx = 0;
			int current_fingertipy = 0;
			for(int i=0; i<image->getHeight(); i++)
				for(int j=0; j<image->getWidth(); j++)
					if(image->getR(j,i)==255 && image->getG(j,i)==10 && image->getB(j,i)==136){

						float t1 = (j-last_x)*(j-last_x);
						float t2 = (i-last_y)*(i-last_y);
						t1 += t2;
						t1 = sqrt((float)t1);

						if(t1 < shortest_dist){
							shortest_dist = t1;
							current_fingertipx = j;
							current_fingertipy = i;
						}

					}



			// -- Draw Fingertip -- //
			int drawX1 = 0;
			int drawY1 = 0;
			int count1 = 0;
			int type1 = 0;
			bool flag1 = true;
			while(count1<9 && flag1){

				if (type1==0){
						drawX1 = current_fingertipx - count1;
						drawY1 = current_fingertipy - count1;
				}

				if (type1==1){
						drawX1 = current_fingertipx + count1;
						drawY1 = current_fingertipy - count1;
				}

				if (type1==2){
						drawX1 = current_fingertipx - count1;
						drawY1 = current_fingertipy + count1;
				}

				if (type1==3){
						drawX1 = current_fingertipx + count1;
						drawY1 = current_fingertipy + count1;
				}

				if( (drawX1 < image->getWidth()) && (drawX1 >= 0) && (drawY1 < image->getHeight()) && (drawY1 >=0) ){
					int r = abs(image->getR(drawX1,drawY1) - 255);
					int g = abs(image->getG(drawX1,drawY1) - 255);
					int b = abs(image->getB(drawX1,drawY1) - 255);
					image->set(drawX1,drawY1,r,g,b);
				}

				count1++;
				if(count1==8){
					count1=0;
					type1++;
					if(type1==4)
						flag1=false;
				}
			}

			// Find Distance from Palm Centroid to Finger Tip
			float temp1 = (wholeCentroidx-current_fingertipx)*(wholeCentroidx-current_fingertipx);
			float temp2 = (wholeCentroidy-current_fingertipy)*(wholeCentroidy-current_fingertipy);
			temp1 += temp2;
			temp1 = sqrt((float)temp1);


			// Make judgements between open, closed, pointing
			// Output judgements


			// -- Color Hand
			for(int i=0; i<image->getHeight(); i++)
				for(int j=0; j<image->getWidth(); j++)
					if(image->getR(j,i)==255 && image->getG(j,i)==10 && image->getB(j,i)==136){

						float t1 = (j-current_fingertipx)*(j-current_fingertipx);
						float t2 = (i-current_fingertipy)*(i-current_fingertipy);
						t1 += t2;
						t1 = sqrt((float)t1);

						if(t1>temp1)
							image->set(j,i,0,0,0);

					}

			int most_x = INT_MIN;
			int most_y = INT_MIN;
			int least_x = INT_MAX;
			int least_y = INT_MAX;
			for(int i=0; i<image->getHeight(); i++)
				for(int j=0; j<image->getWidth(); j++)
					if(image->getR(j,i)==255 && image->getG(j,i)==10 && image->getB(j,i)==136){

						if(j<least_x)
							least_x = j;
						if(i<least_y)
							least_y = i;
						if(j>most_x)
							most_x = j;
						if(i>most_y)
							most_y = i;
					}

			// Draw White Box
			for(int i=least_y; i<most_y; i++)
				for(int j=least_x; j<most_x; j++)
					if(image->getR(j,i)!=255)
						image->set(j,i,255,255,255);

			// -- Compute Hand, Whitespace Area -- //
			int hand_area = 0;
			int white_area = 0;
			int hand_meanx = 0;
			int hand_meany = 0;
			for (int i=0; i < image->getHeight(); i++)
				for (int j=0; j < image->getWidth(); j++){
					if(image->getR(j,i)==255 && image->getG(j,i)==10 && image->getB(j,i)==136){
						hand_area++;
						hand_meany+=i;
						hand_meanx+=j;
					}
					else if (image->getR(j,i)==255 && image->getG(j,i)==255 && image->getB(j,i)==255)
						white_area++;
				}

			// -- Compute Hand Centroid
			int hand_centroidx = 0;
			int hand_centroidy = 0;
			if(hand_area!=0){
				hand_centroidx=(hand_meanx/hand_area);
				hand_centroidy=(hand_meany/hand_area);
			}

			// -- Draw Hand Centroid
			int drawX2 = 0;
			int drawY2 = 0;
			int count2 = 0;
			int type2 = 0;
			bool flag2 = true;
			while(count2<9 && flag2){

				if (type2==0){
						drawX2 = hand_centroidx - count2;
						drawY2 = hand_centroidy - count2;
				}

				if (type2==1){
						drawX2 = hand_centroidx + count2;
						drawY2 = hand_centroidy - count2;
				}

				if (type2==2){
						drawX2 = hand_centroidx - count2;
						drawY2 = hand_centroidy + count2;
				}

				if (type2==3){
						drawX2 = hand_centroidx + count2;
						drawY2 = hand_centroidy + count2;
				}

				if( (drawX2 < image->getWidth()) && (drawX2 >= 0) && (drawY2 < image->getHeight()) && (drawY2 >=0) ){
					int r = abs(image->getR(drawX2,drawY2) - 255);
					int g = abs(image->getG(drawX2,drawY2) - 255);
					int b = abs(image->getB(drawX2,drawY2) - 255);
					image->set(drawX2,drawY2,r,g,b);
				}

				count2++;
				if(count2==8){
					count2=0;
					type2++;
					if(type2==4)
						flag2=false;
				}
			}


			// -- Compute Hand Circularity
			double hand_circ = 0;
			double r = sqrt(hand_area/3.14159);
			double t = 0.00000;

			for (int i=0; i < image->getHeight(); i++)
				for (int j=0; j < image->getWidth(); j++)
					if(image->getR(j,i)==255 && image->getG(j,i)==10 && image->getB(j,i)==136){
					float tmp = (j-hand_centroidx)*(j-hand_centroidx)+(i-hand_centroidy)*(i-hand_centroidy);
						if((sqrt(tmp))<r){
							t=t+1;
						}
					}

			if(hand_area!=0)
				hand_circ=(t/hand_area);
	
			// -- End Circularity --//

			// Reset White Box
			for(int i=least_y; i<most_y; i++)
				for(int j=least_x; j<most_x; j++)
					if(image->getR(j,i) == 255 && image->getG(j,i) == 255 && image->getB(j,i)==255)
						image->set(j,i,0,250,255);


			// Find Distance from Hand Centroid to Finger Tip
			float temp3 = (hand_centroidx-current_fingertipx)*(hand_centroidx-current_fingertipx);
			float temp4 = (hand_centroidy-current_fingertipy)*(hand_centroidy-current_fingertipy);
			temp3 += temp4;
			temp3 = sqrt((float)temp3);


					for(int i=0; i<image->getHeight(); i++)
					for(int j=0; j<image->getWidth(); j++)
						if(image->getR(j,i)==0 && image->getG(j,i)==250 && image->getB(j,i)==255){
							float t1 = (j-hand_centroidx)*(j-hand_centroidx);
							float t2 = (i-hand_centroidy)*(i-hand_centroidy);
							t1 += t2;
							t1 = sqrt((float)t1);

							if(t1<temp3)
								image->set(j,i,255,255,255);

					}

			float ratio = 0;
			if(hand_area != 0)
				ratio = (float)hand_area/white_area;

			// Fist Ratio: 1 - 3, usually in upper 2.x's
			// Open Ratio: 0.6 - 1.5, usually around 1.0
			// Point Ratio: 0.7 - 2, usually around 1.2

			// Fist Circ: Above 0.9

			// Fist: High Ratio, High Circ
			// Open: Medium Ratio, Medium Circ
			// Point: Medium Ratio, Low-Medium Circ

			// Create Scoring Metrics
			float bigratio = 0;
			if(hand_area != 0)
				bigratio = (float)white_area/ratio;


			float fist_score = 0;
			float nonfist_score = 0;
			float pulse_fist_score = 0;
			int choice = 0;

			// Score Hand Gesture
			if(hand_circ > 0.8)
				fist_score+=0.3;

			if(hand_circ > 0.85)
				fist_score+=0.3;

			if(bigratio < 1500 && bigratio != 0)
				fist_score+=0.3;

			if(ratio > 1.5)
				fist_score+=0.3;

			if(hand_circ > 0 && hand_circ < 0.79)
				nonfist_score+=0.3;

			if(bigratio > 3000)
				nonfist_score+=0.3;

			if(bigratio > 7000)
				nonfist_score+=0.3;

			if(ratio >0 && ratio<1 )
				nonfist_score+=0.3;

			if(fist_score > 1)
				fist_score = 1;
			if(nonfist_score > 1)
				nonfist_score = 1;
			if(nonfist_score == fist_score && nonfist_score!=0)
				fist_score = fist_score - 0.1;
			
			if(fist_score > nonfist_score && fist_score!=0){
				gesture_array[index] = 1;
			}
			else if (fist_score < nonfist_score && nonfist_score !=0){
				gesture_array[index] = 2;
			}

			else
				gesture_array[index] = 0;


			// Keep track of previous gestures
			int fist_count = 0;
			int nonfist_count = 0;
			int null_count = 0;

			for(int i=0; i<51; i++){
				if(gesture_array[i]==1)
					fist_count++;
				else if (gesture_array[i]==2)
					nonfist_count++;
				else if (gesture_array[i]==0)
					null_count++;

			}

			if(abs(fist_count - nonfist_count)<20 && null_count<20){
				pulse_fist_score = 1;
				nonfist_score = 0;
				fist_score = 0;
			}

			if(pulse_fist_score == 1)
				for(int i=0; i<51; i++)
					if(gesture_array[i]==1)
						gesture_array[i]=2;

			// Print Guess
			printf("-----------------------------------------\n");
			printf("Fist Score: %f\n",fist_score);
			printf("Non-Fist Score: %f\n",nonfist_score);
			printf("Pulse Score: %f\n",pulse_fist_score);
			printf("-----------------------------------------\n\n");


			// Change Background Color if Pulse Fist Detected
			if(pulse_fist_score == 1)
				pulse_color = (pulse_color+1) %2;

			if(pulse_color == 0)
				for (int i=0; i < image->getHeight(); i++)
					for (int j=0; j < image->getWidth(); j++)
						image->set(j,i,255,255,255);

			else if (pulse_color == 1)
				for (int i=0; i < image->getHeight(); i++)
					for (int j=0; j < image->getWidth(); j++)
						image->set(j,i,0,250,255);

			// Reset Image for Fist Detected
			int yescount = 0;
			if(fist_score > nonfist_score && fist_score > pulse_fist_score)
				for(int i=0; i<51; i++){
					if(gesture_array[i]==1)
						yescount++;
					else
						yescount--;
				}

			if(yescount>20)
				if (pulse_color == 0)
					for (int i=0; i < image->getHeight(); i++)
						for (int j=0; j < image->getWidth(); j++){
							image->set(j,i,255,255,255);
							firstImage->set(j,i,255,255,255);
						}

			if(yescount>20)
				if (pulse_color == 1)
					for (int i=0; i < image->getHeight(); i++)
						for (int j=0; j < image->getWidth(); j++){
							firstImage->set(j,i,255,255,255);
							image->set(j,i,0,250,255);
						}

			// Draw with Fingertip!
			if(nonfist_score > fist_score && nonfist_score > pulse_fist_score){
					int drawX1 = 0;
					int drawY1 = 0;
					int count1 = 0;
					int type1 = 0;
					bool flag1 = true;
					while(count1<9 && flag1){

						if (type1==0){
								drawX1 = current_fingertipx - count1;
								drawY1 = current_fingertipy - count1;
						}

						if (type1==1){
								drawX1 = current_fingertipx + count1;
								drawY1 = current_fingertipy - count1;
						}

						if (type1==2){
								drawX1 = current_fingertipx - count1;
								drawY1 = current_fingertipy + count1;
						}

						if (type1==3){
								drawX1 = current_fingertipx + count1;
								drawY1 = current_fingertipy + count1;
						}

						if( (drawX1 < image->getWidth()) && (drawX1 >= 0) && (drawY1 < image->getHeight()) && (drawY1 >=0) ){
							int r = abs(image->getR(drawX1,drawY1) - 255);
							int g = abs(image->getG(drawX1,drawY1) - 255);
							int b = abs(image->getB(drawX1,drawY1) - 255);
							firstImage->set(drawX1,drawY1,255,10,136);
						}

						count1++;
						if(count1==8){
							count1=0;
							type1++;
							if(type1==4)
								flag1=false;
						}
					}
				}

			for (int i=0; i < image->getHeight(); i++)
				for (int j=0; j < image->getWidth(); j++)
					if(firstImage->getG(j,i) == 10)
						image->set(j,i,255,10,136);


			if(videoLoop(image))
				break;
		}

		// Free Variables
		cvReleaseCapture(&capture);
		cvDestroyWindow("One");
	
	return 0;
}
