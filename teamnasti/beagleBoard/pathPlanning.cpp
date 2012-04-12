/*****************************************************************************************
* Author: Terry Max Christy
* Bradley University 
* Nautical Autonomous System with Task Integration (code name NASTI)
* Date written: 10/20/11 Last Modified: 1/26/12
*****************************************************************************************/

#include "stdafx.h"
using namespace std;
using namespace cv;

IplImage* findBuoy(IplImage* in, int horizon, char color, vector<buoy> &buoys, char lighting)
{	
	vector<buoy> buoysSorted;
	//run processing on the stuff in the water only *************************************
	//KEEP 11/5/11
	IplImage* out =  cvCreateImage(cvGetSize(in), in->depth, in->nChannels);
	cvCopy(in, out, NULL);
	int x = out->origin; 
	int y = 0;
	if(horizon < out ->height)
	{
		y = out->origin + horizon;
	}
	else y = out->origin;
	int width = out->width;
	int height = out->height + y;
	int add = 100;
	//Make a recatangle of the water 
	CvRect water = cvRect(x,horizon,width,height);
	cvSetImageROI( in, water);
	//Make subimage of just the water, no point waisting processing power on the sky/trees
	//IplImage *water_img = cvCreateImageHeader(cvSize(water.width,water.height), out->depth, out->nChannels);
	//water_img->origin = out->origin;
	//water_img->widthStep = out->widthStep;
	//water_img->imageData = out->imageData + water.y * out->widthStep + water.x * out->nChannels;

	////************************************************************************************
	//^^^^this block will probably cut processing time down, a good thing. However I am not 
	//sure yet how to integrate it into the following blocks to make it useful
	//************************************************************************************
	IplImage* hsvImg       = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 3);
	IplImage* thresholded  = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
	IplImage* thresholded2 = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
	IplImage* thresholded3  = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);

	CvScalar hsv_min;
	CvScalar hsv_max;
	CvScalar hsv_min2;
	CvScalar hsv_max2;

	//for low light, decrease minimum value  (3rd input)
	//for high light, decrease minimum saturation (2nd input)
	if(color == 'g')
	{
		hsv_min  = cvScalar( 50, 255, 255, 0);
		hsv_max  = cvScalar(70, 255, 255, 0);
		hsv_min2 = cvScalar(50, 200, 50, 0);
		hsv_max2 = cvScalar(70, 255, 255, 0);
	}
	else if (color == 'r')
	{
		hsv_min  = cvScalar( 0, 200, 200, 0);
		hsv_max  = cvScalar(30, 255, 255, 0);
		hsv_min2 = cvScalar(110, 200, 110, 0);
		hsv_max2 = cvScalar(180, 255, 255, 0);
		//hsv_min  = cvScalar( 0, 150, 100, 0);
		//hsv_max  = cvScalar(30, 255, 255, 0);
		//hsv_min2 = cvScalar(100,  150, 100, 0);
		//hsv_max2 = cvScalar(180, 255, 255, 0);
	}
	else if (color == 'y')
	{
		hsv_min  = cvScalar( 0, 5, 250, 0);
		hsv_max  = cvScalar(60, 10, 255, 0);
		hsv_min2 = cvScalar(60,  100, 200, 0);
		hsv_max2 = cvScalar(100, 255, 255, 0);
	}
	else if (color == 'b')
	{
		hsv_min  = cvScalar(40, 0, 0, 0);
		hsv_max  = cvScalar(20, 255, 255, 0);
		hsv_min2 = cvScalar(0,  100, 100, 0);
		hsv_max2 = cvScalar(20, 255, 255, 0);
	}
	else 
	{
		hsv_min  = cvScalar( 40, 255, 255, 0);
		hsv_max  = cvScalar(70, 255, 255, 0);
		hsv_min2 = cvScalar(40,  100, 100, 0);
		hsv_max2 = cvScalar(70, 255, 255, 0);
	}

	cvCvtColor(in, hsvImg, CV_RGB2HSV);

	cvInRangeS(hsvImg,  hsv_min,  hsv_max,  thresholded);
	cvInRangeS(hsvImg, hsv_min2, hsv_max2, thresholded2);
	cvOr(thresholded, thresholded2, thresholded3);

	//Atempt to doctor the image so that the circles are found easier
	if(color != 'y'){ cvSmooth(thresholded3, thresholded3, CV_GAUSSIAN, 3, 3); }

	cvErode(thresholded3, thresholded3, NULL, 1);
	//cvSmooth(thresholded, thresholded, CV_BLUR, 9, 9); //heavy blur
	cvDilate(thresholded3, thresholded3, NULL, 3);
	cvSmooth(thresholded3, thresholded3, CV_GAUSSIAN, 3, 3);

#ifdef debug
	//make an image so I can see what is happening durring the edge detection
	//IplImage* edge = doCanny(thresholded, 100,200, 3);
	//cvNamedWindow( "edge",CV_WINDOW_AUTOSIZE);
	//cvShowImage( "edge", edge );
	//cvReleaseImage(&edge);	
#endif

	/* Using the cvHoughCircles method - not that good. Doesn't find enough of the buoys and is inconsistant*/
	//find  circles
	//CvMemStorage* storage = cvCreateMemStorage(0);
	//CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT,  2, 50, 200, 10, 1, 50);
	/*CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 4, 10, 200, 3, 1, 50);*/
	CBlobResult blobs;
	blobs = CBlobResult( thresholded3, NULL, 0);
	
	buoys.resize(blobs.GetNumBlobs());
	int k = 0;
	for(int i = 0; i < (blobs.GetNumBlobs()); i++)
	{
		//float* p = (float*) cvGetSeqElem(circles, i);
		float x = (float)(blobs.GetBlob(i)->MinX() + (( blobs.GetBlob(i)->MaxX() - blobs.GetBlob(i)->MinX() ) / 2.0));
		
		float y = (float)(blobs.GetBlob(i)->MinY() + (( blobs.GetBlob(i)->MaxY() - blobs.GetBlob(i)->MinY() ) / 2.0));
		float radius =  (float)(blobs.GetBlob(i)->MaxY() - blobs.GetBlob(i)->MinY())/2;
		float diameter = 2*radius;
		float n =0;
		float total = diameter*diameter;
////		for( int k = 0; k < 2*p[2]; k++)
////		{
//			uchar* ptr = (uchar*)(thresholded->imageData  + (int)(y) * thresholded->widthStep);
//		//	for(int f = 0; f < 2*p[2]; f++)
//		//	{
//		//		if(ptr[(int)(p[0]-p[2]+f)]) n++;
//		//		else n = n;
//		//	}
//		//}
		if(radius > 7)
		{
			buoys[k].x= x;
			buoys[k].y = y;
			buoys[k].radius = radius;
			k++;
		}
	}

	//sort the vector of buoys so that the first listed buoys are the 'closest' buoys
	buoys.resize(k);
	buoysSorted.resize(k);
	buoy buoyTemp;
	for(int j = 0; j < k-1; j++)
	{
		for(int i = 0; i < k-1; i++)
		{
			if( buoys[i].y < buoys[i+1].y )
			{
				buoyTemp = buoys[i+1];
				buoys[i+1] = buoys[i];
				buoys[i] = buoyTemp;
			}
		}
	}
	//adjust the buoy y location because the ROI sets a new origin at the top left of ROI
	//the adjustment is needed so the buoys are locates properly relative to the whole image
	for(int j = 0; j < k; j++)
	{
		buoys[j].y = buoys[j].y + horizon;
	}

#ifdef debug
	cvNamedWindow( "HSV",CV_WINDOW_AUTOSIZE);
	cvShowImage( "HSV", thresholded );
	cvNamedWindow( "thresholded",CV_WINDOW_AUTOSIZE);
	cvShowImage( "thresholded", thresholded3 );
	cvNamedWindow( "thresholded2",CV_WINDOW_AUTOSIZE);
	cvShowImage( "thresholded2", thresholded2 );
#endif
	//Used with the Hough Circle Transform
	//cvReleaseMemStorage(&storage);
	//cvReleaseImage(&thresholded);
	
	cvReleaseImage(&thresholded);
	cvReleaseImage(&thresholded2);
	cvReleaseImage(&hsvImg);
	cvReleaseImage(&out);
	return(thresholded3);
}

int constructGates( vector<buoy> &greenBuoys, vector<buoy> &redBuoys, vector<buoy> &yellowBuoys, vector<gate> &gates, bool avoidYellow)
{
	//Check if the boat needs to avoid a yellow buoy, if so set the first target to the yellow buoy
	if(avoidYellow == true)
	{
		//If i wanted to avoid some computation, i could just set the goal to the location
		//of the yellow buoy, however this is currently untested
		gates[0].green = cvPoint(cvRound(yellowBuoys[0].x), cvRound(yellowBuoys[0].y));
		gates[0].red = cvPoint(cvRound(yellowBuoys[0].x), cvRound(yellowBuoys[0].y));
		gates[0].goal = cvPoint(cvRound((gates[0].green.x + gates[0].red.x)/2.0), 
			cvRound((gates[i].green.y + gates[0].red.y)/2.0));
	}
	else 
	{
		//make sure there is a pair of red/green buoys
		if ((greenBuoys.size() > 0) && (redBuoys.size() > 0))
		{
			//determine which color buoys there are more of
			if (greenBuoys.size() > redBuoys.size())
			{
				gates.resize(redBuoys.size());
				for(unsigned int i = 0; i < redBuoys.size(); i++)
				{
					gates[i].green = cvPoint(cvRound(greenBuoys[i].x), cvRound(greenBuoys[i].y));
					gates[i].red = cvPoint(cvRound(redBuoys[i].x), cvRound(redBuoys[i].y));
					gates[i].goal = cvPoint(cvRound((gates[i].green.x + gates[i].red.x)/2.0), 
						cvRound((gates[i].green.y + gates[i].red.y)/2.0));
				}
			}
			else
			{
				gates.resize(greenBuoys.size());
				for(unsigned int i = 0; i < greenBuoys.size(); i++)
				{
					gates[i].green = cvPoint(cvRound(greenBuoys[i].x), cvRound(greenBuoys[i].y));
					gates[i].red = cvPoint(cvRound(redBuoys[i].x), cvRound(redBuoys[i].y));
					gates[i].goal = cvPoint(cvRound((gates[i].green.x + gates[i].red.x)/2.0), 
						cvRound((gates[i].green.y + gates[i].red.y)/2.0));
				}
			}
		}
		else
		{
			if(greenBuoys.size() <= 0 && redBuoys.size() <= 0) return(1);
			else if (greenBuoys.size() <= 0)                   return(2);
			else if (redBuoys.size() <= 0)                     return(3);
			else                                               return(4);
		}
	}
	return(0);
}

bool redRightReturn(vector<gate> &gates)
{
	char redRightCount = 0;
	for(unsigned int i = 0; i < gates.size(); i++)
	{
		if(gates[i].red.x > gates[i].green.y) redRightCount++;
		else redRightCount = redRightCount;
	}
	if(gates.size() > 0) redRightCount = redRightCount/gates.size();
	else redRightCount = 0;
	if(redRightCount >= 1.0) return(TRUE);
	else return(FALSE);
}

int findPath(IplImage *in, vector<gate> &gates, vector<path> &path)
{
	int x, y;
	if(gates.size() > 0){path.resize(gates.size());}
	else {path.resize(1);}
	//cout<<path.size()<<endl;
	path[0].nearEnd = cvPoint( in->width/2, in->height);
	if( gates.size() > 0 )
	{
		path[0].height = (float)in->height;
		path[0].farEnd = gates[0].goal;
		path[0].slope = (float)(path[0].farEnd.y - path[0].nearEnd.y)/(float)(path[0].farEnd.x - path[0].nearEnd.x);
		x = (path[0].farEnd.x - path[0].nearEnd.x);
		y = (path[0].farEnd.y - path[0].nearEnd.y);
		path[0].length = sqrt((float)((x*x) + (y*y)));
		for(unsigned int i = 1; i < gates.size(); i++)
		{
			//cout<<path[i-1].nearEnd.x<<" , "<<path[i-1].farEnd.y<<endl;
			path[i].nearEnd = gates[i-1].goal;
			path[i].farEnd = gates[i].goal;
			path[i].slope = (float)(path[i].farEnd.y - path[i].nearEnd.y)/(float)(path[i].farEnd.x - path[i].nearEnd.x);
			x = (path[i].farEnd.x - path[i].nearEnd.x);
			y = (path[i].farEnd.y - path[i].nearEnd.y);
			path[i].length = (float)y; //sqrt((float)((x*x) + (y*y)));
		}

	}
	else
	{
		path[0].farEnd = cvPoint(cvRound(in->width/2), 0);
		path[0].slope = (float)(path[0].farEnd.y - path[0].nearEnd.y)/(float)(path[0].farEnd.x - path[0].nearEnd.x);
		x = (path[0].farEnd.x - path[0].nearEnd.x);
		y = (path[0].farEnd.y - path[0].nearEnd.y);
		path[0].length = sqrt((float)((x*x) + (y*y)));
	}
	return(0);
}

int constructWall(vector<buoy> &buoys, vector<wall> &walls)
{
	if(buoys.size() > 0)
	{
		walls.resize(buoys.size()-1);
		for(unsigned int i = 0; i < buoys.size()-1; i++)
		{
			walls[i].nearEnd = cvPoint(cvRound(buoys[i].x), cvRound(buoys[i].y));
			walls[i].farEnd = cvPoint(cvRound(buoys[i+1].x), cvRound(buoys[i+1].y));
		}
		return(1);
	}
	else return(0);
}

bool checkForObsticle(vector<buoy> &greenBuoys, vector<buoy> &redBuoys, vector<buoy> &yellowBuoys)
{
	bool obsticle = false;
	//check if a yellow buoy has been detected
	if( (yellowBuoys.size() == 0))
	{
		obsticle = false;
	}
	//determine if the yellow buoy needs to be avoided
	else if(greenBuoys.size() == 0 && redBuoys.size() == 0)
	{
		obsticle = true;
	}
	else
	{
		if(greenBuoys.size() > 0)
		{
			if(yellowBuoys[0].y > greenBuoys[0].y){obsticle = true;}
			else {obsticle = false;}
		}
		else
		{
			if(yellowBuoys[0].y > redBuoys[0].y){obsticle = true;}
			else {obsticle = false;}
		}
	}
	return(obsticle);
}