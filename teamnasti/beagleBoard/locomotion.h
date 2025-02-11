/*****************************************************************************************
// Author: Terry Max Christy
// Bradley University 
// Nautical Autonomous System with Task Integration (code name NASTI)
// Date written: 1/24/12 Last Modified: 1/24/12
*****************************************************************************************/
// #pragma once ensures that any file that is inlcuded after it is only included in the 
// overall project once.
#pragma once


void throttle(float PWM, vector<float> &motors);
void turn( char severity, char direction, vector<float> &motors);
void mainThrust(float motor1, float motor2, vector<float> &motors);
void sideThrust(float frontLeft, float backLeft, float frontRight, float backRight, vector<float> &motors);
void speedGateRun(path *control, vector<float> &motors, float height, float PWMoffset, float maxThrottle, float diffCoef, float leftOff, float rightOff, bool &speedGate, char &inGate, char &gateCount);
void navigateChannel(path *control, vector<float> &motors, float height, float closingOnGateDen, float closingPWM, float PWMoffset, float maxThrottle, float diffCoef, float leftOff, float rightOff);
void avoidObsticle(path *control, vector<path> &path, vector<float> &motors, float height, float PWMoffset, float maxThrottle, float diffCoef, float yellowCoef, float leftOff, float rightOff, float optimalSlope, float deadBand);
void pwm2uchar(vector<float> &motors, unsigned char *motorschar);
void aboutFace(unsigned char direction, vector<float> &motors);