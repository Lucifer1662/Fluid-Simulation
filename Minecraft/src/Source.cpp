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


void main() {
		

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
	fragColor = vec4((texture(tex, uv).xy + vec2(1)) /2,0,1);
	fragColor = (texture(tex, uv).zzzz);
}            

)V0G0N";

	unsigned int text, updatedText;
	glGenTextures(1, &text);
	glGenTextures(1, &updatedText);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text);
	float data[200 * 200 * 4];
	size_t i = 0;
	
		for (int y = -100; y < 100; y++)
		{
			for (int x = -100; x < 100; x++)
			{
				data[i] = (x-y^2 + 2) / 10000.0f;// +((rand() % 200) / 100.0f - 1.0f)*0.1f;
			//((rand() % 200) / 100.0f - 1.0f)*10.0f;
				data[i + 1] = (x*y + y) / 10000.0f;// +((rand() % 200) / 100.0f - 1.0f)*0.1f;
			//((rand() % 200) / 100.0f - 1.0f)*10.0f;
			data[i + 2] = 0.1f;// (rand() % 100) / 100.0f;
			if (x > 50 && x < 100 && y>50 && y < 100) {
			//	data[i] = 0.3f;
			//	data[i + 1] = 3.0f;
				data[i + 2] = 1.0f;// (rand() % 100) / 100.0f;
			}
			
		
			if (x > -80 && x < 20 && y> -30 && y < 10) {
				//data[i] = -0.4f;
				//data[i + 1] = -1.0f;
				data[i + 2] = 1.0f;// (rand() % 100) / 100.0f;
			}
	

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

		ivec2 getPos(int x, int y){
			ivec2 result = storePos + ivec2(x, y);
			if(result.x < 0)
				result.x = 199;
			if(result.x > 199)
				result.x = 0; 			
			if(result.y < 0)
				result.y = 199;
			if(result.y > 199)
				result.y = 0; 	
			return result;
			
		}

		vec4 tex2DBiLinear(vec2 texCoord_i){
			float fWidth = 1.0f;
			float fHeight = 1.0f;

			//return imageLoad(vel, ivec2(texCoord_i));
			//texCoord_i -= vec2(0.5f);
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
			
			vec2 backInTimePos = pos - v.xy;
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
	
		
		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
			startv = v = imageLoad(vel, storePos);

			
			Advect();
						
			//v.xy = startv.xy;
			imageStore(updatedVel,storePos,v);
		
         }

	)V0G0N";

	

	unsigned int advectShader = CreateProgram(csSrc);
	
	glUniform1i(glGetUniformLocation(advectShader, "vel"), 0);
	glUniform1i(glGetUniformLocation(advectShader, "updatedVel"), 1);

	std::string src = R"V0G0N(
		#version 430

		layout(rgba32f, binding = 0) uniform image2D vel;
		layout(rgba32f, binding = 1) uniform image2D updatedVel;
		
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
			
		ivec2 storePos;
		
		ivec2 getPos(int x, int y){
			ivec2 result = storePos + ivec2(x, y);
			if(result.x < 0)
				result.x = 199;
			if(result.x > 199)
				result.x = 0; 			
			if(result.y < 0)
				result.y = 199;
			if(result.y > 199)
				result.y = 0; 	
			return result;
			
		}

		float Divergence(){               
			vec4 wL = imageLoad(vel, getPos(-1, 0));
			vec4 wR = imageLoad(vel, getPos(1, 0));
			vec4 wB = imageLoad(vel, getPos(0, -1));
			vec4 wT = imageLoad(vel, getPos(0, 1));

			return 0.5f * ((wR.x - wL.x) + (wT.y - wB.y));
		}

		float Pressure(){     

		//p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
		//			p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;

			
          
			float pL = imageLoad(vel, getPos(-1, 0)).w;
			float pR = imageLoad(vel, getPos(1, 0)).w;
			float pB = imageLoad(vel, getPos(0, -1)).w;
			float pT = imageLoad(vel, getPos(0, 1)).w;		
			return (Divergence() + pL + pR + pB + pT)/4;	
		}
	
		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
			vec3 v = imageLoad(vel, storePos).xyz;			
			imageStore(updatedVel, storePos, vec4(v,Pressure()));
         }
	)V0G0N";

	unsigned int pressureProgram = CreateProgram(src);
	
	glUniform1i(glGetUniformLocation(pressureProgram, "vel"), 0);
	glUniform1i(glGetUniformLocation(pressureProgram, "updatedVel"), 1);


	src = R"V0G0N(
		#version 430

		layout(rgba32f, binding = 0) uniform image2D vel;
		layout(rgba32f, binding = 1) uniform image2D updatedVel;
		
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
			
		ivec2 storePos;
		
		ivec2 getPos(int x, int y){
			ivec2 result = storePos + ivec2(x, y);
			if(result.x < 0)
				result.x = 199;
			if(result.x > 199)
				result.x = 0; 			
			if(result.y < 0)
				result.y = 199;
			if(result.y > 199)
				result.y = 0; 	
			return result;
			
		}

		vec3 NoDiv(){               
			float pL = imageLoad(vel, getPos(-1, 0)).w;
			float pR = imageLoad(vel, getPos(1, 0)).w;
			float pB = imageLoad(vel, getPos(0, -1)).w;
			float pT = imageLoad(vel, getPos(0, 1)).w;
			vec3 v = imageLoad(vel,storePos).xyz;

			v.x += 0.5*(pR - pL)/10;
			v.y += 0.5*(pT - pB)/10;
		
			return v;
		}
	
		void main() {
            storePos = ivec2(gl_GlobalInvocationID.xy);
		
			imageStore(updatedVel, storePos, vec4(NoDiv(),0));
         }
	)V0G0N";

	unsigned int jacobiProgram = CreateProgram(src);

	glUniform1i(glGetUniformLocation(jacobiProgram, "vel"), 0);
	glUniform1i(glGetUniformLocation(jacobiProgram, "updatedVel"), 1);

	cout << glGetError() << std::endl;
	unsigned int temp;

	for (size_t j = 0; j < 100; j++)
	{

		for (size_t i = 0; i < 20; i++)
		{
			glUseProgram(pressureProgram);
			glDispatchCompute(200, 200, 1);

			temp = updatedText;
			updatedText = text;
			text = temp;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, text);
			glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, updatedText);
			glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		}



		glUseProgram(jacobiProgram);
		glDispatchCompute(200, 200, 1);

		temp = updatedText;
		updatedText = text;
		text = temp;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, text);
		glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, updatedText);
		glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}


	while (true) {
		auto start = clock();
		
		
		
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);


		for (size_t i = 0; i < 20; i++)
		{
			glUseProgram(pressureProgram);
			glDispatchCompute(200, 200, 1);

			temp = updatedText;
			updatedText = text;
			text = temp;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, text);
			glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, updatedText);
			glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		}



		glUseProgram(jacobiProgram);
		glDispatchCompute(200, 200, 1);

		temp = updatedText;
		updatedText = text;
		text = temp;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, text);
		glBindImageTexture(0, text, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, updatedText);
		glBindImageTexture(1, updatedText, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);



		
		glUseProgram(advectShader);
		glDispatchCompute(200, 200, 1);
		
		
		glUseProgram(myProgram);
		temp = updatedText;
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
		
		//std::cin.get();

	

		/*
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
		float sum = 0;
		for (size_t i = 0; i < 200 * 200 * 4; i += 4)
		{
			sum += data[i + 2];
		}
		cout << sum << std::endl;
		*/
		

		auto end = clock();
		//cout << (end-start) / 1000.0f << "\r";
		
	}
	
	

}
