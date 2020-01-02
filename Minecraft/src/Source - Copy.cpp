#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include "Program.h"
#include "Buffer.h"
#include <string>
using std::string;

#include <iostream>
using std::cout;
#include <Windows.h>
#include <time.h>



#define PARTICLENUM 10000


void main() {
	
	

	glfwInit();
	auto window = glfwCreateWindow(600, 600, "Plot Plus", NULL, NULL);
	glfwMakeContextCurrent(window);


	glewInit();
	glClearColor(0.5f, 0.5f, 0.5f, 1);

	string vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;


void main() {
    gl_Position = vPosition;
}            

)V0G0N";


	string fragmentShdader =
		R"V0G0N(
#version 330 core

out vec4 fragColor;


void main(){
	fragColor = vec4(1,0,0,1);
}            

)V0G0N";

	unsigned int myProgram = CreateProgram(vertexShader, fragmentShdader);



	
	

	struct Particle {
		glm::vec4 pos, vel;
	};
	Particle* particles = new Particle[PARTICLENUM];

	int x = 0, y = 0;
	for (size_t i = 0; i < PARTICLENUM; i++)
	{

		particles[i].pos = glm::vec4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 0,0);
		particles[i].vel = glm::vec4((rand() % 100) / 100.0f-0.5f, (rand() % 100) / 100.0f-0.5f, 0 ,0)/1000.0f;
	
	}


	cout << particles[0].pos.x << " " << particles[0].vel.x << std::endl;

	
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * PARTICLENUM, particles, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	delete particles;


	string csSrc = R"V0G0N(
		#version 430

		struct Particle{
			vec4 pos;
			vec4 vel;
		};

		layout( std430, binding=3 ) buffer num
		{
			Particle p[ ]; 
		};
		
		         

         layout (local_size_x = 1) in;

         void main() {
			 uint gid = gl_GlobalInvocationID.x;
             p[gid].pos += p[gid].vel/10.0f;


			vec3 pos = p[gid].pos.xyz;
			if(pos.x > 1)
				p[gid].pos.x = -1;
			if(pos.y > 1)
				p[gid].pos.y = -1;
			if(pos.x < -1)
				p[gid].pos.x = 1;
			if(pos.y < -1)
				p[gid].pos.y = 1;

			
         }

	)V0G0N";

	

	unsigned int computeShader = CreateProgram(csSrc);

	
	cout << glGetError() << std::endl;

	glEnable(GL_POINT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(1);
	while (true) {
		auto start = clock();
		
		//std::cin.get();
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(computeShader);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glDispatchCompute(PARTICLENUM, 1, 1);

		glUseProgram(myProgram);


		glBindBuffer(GL_ARRAY_BUFFER, ssbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4)*2, 0);

		glDrawArrays(GL_POINTS, 0, PARTICLENUM);

		//glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, 0);
		auto end = clock();
		cout << (end-start) / 1000.0f << "\r";
		
	}
	
	

}

/*


#include <iostream>
#include <vector>
#include <string>

#include "Buffer.h"
#include "Program.h"

#include <time.h>

using std::string;
using glm::vec3;
using glm::vec2;
using glm::vec4;
using glm::mat4;


struct DotPlot{
	unsigned int program, colourLocation;
	unsigned int transformationMatrixLocation;
	Buffer<GL_ARRAY_BUFFER> vbo;
	vec3 extents;
	int divisions;
public:

	



	DotPlot& operator=(const DotPlot&) = delete;
	DotPlot(const DotPlot& rect) = delete;

	DotPlot() {
		float size = 0.1f;
		glEnable(GL_POINT);
		glEnable(GL_POINTS);
		glPointSize(3);
		divisions = 30;
		vec3 *points = new vec3[divisions*divisions*divisions];

		int i = 0;
		float start = -divisions / 2.0f;
		float end = (float)divisions + start;
		for (float x = start; x < end; x++)
		{
			for (float y = start; y < end; y++)
			{
				for (float z = start; z < end; z++)
				{
					points[i] = vec3(x, y, z)*(size/2);
					i++;
				}
			}
		}



		vbo.SetUsage(GL_DYNAMIC_DRAW);
		vbo.SetData(points, divisions*divisions*divisions*sizeof(vec3));

		delete points;

		string vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;


out vec2 screenPos;

void main() {
    gl_Position = vPosition;
	screenPos = vPosition.xy;
}            

)V0G0N";



		string geometryShader = R"V0G0N(
#version 330 core

#define Count 6
layout (points) in;
layout (points, max_vertices = 256) out;
uniform mat4 transform;

void main() {
	float size = 0.1f/Count;
	
	for(float _x = 0; _x < Count; _x ++){
		for(float _y = 0; _y < Count; _y ++){
			for(float _z = 0; _z < Count; _z ++){
					
				float x = size*_x + gl_in[0].gl_Position.x - 3 * size;
				float y = size*_y + gl_in[0].gl_Position.y - 3 * size;
				float z = size*_z + gl_in[0].gl_Position.z - 3 * size;



				float result = y;
				float f = x*z;


				result = 0.4f;
				f = sqrt(pow(x,2) + y*y + z*z);
	
		
				
				//float dis = abs(result - f);
				//if(dis < 0.1f){
				if(f < abs(result)){
					gl_Position = gl_in[0].gl_Position;
			
					gl_Position += vec3(x,y,z);

					gl_Position = transform * gl_Position;

					EmitVertex();
				}

	    
			}
		}	
	}
}            

)V0G0N";



string	fragmentShdader =
			R"V0G0N(
#version 330 core

out vec4 fragColor;

uniform vec4 colour;


void main(){
	
	fragColor = colour;


}            

)V0G0N";

		program = CreateProgram(vertexShader, geometryShader, fragmentShdader);
		colourLocation = glGetUniformLocation(program, "colour");
		transformationMatrixLocation = glGetUniformLocation(program, "transform");


	}



	void Draw(const vec4& colour, const mat4& transform) {
		glUseProgram(program);

		glUniform4fv(colourLocation, 1, &colour[0]);
		glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &transform[0][0]);


		vbo.Bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


		glDrawArrays(GL_POINTS,0, divisions*divisions*divisions);
	}
};


class Rect {
	unsigned int program, programUV, colourUVLocation, colourLocation, positionLocation, positionUVLocation, scaleLocation, scaleUVLocation;
	unsigned int transformationMatrixLocation;
	unsigned int program2;
	Buffer<GL_ARRAY_BUFFER> vbo;
	Buffer<GL_ELEMENT_ARRAY_BUFFER> ibo;

	Buffer<GL_ARRAY_BUFFER> vbo2;
	Buffer<GL_ELEMENT_ARRAY_BUFFER> ibo2;

public:


	Rect& operator=(const Rect&) = delete;
	Rect(const Rect& rect) = delete;

	Rect() {
		float verts[8] = {
			0, 0,
			0,  1,
			1,  1,
			1, 0,
		};



		vbo.SetUsage(GL_STATIC_DRAW);
		vbo.SetData(verts, sizeof(verts));


		unsigned int indices[6] = {
			0,1,2,
			0,2,3
		};
		ibo.SetUsage(GL_STATIC_DRAW);
		ibo.SetData(indices, sizeof(indices));


		string vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;

uniform vec3 offset;
uniform vec2 scale;

out vec2 uv;

void main() {
	uv = vPosition.xy;
    gl_Position = vPosition * vec4(scale,1,1) + vec4(offset,0);
}            

)V0G0N";


		string fragmentShdader =
			R"V0G0N(
#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform vec4 colour;
uniform sampler2D texture;

void main(){
	fragColor = colour * texture(texture,uv);

}            

)V0G0N";

		programUV = CreateProgram(vertexShader, fragmentShdader);


		positionUVLocation = glGetUniformLocation(programUV, "offset");
		scaleUVLocation = glGetUniformLocation(programUV, "scale");
		colourUVLocation = glGetUniformLocation(programUV, "colour");
		glUniform1i(glGetUniformLocation(programUV, "texture"), 0);



		vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;

uniform vec3 offset;
uniform vec2 scale;

out vec2 screenPos;

void main() {
    gl_Position = vPosition * vec4(scale,1,1) + vec4(offset,0);
	screenPos = vPosition.xy;
}            

)V0G0N";


		fragmentShdader =
			R"V0G0N(
#version 330 core

out vec4 fragColor;

uniform vec4 colour;

uniform mat4 transform;

in vec2 screenPos;

void main(){
	
	
	float _x = screenPos.x*20-10;
	float _y = screenPos.y*20-10;

	float result = 1.5f;


	for(float _z = -5; _z < 5; _z += 0.1f)
	{
		
		vec4 pos = vec4(_x,_y,_z,1) * transform;
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
	
		float f = sqrt(pow(x*2,2) + y*y + z*z);
		
		//result = y;
		//f = x*z;
		
		
			
		float distance = abs(result - f);
		//if(f < result){
		float dis = abs(result - f);
		if(dis < 0.1f){
			fragColor = colour;
			
			if(abs(y) < 0.1f)
				fragColor = vec4(0,0,0,1);
			if(abs(z) < 0.1f)
				fragColor = vec4(0,0,0,1);
			if(abs(x) < 0.1f)
				fragColor = vec4(0,0,0,1);
			return;
		}
	}
	
	discard;
	

	

}            

)V0G0N";

		program = CreateProgram(vertexShader, fragmentShdader);

		positionLocation = glGetUniformLocation(program, "offset");
		scaleLocation = glGetUniformLocation(program, "scale");
		colourLocation = glGetUniformLocation(program, "colour");
		transformationMatrixLocation = glGetUniformLocation(program, "transform");



//Program 2


		vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;

uniform vec3 offset;
uniform vec2 scale;

out vec2 screenPos;

void main() {
    gl_Position = vPosition * vec4(scale,1,1) + vec4(offset,0);
	screenPos = vPosition.xy;
}            

)V0G0N";



		string geometryShader = R"V0G0N(
#version 330 core

void main() {
    gl_Position = vPosition * vec4(scale,1,1) + vec4(offset,0);
	screenPos = vPosition.xy;
}            

)V0G0N";



		fragmentShdader =
			R"V0G0N(
#version 330 core

out vec4 fragColor;

uniform vec4 colour;

uniform mat4 transform;

in vec2 screenPos;

void main(){
	
	
	float _x = screenPos.x*20-10;
	float _y = screenPos.y*20-10;

	float result = 1.5f;



	for(float _z = -5; _z < 5; _z += 0.1f)
	{
		
		vec4 pos = vec4(_x,_y,_z,1) * transform;
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
	
		float f = sqrt(pow(x*2,2) + y*y + z*z);
		
		//result = y;
		//f = x*z;
		
		
			
		float distance = abs(result - f);
		//if(f < result){
		float dis = abs(result - f);
		if(dis < 0.1f){
			fragColor = colour;
			
			if(abs(y) < 0.1f)
				fragColor = vec4(0,0,0,1);
			if(abs(z) < 0.1f)
				fragColor = vec4(0,0,0,1);
			if(abs(x) < 0.1f)
				fragColor = vec4(0,0,0,1);
			return;
		}
	}
	
	discard;
	

	

}            

)V0G0N";

		program2 = CreateProgram(vertexShader, fragmentShdader);

		positionLocation = glGetUniformLocation(program2, "offset");
		scaleLocation = glGetUniformLocation(program2, "scale");
		colourLocation = glGetUniformLocation(program2, "colour");
		transformationMatrixLocation = glGetUniformLocation(program2, "transform");
		

	}



	void Draw(const vec3& position, const vec2& size, const vec4& colour, unsigned int textureId) {
		glUseProgram(programUV);

		glUniform3fv(positionUVLocation, 1, &position[0]);
		glUniform2fv(scaleUVLocation, 1, &size[0]);
		glUniform4fv(colourUVLocation, 1, &colour[0]);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, textureId);


		ibo.Bind();
		vbo.Bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void Draw(const vec3& position, const vec2& size, const vec4& colour, const mat4& transform) {
		glUseProgram(program);

		glUniform3fv(positionLocation, 1, &position[0]);
		glUniform2fv(scaleLocation, 1, &size[0]);
		glUniform4fv(colourLocation, 1, &colour[0]);
		glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &transform[0][0]);




		ibo.Bind();
		vbo.Bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
};



	
void DrawParbola() {
	int width = 600, height = 600;
	std::vector<vec3> pixels = std::vector<vec3>(600 * 600);
	float w = 5;
	float h = 5;

	w = (float)width / w;
	h = (float)height / h;


	for (int _x = 0; _x < width; _x++)
	{
		for (int _y = 0; _y < height; _y++)
		{
			float x = _x/w;
			float y = _y/h;
			
			//y = x^2
			float delta = 2/h;

			float result = 1.5f;
			float f = sqrtf(powf(x,2) + y*y);

			result = y;
			f = x*x;
			
			float distance = abs(result - f);
			if(distance < delta)
				pixels[_x + width * _y][0] = 1;


		}
	}

	glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);
}


void DrawShape() {

	int width = 600, height = 600;
	std::vector<vec3> pixels = std::vector<vec3>(600 * 600);
	float w = 10;
	float h = 10;
	float offy = -5;
	float offx = -5;

	

	w = (float)width / w;
	h = (float)height / h;

	glm::mat4 rotation = glm::rotate(glm::radians(20.0f), vec3(0.0f, 0.0f, 1.0f));
	rotation = glm::rotate(rotation, glm::radians(-20.0f), vec3(0.0f, 1.0f, 0.0f));


	for (int _x = 0; _x < width; _x++)
	{
		for (int _y = 0; _y < height; _y++)
		{
			
			for (float z = 5; z < 10; z += 1.0f)
			{
				float x = _x / w + offx;
				float y = _y / h + offy;

				vec4 vec = vec4(x, y, z, 1);
				vec = vec*rotation;
				x = vec.x;
				y = vec.y;
				z = vec.z;

				float result = 1.5f;
				float f = sqrtf(powf(x*2, 2) + y*y + z*z);



				if (f < result)
					pixels[_x + width * _y][0] = 1;
			}

		}
	}

	glDrawPixels(width, height, GL_RGB, GL_FLOAT, &pixels[0]);
}


float posx;
float posy;
float posz;

float mosx, mosy, mosdx, mosdy;
void KeyCallBack(GLFWwindow* window, int a, int b, int c, int d) {
	if (a == GLFW_KEY_A)
		posx++;
	if (a == GLFW_KEY_D)
		posx--;

	if (a == GLFW_KEY_W)
		posy--;
	if (a == GLFW_KEY_S)
		posy++;

	if (a == GLFW_KEY_E)
		posz--;
	if (a == GLFW_KEY_R)
		posz++;

}

void CursorMoved(GLFWwindow* window, double x, double y) {

	mosdx = x - mosx;
	mosdy = y - mosy;

	mosx = x;
	mosy = y;

}

void main() {

	glfwInit();
	auto window = glfwCreateWindow(600, 600, "Plot Plus", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewInit();
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	Rect rect;
	
	glfwSetKeyCallback(window, KeyCallBack);
	
	glfwSetCursorPosCallback(window, CursorMoved);
	
	float delta = 0;
	DotPlot dotPlot;


	glm::mat4 rotation;
	while (true) {
	
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);
		//glm::mat4 rotation = glm::rotate(glm::radians(-20.0f), vec3(1.0f, 0.0f, 0.0f));
		//rotation = glm::rotate(rotation, glm::radians(delta), vec3(0.0f, 1.0f, 0.0f));

		//rotation = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		//rotation = glm::lookAt(vec3(posx, posz, posy), vec3(0, 0, 0), vec3(0, 1, 0));

		//rect.Draw(vec3(-1, -1, 0), { 2,2 }, { 1,0,0,1 }, rotation );
		//
		
		rotation = glm::perspective(glm::radians(30.0f), 1.0f, 0.1f, 1000.0f)  *  glm::lookAt(vec3(-posx, -posz, posy+10), vec3(0,0,0), vec3(0, 1, 0));
		//rotation =  rotation * glm::rotate(glm::radians(delta), vec3(0.0f, 1.0f, 0.0f));
		dotPlot.Draw({ 1,0,0,1 }, rotation);
		delta += 0.1f;	
		
	
	}


	std::cin.get();

}
*/