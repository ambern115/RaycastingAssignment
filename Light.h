//Light.h
//Class stores a Light
#include <string>
#include "Vector3.h" //needed? 
#include "Ray.h"

#ifndef LIGHT_H_
#define LIGHT_H_

class Light {
public:
	Vector3 pos; //position 
	float intensity; 

	Light(Vector3 pos_, float intensity_) {
		pos = pos_;
		intensity = intensity_;
	}

};

#endif