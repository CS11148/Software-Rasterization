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

		VertexShader Rasterizer::vsColorTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				out.set<glm::vec4>(0, color);
				return transform*vertex;
			};
		}

		VertexShader Rasterizer::vsNormalTransform(){
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 normal = in.get<glm::vec4>(1);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				glm::mat4 wsTransform = uniforms.get<glm::mat4>("wsTransform");
				out.set<glm::vec4>(0,normal*wsTransform);
				return transform*vertex;
				
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

		FragmentShader Rasterizer::fsDiffuseLighting(){
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec3 normal = in.get<glm::vec4>(0);

				glm::vec3 object_color = uniforms.get<glm::vec3>("objectColor");
				glm::vec3 ambient_color = uniforms.get<glm::vec3>("ambientColor");

				glm::vec3 ambient = object_color*ambient_color;


				glm::vec3 light_color = uniforms.get<glm::vec3>("lightColor");
				glm::vec3 light_direction = uniforms.get<glm::vec3>("lightDir");

				float diffuse_intensity = std::max(0.0f,dot(normal,light_direction));

				glm::vec3 diffuse_light = diffuse_intensity*light_color*object_color;

				glm::vec3 final_color = ambient+diffuse_light;

				glm::vec4 color(final_color.x,final_color.y,final_color.z,1.0f);

				return color;
			};
		}

		FragmentShader Rasterizer::fsSpecularLighting(){
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec3 normal = glm::normalize(in.get<glm::vec4>(0));  // Make sure the normal is normalized

				// Object and ambient colors
				glm::vec3 object_color = uniforms.get<glm::vec3>("objectColor");
				glm::vec3 ambient_color = uniforms.get<glm::vec3>("ambientColor");
				
				// Ambient lighting contribution
				glm::vec3 ambient = object_color * ambient_color;

				// Light parameters
				glm::vec3 light_color = uniforms.get<glm::vec3>("lightColor");
				glm::vec3 light_direction = glm::normalize(uniforms.get<glm::vec3>("lightDir"));  // Make sure the light direction is normalized

				// Calculate the diffuse lighting
				float diffuse_intensity = std::max(0.0f, glm::dot(normal, light_direction));
				glm::vec3 diffuse_light = diffuse_intensity * light_color * object_color;

				// View parameters
				glm::vec3 view_position = uniforms.get<glm::vec3>("viewPos");
				glm::vec3 frag_position = in.get<glm::vec3>(0);  // Assuming position is stored in the 2nd attribute slot
				glm::vec3 view_direction = glm::normalize(view_position - frag_position);

				// Reflection direction (R = 2 * (N · L) * N - L)
				glm::vec3 reflect_dir = glm::normalize(glm::reflect(-light_direction, normal));

				// Calculate the specular component
				float specular_strength = std::max(0.0f, glm::dot(view_direction, reflect_dir));
				float shininess = uniforms.get<float>("blinnpow");  // Shininess factor (Blinn exponent)
				float specular_intensity = std::pow(specular_strength, shininess);

				// Specular lighting contribution
				glm::vec3 specular_light = specular_intensity * light_color;  // Specular color is typically the light color

				// Final color: Ambient + Diffuse + Specular
				glm::vec3 final_color = ambient + diffuse_light + specular_light;

				// Convert final color to a 4D vector and return
				glm::vec4 color(final_color.x, final_color.y, final_color.z, 1.0f);

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
			a.uniforms=Uniforms();

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
    		
			if(depth_enabled == true)
			{
			for(int i = 0; i < framebuffer->w; ++i) {
				for(int j = 0; j < framebuffer->h; ++j) {
					z_buffer[i * framebuffer->h + j] = 100.0f;
				}
			}

			}

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

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec3 value)
		{
			program.uniforms.set(name,value);
		}

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec4 value)
		{
			program.uniforms.set(name,value);
		}

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat4 value)
		{
			program.uniforms.set(name,value);
		}

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, float value)
		{
			program.uniforms.set(name,value);
		}

		template<> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, int value)
		{
			program.uniforms.set(name,value);
		}


		void Rasterizer::useShaderProgram(const ShaderProgram &program)
		{
			current_shaderprogram=&program;
		}

		void Rasterizer::drawObject(const Object& tickmark)
		{

			for(int i=0; i<tickmark.indices.size(); ++i)
			{

				glm::ivec3 triangle = tickmark.indices[i];

				Attribs p1_att = tickmark.vertexAttributes[triangle[0]];

				Attribs p2_att = tickmark.vertexAttributes[triangle[1]];

				Attribs p3_att = tickmark.vertexAttributes[triangle[2]];

				draw_triangle(p1_att,p2_att,p3_att, framebuffer);

			}

		}

		void Rasterizer::show()
		{
			SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
            SDL_UpdateWindowSurface(window);
		}

		void Rasterizer::deleteShaderProgram(ShaderProgram &Program)
		{
			current_shaderprogram=nullptr;
		}

		void Rasterizer::enableDepthTest()
		{
			depth_enabled = true;

	
			z_buffer = new float[framebuffer->w * framebuffer->h];

			
			for(int i = 0; i < framebuffer->w; ++i) {
				for(int j = 0; j < framebuffer->h; ++j) {
					z_buffer[i * framebuffer->h + j] = 100.0f;
				}
			}

		}








		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool Rasterizer::is_in_triangle(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 point)
		{
			// Remove the extra homogeneous coordinate (the fourth component)
			glm::vec3 v0 = glm::vec3(p1);
			glm::vec3 v1 = glm::vec3(p2);
			glm::vec3 v2 = glm::vec3(p3);
			glm::vec3 p = glm::vec3(point);
			
			// Compute vectors
			glm::vec3 v0v1 = v1 - v0;
			glm::vec3 v0v2 = v2 - v0;
			glm::vec3 v0p = p - v0;
			
			// Compute dot products
			float dot00 = glm::dot(v0v2, v0v2);
			float dot01 = glm::dot(v0v2, v0v1);
			float dot02 = glm::dot(v0v2, v0p);
			float dot11 = glm::dot(v0v1, v0v1);
			float dot12 = glm::dot(v0v1, v0p);
			
			// Compute barycentric coordinates
			float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
			
			// Check if point is in triangle
			return (u >= 0) && (v >= 0) && (u + v < 1);
		}


		std::vector<float> Rasterizer::Barycentric_Coordinates(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 point)
		{
			glm::vec3 v0 = glm::vec3(p1);
			glm::vec3 v1 = glm::vec3(p2);
			glm::vec3 v2 = glm::vec3(p3);
			glm::vec3 p = glm::vec3(point);
			
			// Compute vectors
			glm::vec3 v0v1 = v1 - v0;
			glm::vec3 v0v2 = v2 - v0;
			glm::vec3 v0p = p - v0;
			
			// Compute dot products
			float dot00 = glm::dot(v0v2, v0v2);
			float dot01 = glm::dot(v0v2, v0v1);
			float dot02 = glm::dot(v0v2, v0p);
			float dot11 = glm::dot(v0v1, v0v1);
			float dot12 = glm::dot(v0v1, v0p);
			
			// Compute barycentric coordinates
			float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			std::vector<float> coordinates = {1-u-v,v,u};

			return coordinates;

		}

		
		glm::vec2 Rasterizer::convert_to_screen_coordinates(glm::vec4 point, int screen_width, int screen_height,bool depth_enabled) 
		{

			point.x=(point.x)/(point.w);
			point.y=(point.y)/(point.w);
			point.z=(point.z)/(point.w);

			if(depth_enabled==false)
			{
				float x = (point.x + 1.0f) * 0.5f * screen_width;
				float y = (1.0f - point.y) * 0.5f * screen_height;
				return glm::vec2(x, y);
			}

			else 
    		{
				// Define camera position at z = -1
				glm::vec3 camera_pos(0.0f, 0.0f, -100.0f);

				// Define screen plane at z = 0
				float screen_z = -1.0f;

				// Calculate the direction from the camera to the point
				glm::vec3 direction(point.x, point.y, point.z - camera_pos.z);

				// Find the intersection of the ray with the screen plane
				float t = (screen_z - camera_pos.z) / direction.z;
				glm::vec3 intersection = camera_pos + t * direction;

				// Convert the intersection point to screen coordinates
				float x = (intersection.x + 1.0f) * 0.5f * screen_width;
				float y = (1.0f - intersection.y) * 0.5f * screen_height;

				return glm::vec2(x, y);
    		}
		}

		glm::vec4 Rasterizer::perspective_coordinates(glm::vec4 point, int screen_width, int screen_height, bool depth_enabled)
		{

			point.x=(point.x)/(point.w);
			point.y=(point.y)/(point.w);
			point.z=(point.z)/(point.w);

			if(depth_enabled==false)
			{
				float x = point.x; 
				float y = point.y;
				return glm::vec4(x, y,0,0);
			}

			else 
    		{
				// Define camera position at z = -1
				glm::vec3 camera_pos(0.0f, 0.0f, -100.0f);

				// Define screen plane at z = 0
				float screen_z = -1.0f;

				// Calculate the direction from the camera to the point
				glm::vec3 direction(point.x, point.y, point.z - camera_pos.z);

				// Find the intersection of the ray with the screen plane
				float t = (screen_z - camera_pos.z) / direction.z;
				glm::vec3 intersection = camera_pos + t * direction;

				// Convert the intersection point to screen coordinates
				float x = intersection.x; 
				float y = intersection.y;

				return glm::vec4(x, y,0,0);
    		}
		}

		glm::vec4 Rasterizer::convert_to_rgb_colors(glm::vec4 colors)
		{
			Uint8 r = static_cast<Uint8>(colors.x*255);
			Uint8 g = static_cast<Uint8>(colors.y*255);
			Uint8 b = static_cast<Uint8>(colors.z*255);

			glm::vec4 rgb_colors(r,g,b,255);

			return rgb_colors;
			
		}

	
		void Rasterizer::draw_pixel(SDL_Surface* surface, int x, int y, Uint32 color) 
		{
			if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
				Uint32* pixels = (Uint32*)surface->pixels;
				pixels[(y * surface->w) + x] = color;
			}
		}

		void Rasterizer::draw_triangle(Attribs p1_att, Attribs p2_att, Attribs p3_att, SDL_Surface* framebuffer) 
		{

			glm::vec4 p1 = p1_att.get<glm::vec4>(0);
			glm::vec4 p2 = p2_att.get<glm::vec4>(0);
			glm::vec4 p3 = p3_att.get<glm::vec4>(0);

			std::cout<<"p1 "<<p1.x<<" "<<p1.y<<" "<<p1.z<<std::endl;
			std::cout<<"p2 "<<p2.x<<" "<<p2.y<<" "<<p2.z<<std::endl;
			std::cout<<"p3 "<<p3.x<<" "<<p3.y<<" "<<p3.z<<std::endl;


			int screen_width = framebuffer->w;
			int screen_height = framebuffer->h;

			VertexShader vs = current_shaderprogram->vs;
			FragmentShader fs = current_shaderprogram->fs;
			Uniforms uniforms = current_shaderprogram->uniforms;

			Attribs out1 = Attribs();
			Attribs out2 = Attribs();
			Attribs out3 = Attribs();

			p1 = vs(uniforms,p1_att,out1);
			p2 = vs(uniforms,p2_att,out2);
			p3 = vs(uniforms,p3_att,out3);

			// std::cout<<"p1 "<<p1.x<<" "<<p1.y<<" "<<p1.z<<" "<<p1.w<<std::endl;
			// std::cout<<"p2 "<<p2.x<<" "<<p2.y<<" "<<p2.z<<" "<<p2.w<<std::endl;
			// std::cout<<"p3 "<<p3.x<<" "<<p3.y<<" "<<p3.z<<" "<<p3.w<<std::endl;

			glm::vec4 color_vertex_1 = fs(uniforms,out1);
			glm::vec4 color_vertex_2 = fs(uniforms,out2);
			glm::vec4 color_vertex_3 = fs(uniforms,out3);

			// Convert 3D coordinates to 2D screen coordinates
			glm::vec2 p1_screen = convert_to_screen_coordinates(p1, screen_width, screen_height,depth_enabled);
			glm::vec2 p2_screen = convert_to_screen_coordinates(p2, screen_width, screen_height,depth_enabled);
			glm::vec2 p3_screen = convert_to_screen_coordinates(p3, screen_width, screen_height,depth_enabled);

			glm::vec4 p1_perspective = perspective_coordinates(p1,screen_width,screen_height,depth_enabled);
			glm::vec4 p2_perspective = perspective_coordinates(p2,screen_width,screen_height,depth_enabled);
			glm::vec4 p3_perspective = perspective_coordinates(p3,screen_width,screen_height,depth_enabled);


			// std::cout<<"p1_screen "<<p1_screen.x<<" "<<p1_screen.y<<std::endl;
			// std::cout<<"p2 screen "<<p2_screen.x<<" "<<p2_screen.y<<std::endl;
			// std::cout<<"p3 screen "<<p3_screen.x<<" "<<p3_screen.y<<std::endl;

			// std::cout<<"p1_perspective "<<p1_perspective.x<<" "<<p1_perspective.y<<" "<<p1_perspective.z<<" "<<p1_perspective.w<<std::endl;
			// std::cout<<"p2_perspective "<<p2_perspective.x<<" "<<p2_perspective.y<<" "<<p2_perspective.z<<" "<<p2_perspective.w<<std::endl;
			// std::cout<<"p3_perspective "<<p3_perspective.x<<" "<<p3_perspective.y<<" "<<p3_perspective.z<<" "<<p3_perspective.w<<std::endl;

			int min_x = min(static_cast<int>(p1_screen.x), static_cast<int>(p2_screen.x), static_cast<int>(p3_screen.x));
			int max_x = max(static_cast<int>(p1_screen.x), static_cast<int>(p2_screen.x), static_cast<int>(p3_screen.x));
			int min_y = min(static_cast<int>(p1_screen.y), static_cast<int>(p2_screen.y), static_cast<int>(p3_screen.y));
			int max_y = max(static_cast<int>(p1_screen.y), static_cast<int>(p2_screen.y), static_cast<int>(p3_screen.y));

			max_x=framebuffer->w;
			max_y=framebuffer->h;
			min_x=0;
			min_y=0;
			
			for (int y = min_y; y <= max_y; ++y) 
			{
				for (int x = min_x; x <= max_x; ++x) 
				{
					// Create a point in the middle of the pixel
					glm::vec4 point((float)x / screen_width * 2.0f - 1.0f, 1.0f - (float)y / screen_height * 2.0f, 0.0f, 1.0f);

					std::vector<float> Barycentric_C = Barycentric_Coordinates(p1_perspective,p2_perspective,p3_perspective,point);

					glm::vec4 average_c = average_color(color_vertex_1,color_vertex_2,color_vertex_3,Barycentric_C);

					glm::vec4 colors = convert_to_rgb_colors(average_c);

					float z = Barycentric_C[0]*p1.z+Barycentric_C[1]*p2.z+Barycentric_C[2]*p3.z;
					
					if (is_in_triangle(p1_perspective, p2_perspective, p3_perspective, point)) 
					{

						if(depth_enabled==false)
						{
							Uint32 color = SDL_MapRGB(framebuffer->format, colors.x, colors.y, colors.z);
							draw_pixel(framebuffer, x, y, color);
						}

						else if(z<=z_buffer[y*screen_width+x])
						{
							z_buffer[y*screen_width+x]=z;
							Uint32 color = SDL_MapRGB(framebuffer->format, colors.x, colors.y, colors.z);
							draw_pixel(framebuffer, x, y, color);
						}

					}
				}
			}
		}


		int Rasterizer::min(int a,int b ,int c)
		{
			int i = std::min(a,b);
			int j = std::min(b,c);			
			return std::min(i,j);
		}

		int Rasterizer::max(int a,int b ,int c)
		{
			int i = std::max(a,b);
			int j = std::max(b,c);

			return std::max(i,j);
		}

	

		glm::vec4 Rasterizer::average_color(glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, std::vector<float> Barycentric_coordinates) {
			// Ensure the Barycentric coordinates vector has exactly 3 elements
			if (Barycentric_coordinates.size() != 3) {
				throw std::invalid_argument("Barycentric_coordinates must have exactly 3 elements.");
			}

			
			glm::vec4 average_color = c1 * Barycentric_coordinates[0] + c2 * Barycentric_coordinates[1] + c3 * Barycentric_coordinates[2];

			return average_color;
		}


		



	}
}
