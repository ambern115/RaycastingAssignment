//DirectionalLight.h
//Class stores a Directional Light
#include <string>
#include "Vector3.h" //needed? 
#include "Ray.h"
#include "Light.h"

#ifndef DIRECTIONALLIGHT_H_
#define DIRECTIONALLIGHT_H_

class DirectionalLight : public Light {
public:
	DirectionalLight(Vector3 pos_, Vector3 intensity_) : Light(pos_, intensity_, 1) {}
};

#endif