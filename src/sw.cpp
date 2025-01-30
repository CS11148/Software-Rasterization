#include "sw.hpp"

#include <iostream>
#include <vector>

namespace COL781 {
	namespace Software {

		// Forward declarations

		template <> float Attribs::get(int index) const;
		template <> glm::vec2 Attribs::get(int index) const;
		template <> glm::vec3 Attribs::get(int index) const;
		template <> glm::vec4 Attribs::get(int index) const;

		template <> void Attribs::set(int index, float value);
		template <> void Attribs::set(int index, glm::vec2 value);
		template <> void Attribs::set(int index, glm::vec3 value);
		template <> void Attribs::set(int index, glm::vec4 value);

		// Built-in shaders

		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
				return vertex;
			};
		}

		FragmentShader Rasterizer::fsConstant() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = uniforms.get<glm::vec4>("color");
				return color;
			};
		}

		FragmentShader Rasterizer::fsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = in.get<glm::vec4>(0);
				return color;
			};
		}

		// Implementation of Attribs and Uniforms classes

		void checkDimension(int index, int actual, int requested) {
			if (actual != requested) {
				std::cout << "Warning: attribute " << index << " has dimension " << actual << " but accessed as dimension " << requested << std::endl;
			}
		}

		template <> float Attribs::get(int index) const {
			checkDimension(index, dims[index], 1);
			return values[index].x;
		}

		template <> glm::vec2 Attribs::get(int index) const {
			checkDimension(index, dims[index], 2);
			return glm::vec2(values[index].x, values[index].y);
		}

		template <> glm::vec3 Attribs::get(int index) const {
			checkDimension(index, dims[index], 3);
			return glm::vec3(values[index].x, values[index].y, values[index].z);
		}

		template <> glm::vec4 Attribs::get(int index) const {
			checkDimension(index, dims[index], 4);
			return values[index];
		}

		void expand(std::vector<int> &dims, std::vector<glm::vec4> &values, int index) {
			if (dims.size() < index+1)
				dims.resize(index+1);
			if (values.size() < index+1)
				values.resize(index+1);
		}

		template <> void Attribs::set(int index, float value) {
			expand(dims, values, index);
			dims[index] = 1;
			values[index].x = value;
		}

		template <> void Attribs::set(int index, glm::vec2 value) {
			expand(dims, values, index);
			dims[index] = 2;
			values[index].x = value.x;
			values[index].y = value.y;
		}

		template <> void Attribs::set(int index, glm::vec3 value) {
			expand(dims, values, index);
			dims[index] = 3;
			values[index].x = value.x;
			values[index].y = value.y;
			values[index].z = value.z;
		}

		template <> void Attribs::set(int index, glm::vec4 value) {
			expand(dims, values, index);
			dims[index] = 4;
			values[index] = value;
		}

		template <typename T> T Uniforms::get(const std::string &name) const {
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {
			auto it = values.find(name);
			if (it != values.end()) {
				delete it->second;
			}
			values[name] = (void*)(new T(value));
		}











		bool Rasterizer::initialize(const std::string &title, int width, int height, int spp)
		{
			bool success = true;
    		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        		printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        		success = false;
    		} 
			else {
        		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        		if (window == NULL) {
            		printf("Window could not be created! SDL_Error: %s", SDL_GetError());
            	success = false;
        		} 		
				else {
            		windowSurface = SDL_GetWindowSurface(window);
            		framebuffer = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        		}
    		}
    		return success;
		}


		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs)
		{
			ShaderProgram a;

			a.vs=vs;
			a.fs=fs;

			return a;
		}

		Object Rasterizer::createObject()
		{
			Object a;

			return a;
		}

		void Rasterizer::clear(glm::vec4 color)
		{
			 Uint32 color32 = SDL_MapRGBA(framebuffer->format,
        	static_cast<Uint8>(color.r * 255),
        	static_cast<Uint8>(color.g * 255),
        	static_cast<Uint8>(color.b * 255),
        	static_cast<Uint8>(color.a * 255));
    		SDL_FillRect(framebuffer, NULL, color32);
    		SDL_BlitSurface(framebuffer, NULL, windowSurface, NULL);
    		SDL_UpdateWindowSurface(window); 

		}

		bool Rasterizer::shouldQuit()
		{
			SDL_Event e;

    		while (SDL_PollEvent(&e)) 
			{
        		if (e.type == SDL_QUIT) {
            		return true;
        		}
			}

			return false; 

		}

		void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, int d, const float* data)
		{

			if (object.vertexAttributes.size() < n) {
				object.vertexAttributes.resize(n);
			}

			// Iterate over each vertex
			for (int i = 0; i < n; ++i) {
				Attribs& attrib = object.vertexAttributes[i]; // Get the current vertex's attributes

				// Now set the attribute for the current vertex
				if (d == 1) {
					// If the dimension is 1 (float)
					attrib.set(attribIndex, data[i * d]);
				}
				else if (d == 2) {
					// If the dimension is 2 (glm::vec2)
					attrib.set(attribIndex, glm::vec2(data[i * d], data[i * d + 1]));
				}
				else if (d == 3) {
					// If the dimension is 3 (glm::vec3)
					attrib.set(attribIndex, glm::vec3(data[i * d], data[i * d + 1], data[i * d + 2]));
				}
				else if (d == 4) {
					// If the dimension is 4 (glm::vec4)
					attrib.set(attribIndex, glm::vec4(data[i * d], data[i * d + 1], data[i * d + 2], data[i * d + 3]));
				}
				else {
					// Handle error case (invalid dimension)
					std::cerr << "Unsupported dimension: " << d << std::endl;
				}
			}
			
		}


		void Rasterizer::setTriangleIndices(Object &object, int n, int* triangles)
		{
			if(object.indices.size()<n)
			{
				object.indices.resize(n);
			}

			for(int i=0; i<n; ++i)
			{
				glm::ivec3& index = object.indices[i];

				object.indices[i][0] = triangles[i*3];
				object.indices[i][1] = triangles[i*3+1];
				object.indices[i][2] = triangles[i*3+2]; 
			}

		}

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec4 value)
		{
			program.uniforms.set(name,value);
		}


		void Rasterizer::useShaderProgram(const ShaderProgram &program)
		{
			*current_shaderprogram=program;
		}


		



	}
}
