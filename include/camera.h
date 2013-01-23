#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <gccore.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "Math_utils.h"
#include "Wiicraft.h"

class Camera
{
public:
//Constructor
	Camera(MtxP viewP, guVector *positionP, float *pitchP, float *yawP);

//Methods
	guVector getLookVector();
	guVector getUpVector();
	guVector getForwardVector();
	guVector getRightVector();
	void updateViewMatrix();

//private:
	guVector *position;
	float *pitch, *yaw;
	
	MtxP viewMatrix;	
};



#endif