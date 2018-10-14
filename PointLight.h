//PointLight.h
//Class stores a Point Light
#include <string>
#include "Vector3.h" //needed? 
#include "Ray.h"
#include "Light.h"

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

class PointLight : public Light {
public:
	PointLight(Vector3 pos_, Vector3 intensity_) : Light(pos_, intensity_, 0) {}
};

#endif