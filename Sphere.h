//Sphere.h
//Class stores a sphere

#ifndef SPHERE_H_
#define SPHERE_H_

#include <stdint.h>
#include "Primitive.h"
#include "Material.h"

class Sphere : public Primitive {
public:
	float r; // Radius of sphere
	Material mat;
	Sphere(float x_, float y_, float z_, float r_, Material mat_) : Primitive(x_, y_, z_), r(r_), mat(mat_) {}

	Intersection Intersect(Ray ray) { //if a ray hits this sphere, return a pointer
												 //to this sphere, else return null.
		Vector3 p = ray.point;
		Vector3 d = ray.direction;
		// Ray = Point + Direction * t0
		// Sphere = (x - x1)^2 + (y - y1)^2 + (z - z1)^2 = r^2
		//Substitute Ray x,y,z into Sphere 3d equation, and solve for t. (Quadratic formula required)
		//Quadratic formula values:

		//if the ray direction was not normalized, it would look like this: float A = d.x  * d.x + d.y * d.y + d.z * d.z or d.Dot(d);
		//but since it is normalized, A is equal to 1
		
		
		Vector3 offset = p - v; // Offset sphere position from origin of ray (v is position of sphere, p is origin of Ray)
		float B = offset.Dot(d); 
		float C = offset.Dot(offset) - r * r;
		float disc = B * B - C;
		if (disc < 0) { // if discriminant is less than 0, the ray does not intersect the sphere.
			return Intersection();
		}
		float sqdisc = sqrt(disc); //calculate square root of disc once to increase performance

		float t0 = (-B + sqdisc); //t0 and t1 are the two points of intersection of the ray		
		float t1 = (-B - sqdisc);
		if (t0 < 0 && t1 < 0)
			return Intersection(); // If both t0, and t1 are negative, the sphere is behind the camera
		if (t0 < t1) {
			Vector3 poi = p + d*t0; //point of intersection
			Vector3 normal = poi - Vector3(x,y,z); //point it hit - origin of sphere
			return Intersection(mat, t0, true, normal.Normalized(), poi); //Return the smallest positive t value (first point of intersection)
		}
		else {
			Vector3 poi = p + d*t1; //point of intersection
			Vector3 normal = poi - Vector3(x,y,z);//point it hit - origin of sphere
			return Intersection(mat, t1, true, normal.Normalized(), poi);
		}
	}
};



#endif
