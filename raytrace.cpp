//HW 0 - Moving Square
//Starter code for the first homework assignment.
//This code assumes SDL2 and OpenGL are both properly installed on your system

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "Vector3.h"
#include "Sphere.h"
#include "Ray.h"
#include "Camera.h"
//#include "Light.h"
#include "DirectionalLight.h"
#include "Scene.h"
#include "PointLight.h"
using namespace std;



float vertices[] = {  //These values should be updated to match the square's state when it changes
					  //  X     Y     R     G     B     U    V
	1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
	-1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
	-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f  // bottom left
};

int screen_width = 800;
int screen_height = 600;

float g_mouse_down = false;

void mouseClicked(float mx, float my);
void mouseDragged(float mx, float my);
//////////////////////////
///  Begin your code here
/////////////////////////



Intersection FindIntersection(Ray ray, Scene scene) {
	Intersection temp = Intersection();
	if (scene.objects.size() < 0)
		return temp;
	float min_t = 99;
	for (int k = 0; k < scene.objects.size(); k++) {
		Sphere* s = dynamic_cast<Sphere*>(scene.objects[k]);
		Intersection inter = s->Intersect(ray);
		if (inter.collided && inter.t > 0.01 && inter.t < min_t) {
			min_t = inter.t;
			temp = inter;
		}
	}

	return temp;
}

Vector3 getColor(Intersection inter, Scene scene, Vector3 v) {
	
	if (scene.lights.empty()) {
		return scene.ambient_light;
	}
	
	Vector3 result;
	for (int i = 0; i < scene.lights.size(); i++) {
		Light* ablight = scene.lights[i];
		DirectionalLight* lg;
		Vector3 light_intensity;
		Vector3 l;
		switch (ablight->type)
		{
		case 0: //Point Light
			l = ablight->pos - inter.poi; // light direction
			light_intensity = ablight->intensity * (1 / l.MagSquared());//Falloff at 1 / d^2
			break;
		case 1: //Directional Light
			l = ablight->pos; //Directional light's direction is stored in the pos attribute
			light_intensity = ablight->intensity; //No falloff
			break;
		default:
			break;
		}
		/// Calculate Shadows:
		//	 cast another ray from the point of intersection to the light, 
		//	 If it hits an object, that point is a shadow

									//l * 0.01 is Fudge Factor, reduce shadow acne
		Ray shadowray = Ray(inter.poi + l * 0.01, l);
		Intersection int_light = FindIntersection(shadowray, scene); //Slow, don't need to know which surface hit first
		Vector3 intensity;
		if (int_light.collided) {
			intensity = Vector3(0, 0, 0); //Shadow
		}
		else {
			intensity = light_intensity;
		}
	
		float d = inter.normal.Dot(l.Normalized());
		float f = 0;
		if (d > 0) {
			f = d;
		}

		//Blinn Phong
		//need viewing direction: v
		//compute hacky half vector instead of getting reflectance of light 
		Vector3 h = (v + l).Normalized();
		float s = inter.normal.Dot(h);
		float f2 = 0;
		if (s > 0) {
			f2 = pow(s, inter.mat.p);
		}
		Vector3 specular = inter.mat.Ks * intensity * f2;										
		Vector3 diffuse = inter.mat.Kd * intensity * f;
		result = result + diffuse + specular;
	}
	Vector3 ambient = inter.mat.Ka * scene.ambient_light.x;
	result = result + ambient;
	result.Clamp();
	return result;
}

unsigned char* raycast(Scene scene) {
	//(position in space, distance b/w camera and near plane, up vector (determines ratio of smaller square to larger square
	// right vector (also defines ,size of near plane?), point its pointing to)
	Camera c = scene.c;
	
	screen_height = scene.height;
	screen_width = scene.width;
	int img_w = screen_width;
	int img_h = screen_height;
	float aspect = img_w / (float)img_h; 
	unsigned char* img_data = new unsigned char[4 * img_w*img_h]; //
	float fov = tan(c.h * M_PI / 180);
	//set x and y vals you give to camera
	for (int i = 0; i < img_h; i++) {
		for (int j = 0; j < img_w; j++) {
			//Set background color, then overwrite if an object was hit by the camera ray
			img_data[i*img_w * 4 + j * 4] = scene.background.x;        //Red
			img_data[i*img_w * 4 + j * 4 + 1] = scene.background.y;    //Green
			img_data[i*img_w * 4 + j * 4 + 2] = scene.background.z;    //Blue
			img_data[i*img_w * 4 + j * 4 + 3] = 255;  //Alpha
			float x = (2 * (j + 0.5) / (float)img_w - 1) * aspect * fov; //Get x and y in range of (-1, 1)
			float y = (1 - 2 * (i + 0.5) / (float)img_h) * fov;
			//x and y on the near plane
			Ray r = c.GetRay((float)x, (float)y); //giving camera the pixel to shoot ray through
			Intersection inter = FindIntersection(r, scene);
			if (inter.collided == true) { //previously if vector is a null ptr...
				Vector3 color = getColor(inter, scene, c.w);
				img_data[i*img_w * 4 + j * 4] = color.x; //Red
				img_data[i*img_w * 4 + j * 4 + 1] = color.y;  //Green
				img_data[i*img_w * 4 + j * 4 + 2] = color.z;  //Blue
				img_data[i*img_w * 4 + j * 4 + 3] = 255;  //Alpha
			}
		}
	}

	return img_data;
}
void mouseClicked(float m_x, float m_y) {
	printf("Clicked at %f, %f\n", m_x, m_y);
}

void mouseDragged(float m_x, float m_y) {

}

Scene parsefile() {
	string line;

	string fileName = "spheres2.scn";

	Scene result;
	Material cur_matieral; //Current material

	// open the file containing the scene description
	ifstream input(fileName);

	// check for errors in opening the file
	if (input.fail()) {
		cout << "Can't open file '" << fileName << "'" << endl;
		return result;
	}

	// determine the file size (this is optional -- feel free to delete the 6 lines below)
	streampos begin, end;
	begin = input.tellg();
	input.seekg(0, ios::end);
	end = input.tellg();
	cout << "File '" << fileName << "' is: " << (end - begin) << " bytes long.\n\n";
	input.seekg(0, ios::beg);


	//Loop through reading each line
	string command;
	while (input >> command) { //Read first word in the line (i.e., the command type)

		if (command[0] == '#') {
			getline(input, line); //skip rest of line
			cout << "Skipping comment: " << command << line << endl;
			continue;
		}
		if (command == "camera") { //If the command is a sphere command
			float px, py, pz;
			float dx, dy, dz;
			float ux, uy, uz;
			float ha;

			input >> px >> py >> pz >> dx >> dy >> dz >> ux >> uy >> uz >> ha;

			result.c = Camera(Vector3(px, py, pz), Vector3(dx, dy, dz), Vector3(ux, uy, uz), ha);
			
			
			printf("Camera as position (%f,%f,%f)\n", px, py, pz);
		}

		else if (command == "film_resolution") {
			float width, height;
			input >> width >> height;
			result.width = width;
			result.height = height;
			printf("Resolution is (%f,%f)\n", width, height);
		}

		else if (command == "point_light") {
			float r, g, b;
			float x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			result.AddLight(new PointLight(Vector3(x, y, z), Vector3(r, g, b)));
			printf("Point Light at: (%f,%f,%f)\n", x, y, z);
		}

		else if (command == "directional_light") {
			float r, g, b;
			float x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			result.AddLight(new DirectionalLight(Vector3(x, y, z), Vector3(r, g, b)));
			printf("Point Light at: (%f,%f,%f)\n", x, y, z);
		}

		else if (command == "ambient_light") {
			float r, g, b;
			input >> r >> g >> b;
			result.ambient_light = Vector3(r, g, b);
			printf("Ambient Light with color: (%f,%f,%f)\n", r, g, b);
		}

		else if (command == "sphere") { //If the command is a sphere command
			float x, y, z, r;
			input >> x >> y >> z >> r;
			result.AddObject(new Sphere(x, y, z, r, cur_matieral));
			printf("Sphere as position (%f,%f,%f) with radius %f\n", x, y, z, r);
		}
		else if (command == "background") { //If the command is a background command
			float r, g, b;
			input >> r >> g >> b;
			result.background = Vector3(r * 255, g * 255, b * 255);
			printf("Background color of (%f,%f,%f)\n", r, g, b);
		}
		else if (command == "output_image") { //If the command is an output_image command
			string outFile;
			input >> outFile;
			printf("Render to file named: %s\n", outFile.c_str());
		}
		else if (command == "material") {
			float ar, ag, ab;
			float dr, dg, db;
			float sr, sg, sb, ns;

			input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns;
			cur_matieral = Material(Vector3(dr * 255, dg * 255, db * 255), Vector3(sr * 255, sg * 255, sb * 255), Vector3(ar * 255, ag * 255, ab * 255), ns);
			printf("Material diffuse set to: (%f,%f,%f) \n", dr * 255, dg * 255, db * 255);
			
		}
		else if (command == "max_depth") {
			float n;
			input >> n;
			result.max_depth = n;
			printf("Max Depth of : %f \n", n);
		}
		else {
			getline(input, line); //skip rest of line
			cout << "WARNING. Do not know command: " << command << endl;
		}
	}

	return result;
}











/* OpenGL code */

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec3 inColor;"
"in vec2 inTexcoord;"
"out vec3 Color;"
"out vec2 texcoord;"
"void main() {"
"   Color = inColor;"
"   gl_Position = vec4(position, 0.0, 1.0);"
"   texcoord = inTexcoord;"
"}";

const GLchar* fragmentSource =
"#version 150 core\n"
"uniform sampler2D tex0;"
"in vec2 texcoord;"
"out vec3 outColor;"
"void main() {"
"   outColor = texture(tex0, texcoord).rgb;"
"}";

bool fullscreen = false;

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

	//Ask SDL to get a fairly recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//parsescene file, and set screen resolution
	Scene scene = parsefile();
	unsigned char* img_data = raycast(scene);
	int img_w = screen_width;
	int img_h = screen_height;

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("Raycaster", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);

	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

	float aspect = screen_width / (float)screen_height; //aspect ratio (needs to be updated if the window is resized)


	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//OpenGL functions using glad library
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		printf("OpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}

	// Allocate Texture Created by raycaster //
	GLuint tex0;
	glGenTextures(1, &tex0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	//What to do outside 0-1 range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// = Scene(Camera(), std::vector<Primitive*>(), std::vector<Light>());
	

	//Load the texture into memory
	printf("Spot 1");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	// End Allocate Texture //
	printf("Spot 2");
	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	printf("Spot 3");

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo;
	glGenBuffers(1, &vbo);  //Create vbo buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Set vbo as the active array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo

	//Load the vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	
	//Load the fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Check that the shaders compiled
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("Vertex Shader Compile Failed. Info:\n\n%s\n", buffer);
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("Fragment Shader Compile Failed. Info:\n\n%s\n", buffer);
	}

	//Join the vertex and fragment shaders together into a program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker

	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)

    //Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	//Attribute, vals/attrib., type, normalized?, stride, offset
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib); //Binds the VBO current GL_ARRAY_BUFFER 

	GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(colAttrib);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));

	//Event Loop
	SDL_Event windowEvent;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&windowEvent)) {  //Process input events (e.g., mouse & keyboard)
			if (windowEvent.type == SDL_QUIT) done = true;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
				done = true; //Exit event loop
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
				fullscreen = !fullscreen;
			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
		// Clear the screen to white
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Draw the image
		SDL_GL_SwapWindow(window); //Double buffering
	}

	delete[] img_data;
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);


	//Clean Up
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}
