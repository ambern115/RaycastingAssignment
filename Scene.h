//Scene.h
//Class stores a Scene of Objects
#include <string>
#include "Vector3.h"
#include "Camera.h"
#include "Primitive.h"	
#include <vector>
#include "Light.h"

#ifndef SCENE_H_
#define SCENE_H_
class Scene {
public:
	Camera c;
	std::vector<Primitive*> objects;
	std::vector<Light> lights;
	float width;
	float height;
	Vector3 background;
	float max_depth;
	Vector3 ambient_light;
	Scene(Camera c_, std::vector<Primitive*> o_, std::vector<Light> l_, float w_, float h_, Vector3 b_, Vector3 a_, float m_) {
		c = c_;
		objects = o_;
		lights = l_;
		width = w_;
		height = h_;
		background = b_;
		ambient_light = a_;
		max_depth = m_;
	}

	Scene() {
		c = Camera(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0), 45);
		objects = std::vector<Primitive*>();
		lights = std::vector<Light>();
		width = 640;
		height = 480;
		background = Vector3(15, 15, 15);
		ambient_light = Vector3(0, 0, 0);
		max_depth = 5;
	}

	void AddObject(Primitive* o) {
		objects.push_back(o);
	}

	void AddLight(Light l) {
		lights.push_back(l);
	}

};

#endif