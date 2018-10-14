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
	Vector3 intensity;
	int type;
											//Should change type to an enumeration later
	Light(Vector3 pos_, Vector3 intensity_, int type_) {
		pos = pos_;
		intensity = intensity_;
		type = type_;
	}

	Light() {
		pos = Vector3();
		intensity = Vector3();
		type = 0;
	}

	virtual Vector3 getIntensity() { return Vector3(); }
};

#endif