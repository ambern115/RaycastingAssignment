//Material.h
//Class stores a Material
#include <string>
#include "Vector3.h"

#ifndef MATERIAL_H_
#define MATERIAL_H_

class Material {
public:
	Vector3 Kd; // diffuse constant (surface color)
	Vector3 Ks; // specular constsant (specular highlight)
	Vector3 Ka; // Ambient constant
	float p; //Blin-Phong exponent

	Material(Vector3 d_, Vector3 s_, Vector3 a_, float p_) {
		Kd = d_;
		Ks = s_;
		Ka = a_;
		p = p_;
	}

	Material(Vector3 d_) {// Just defuse material
		Kd = d_;
	}

	Material() {
		Ka = Vector3(0, 0, 0);
		Kd = Vector3(1, 1, 1);
		Ks = Vector3(0, 0, 0);
		p = 5;
	}
};

#endif