//Intersection.h
//Class stores a object that can be drawn using the raytracer

#include <stdint.h>
#include "Ray.h"

#ifndef INTERSECTION_H_
#define INTERSECTION_H_

class Intersection { //Class holding the Primitive hit, and distance travelled by a Ray to hit the Primitive  
  public:
	Vector3 color;
	bool collided;
	float t;
	Vector3 normal; //what is the normal of the point hit???
	Vector3 poi; //point of intersection
  
	Intersection(Vector3 color_, float t_, bool collided_, Vector3 normal_, Vector3 poi_) {
		color = color_;
		t = t;
		collided = collided_;
		normal = normal_;
		poi = poi_;
	}
	Intersection() {
		color = Vector3(0,0,0); 
		t = 0;
		collided = false;
		normal = Vector3(0,0,0);
	}
};


#endif
