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
#include "Light.h"
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



Vector3 getColor(Intersection inter, Light light, Vector3 v) {
	Vector3 l = inter.poi - light.pos;
	float d = inter.normal.Normalized().Dot(l.Normalized());

	float f = 0;
	if (d > 0) {
		f = d;
	}

	//Blinn Phong
	//need viewing direction: v
	//compute hacky half vector instead of getting reflectance of light 
	Vector3 h = (v + l).Normalized();
	float s = inter.normal.Normalized().Dot(h.Normalized());

	float f2 = 0;
	if (s > 0) {
		f2 = pow(s, 14);
	}

	Vector3 specular = Vector3(200,200,200) * light.intensity * f2;
	specular.Clamp();

	Vector3 diffuse = inter.color * light.intensity * f;
	diffuse.Clamp();

	Vector3 result = diffuse + specular;


	result.Clamp();

	return result;
}

//TODO: Read from ASCII (P6) PPM files
//Inputs are output variables for returning the image width and heigth
unsigned char* raycast() {
	Sphere s = Sphere(0, 0, 6, 2.5, Vector3(200, 20, 25)); //x, y, z, radius
	Light l = Light(Vector3(3,3,0), 1);
	//(position in space, distance b/w camera and near plane, up vector (determines ratio of smaller square to larger square
	// right vector (also defines ,size of near plane?), point its pointing to)
	Camera c = Camera(Vector3(0,0,0), Vector3(0,0,-1), Vector3(0,1,0), 2); 
	int img_w = screen_width;
	int img_h = screen_height;
	float aspect = img_w / (float)img_h; //set x and y vals you give to camera
	unsigned char* img_data = new unsigned char[4 * img_w*img_h]; //
	for (int i = 0; i < img_h; i++) { //x?
		for (int j = 0; j < img_w; j++) { //y?
			float x = (2 * (j + 0.5) / (float)img_w - 1) * aspect;
			float y = (1 - 2 * (i + 0.5) / (float)img_h);
			//x and y on the near plane

			//assumes 45 degree angle
			Ray r = c.GetRay((float)x, (float)y); //giving camera the pixel to shoot ray through
			Intersection inter = s.Intersect(r); 

			if (inter.collided == true) { //previously if vector is a null ptr...
				Vector3 color = getColor(inter, l, c.w);
				img_data[i*img_w * 4 + j * 4] = color.x; //Red
				img_data[i*img_w * 4 + j * 4 + 1] = color.y;  //Green
				img_data[i*img_w * 4 + j * 4 + 2] = color.z;  //Blue
				img_data[i*img_w * 4 + j * 4 + 3] = 255;  //Alpha
			}
			else {
				img_data[i*img_w * 4 + j * 4] = 0;        //Red
				img_data[i*img_w * 4 + j * 4 + 1] = 0;    //Green
				img_data[i*img_w * 4 + j * 4 + 2] = 0;    //Blue
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

	int img_w = screen_width;
	int img_h = screen_height;
	unsigned char* img_data = raycast();

	//Load the texture into memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	// End Allocate Texture //

	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context


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
