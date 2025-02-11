/*****************************************************************************************
* Author: Terry Max Christy
* Bradley University 
* Nautical Autonomous System with Task Integration (code name NASTI)
* Date written: 10/20/11 Last Modified: 1/26/12
*****************************************************************************************/
#pragma once
using namespace std;
using namespace cv; 

int constructGates( vector<buoy> &greenBuoys, vector<buoy> &redBuoys, vector<buoy> &yellowBuoys, vector<gate> &gates, bool avoidYellow, float gateSeperation);
bool redRightReturn(vector<gate> &gates);
int findPath(IplImage* in, vector<gate> &gates, vector<path> &path);
void constructControl(CvPoint *start, CvPoint *End, path *control);
int constructWall(vector<buoy> &Buoys, vector<wall> &walls);
bool checkForObstacle(vector<buoy> &greenBuoys, vector<buoy> &redBuoys, vector<buoy> &yellowBuoys);
CvPoint rollAverage(vector<CvPoint> &averageBuff, vector<path> &path);
void intigrator(CvPoint *sum, CvPoint *newPoint, float Ki, float width);
CvPoint differentiator(CvPoint *old, CvPoint *newPoint, float Kd, float width);