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


using glm::vec2;

#define IX(i,j) ((i)+(N+2)*(j))
/*
void project(int N, glm::vec4 * v)
{
	int i, j, k;
	float h;
	h = 1.0 / N;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			
			div[IX(i, j)] = -0.5*h*(v[IX(i + 1, j)] - v[IX(i - 1, j)] +
				v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0;
		}
	}
	set_bnd(N, 0, div); set_bnd(N, 0, p);
	for (k = 0; k<20; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
					p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
			}
		}
		set_bnd(N, 0, p);
	}
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			u[IX(i, j)] -= 0.5*(p[IX(i + 1, j)] - p[IX(i - 1, j)]) / h;
			v[IX(i, j)] -= 0.5*(p[IX(i, j + 1)] - p[IX(i, j - 1)]) / h;
		}
	}
	set_bnd(N, 1, u); set_bnd(N, 2, v);
}
*/


void main() {
	
	vec2 xy;

	glm::vec2 dir = {1,0};
	float angle1 = acos(glm::dot(glm::normalize(vec2(-1, 0)), glm::normalize(dir)));
	float angle2 = acos(glm::dot(glm::normalize(vec2(-1, 1)), glm::normalize(dir)));
	float angle3 = acos(glm::dot(glm::normalize(vec2(0, 1)), glm::normalize(dir)));
	float angle4 = acos(glm::dot(glm::normalize(vec2(-1,-1)), glm::normalize(dir)));
	float angle5 = acos(glm::dot(glm::normalize(vec2(0, -1)), glm::normalize(dir)));
	float angle6 = acos(glm::dot(glm::normalize(vec2(1, 1)), glm::normalize(dir)));
	float angle7 = acos(glm::dot(glm::normalize(vec2(1, 0)), glm::normalize(dir)));
	float angle8 = acos(glm::dot(glm::normalize(vec2(1, -1)), glm::normalize(dir)));


	float sum = angle1 + angle4 + angle5;

	


	

	glfwInit();
	auto window = glfwCreateWindow(600, 600, "Plot Plus", NULL, NULL);
	glfwMakeContextCurrent(window);


	glewInit();
	glClearColor(0.5f, 0.5f, 0.5f, 1);

	string vertexShader = R"V0G0N(
#version 330 core

layout(location = 0) in vec4 vPosition;
out vec2 uv;

void main() {
    gl_Position = vPosition;
	uv = vec2(vPosition + vec4(1))/2.0f;
}            

)V0G0N";


	string fragmentShdader =
		R"V0G0N(
#version 330 core

in vec2 uv;
out vec4 fragColor;
uniform sampler2D tex;


void main(){
	fragColor = texture(tex, uv).wwww;
}            

)V0G0N";

	unsigned int text, updatedText;
	glGenTextures(1, &text);
	glGenTextures(1, &updatedText);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text);
	float data[200 * 200 * 4];
	size_t i = 0;
	
		for (size_t y = 0; y < 200; y++)
		{
			for (size_t x = 0; x < 200; x++)
			{
			data[i    ] = (100.0f-x) / 100.0f;
			data[i + 1] = (100.0f-y) / 100.0f;
			data[i + 2] = 0.0f;
			//data[i + 3] = glm::distance(vec2(data[i], data[i + 1]), vec2(0, 0));

			//data[i] = ((rand() % 200) - 100.0f) / 100.0f;
			//data[i + 1] = ((rand() % 200) - 100.0f) / 100.0f;
			//data[i + 2] = 0;
			//[y - x, -x - y]

			float xx = (100.0f - x) / 100.0f;
			float yy = (100.0f - y) / 100.0f;
			data[i] = yy - xx;
			data[i + 1] = -xx - yy;
			data[i] = 0.0f;
			data[i + 1] = 0.0f;
			data[i + 3] = 0.0f;
			if (x > 60 && x < 120 && y > 60 && y < 120) {
				data[i] = 0.5f;
				data[i + 1] = 0.5f;
				data[i + 3] = (rand() % 100) / 100.0f;
			}

			if (x > 20 && x < 180 && y > 135 && y < 175) {
				data[i] = -0.8f;
				data[i + 1] = -0.1f;
				data[i + 3] = (rand() % 100) / 100.0f;
			}
	
			
			//data[i + 3] = 0.2f;
		

			//data[i] = 1;
			//data[i + 1] = 0.01f;

			i+=4;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 200, 200, 0, GL_RGBA, GL_FLOAT, data);
	glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	
	

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, updatedText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 200, 200, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned int myProgram = CreateProgram(vertexShader, fragmentShdader);
	
	glUniform1i(glGetUniformLocation(myProgram, "tex"), 1);


	glm::vec3 vert [4] = { { -1, -1, 0.0f },{ -1, 1, 0.0f },{ 1, 1, 0.0f },{ 1, -1, 0.0f } };
	unsigned int indices[6] = { 0,1,2, 3,0,2 };
	Buffer<GL_ARRAY_BUFFER> vbo(vert, sizeof(glm::vec3) * 4, GL_STATIC_DRAW);
	Buffer<GL_ELEMENT_ARRAY_BUFFER> ibo(indices, sizeof(unsigned int) * 6, GL_STATIC_DRAW);


	


	
	


	string csSrc = R"V0G0N(
		#version 430

		layout(rgba32f, binding = 0) uniform image2D vel;
		layout(rgba32f, binding = 1) uniform image2D updatedVel;
		
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

		float step = 0.03f;

		ivec2 storePos;
		vec4 v;
		vec4 startv;

		vec4 tex2DBiLinear(vec2 texCoord_i ){
			float fWidth = 1.0f;
			float fHeight = 1.0f;

			return imageLoad(vel, ivec2(texCoord_i));
			
			vec4 p0q0 = imageLoad(vel, ivec2(texCoord_i));
			vec4 p1q0 = imageLoad(vel, ivec2(texCoord_i + vec2(1, 0)));
			vec4 p0q1 = imageLoad(vel, ivec2(texCoord_i + vec2(0, 1)));
			vec4 p1q1 = imageLoad(vel, ivec2(texCoord_i + vec2(1 , 1)));

			float a = fract(texCoord_i.x * fWidth ); // Get Interpolation factor for X direction.
							// Fraction near to valid data.

			vec4 pInterp_q0 = mix( p0q0, p1q0, a ); // Interpolates top row in X direction.
			vec4 pInterp_q1 = mix( p0q1, p1q1, a ); // Interpolates bottom row in X direction.

			float b = fract( texCoord_i.y * fHeight );// Get Interpolation factor for Y direction.
			return mix( pInterp_q0, pInterp_q1, b ); // Interpolate in Y direction.
		}

		void Advect(){
			vec2 pos = ivec2(storePos);
			
			vec2 backInTimePos = pos - v.xy * 0.01f;
			if(backInTimePos.x < 0)
				backInTimePos.x = 200 + backInTimePos.x;
			if(backInTimePos.y < 0)
				backInTimePos.y = 200 + backInTimePos.y;
			if(backInTimePos.x > 199)
				backInTimePos.x = 200 - backInTimePos.x;
			if(backInTimePos.y > 199)
				backInTimePos.y = 200 - backInTimePos.y;

			
			v = tex2DBiLinear(backInTimePos);
			
		}
		
		float Divergence(){               
			vec4 wL = imageLoad(v, storePos - ivec2(1, 0));
			vec4 wR = imageLoad(v, storePos + ivec2(1, 0));
			vec4 wB = imageLoad(v, storePos - ivec2(0, 1));
			vec4 wT = imageLoad(v, storePos + ivec2(0, 1));

			return 0.5f * ((wR.x - wL.x) + (wT.y - wB.y));;
		}

		vec4 Gradient(){		
			half pL = h1texRECT(p, coords - half2(1, 0));
			half pR = h1texRECT(p, coords + half2(1, 0));
			half pB = h1texRECT(p, coords - half2(0, 1));
			half pT = h1texRECT(p, coords + half2(0, 1));
			
			v.xy -= 0.5f * half2(pR - pL, pT - pB);
			return vNew;
	
		}	
		void Project(){
			v.z = Divergence();
			
			Gradient();
		}
		
		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
			startv = v = imageLoad(vel, storePos);

			
			Advect();
						
			v.xy = startv.xy;
			imageStore(updatedVel,storePos,v);
		
         }

	)V0G0N";

	

	unsigned int computeShader = CreateProgram(csSrc);
	
	glUniform1i(glGetUniformLocation(computeShader, "vel"), 0);
	glUniform1i(glGetUniformLocation(computeShader, "updatedVel"), 1);

	std::string src = R"V0G0N(
		#version 430

		layout(rgba32f, binding = 0) uniform image2D vel;
		layout(rgba32f, binding = 1) uniform image2D updatedVel;
		
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
			
		ivec2 storePos;


		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
			
			float xL = imageLoad(vel, storePos - ivec2(1, 0)).w;
			float xR = imageLoad(vel, storePos + ivec2(1, 0)).w;
			float xB = imageLoad(vel, storePos - ivec2(0, 1)).w;
			float xT = imageLoad(vel, storePos + ivec2(0, 1)).w;
			float div = imageLoad(vel, storePos).z;			

			// evaluate Jacobi iteration
			vec4 newv = (xL + xR + xB + xT + div)/4.0f;
			imageStore(updatedVel, storePos, newv;
         }
	)V0G0N";

	unsigned int jacobiProgram = CreateProgram(src);
	
	glUniform1i(glGetUniformLocation(computeShader, "vel"), 0);
	glUniform1i(glGetUniformLocation(computeShader, "updatedVel"), 1);

	/*
	src = R"V0G0N(
		#version 430

		layout(rgba32f, binding = 0) uniform image2D vel;
		layout(rgba32f, binding = 1) uniform image2D updatedVel;
		
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
			
		ivec2 storePos;
		

		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
			
			vec4 xL = imageLoad(vel, storePos - ivec2(1, 0)).w;
			vec4 xR = imageLoad(vel, storePos + ivec2(1, 0)).w;
			vec4 xB = imageLoad(vel, storePos - ivec2(0, 1)).w;
			vec4 xT = imageLoad(vel, storePos + ivec2(0, 1)).w;
			
			// evaluate Jacobi iteration
			vec4 newv = (xL + xR + xB + xT)/4.0f;
			imageStore(updatedVel, storePos, newv;
         }
	)V0G0N";

	unsigned int jacobiProgram = CreateProgram(src);

	glUniform1i(glGetUniformLocation(computeShader, "vel"), 0);
	glUniform1i(glGetUniformLocation(computeShader, "updatedVel"), 1);

	*/
	cout << glGetError() << std::endl;


	while (true) {
		auto start = clock();
		
		
		
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);



		glUseProgram(computeShader);
		glDispatchCompute(200, 200, 1);

		

		
		glUseProgram(myProgram);
		unsigned int temp = updatedText;
		updatedText = text;
		text = temp;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, text);
		glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, updatedText);
		glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


		vbo.Bind();
		ibo.Bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//Sleep(10);
		//std::cin.get();

	
		
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
		float sum = 0;
		for (size_t i = 0; i < 200*200*4; i+=4)
		{
			if (data[i + 2] != 2 || data[i+2]!=6) {
				float d = data[i + 2];
				int l = 0;
			}
			sum += data[i+3];
		}
		cout << sum << std::endl;

		auto end = clock();
		//cout << (end-start) / 1000.0f << "\r";
		
	}
	
	

}
