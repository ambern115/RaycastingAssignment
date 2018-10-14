//Camera.h
//Class stores a Camera
#include <string>
#include "Vector3.h" //needed? 
#include "Ray.h"

#ifndef CAMERA_H_
#define CAMERA_H_

class Camera {
public:
	Vector3 eye; //Eye position
	float distance; //Distance to viewing plane
	Vector3 u, v, w; //Camera accross, up, and look vectors

	Camera(Vector3 e_, Vector3 a_, Vector3 b_, float d_) {
		eye = e_; 
		distance = d_;
		w = a_.Normalized();
		u = b_.Cross(w).Normalized();
		v = w.Cross(u);
	}

	//Camera(Vector3 e_, float d_, Vector3 u_, Vector3 v_, Vector3 w_) : eye(e_), distance(d_), u(u_), v(v_) , w(w_) {}
	Ray GetRay(float x, float y) {
		Vector3 s = eye + u * x + v * y - w * distance; //point on the near plane
		return Ray(eye, s - eye); 
	}
};

#endif