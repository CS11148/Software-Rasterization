#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iomanip>

#include <iostream>

namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

int main() {
	R::Rasterizer r;
    if (!r.initialize("Windmill", 640, 640))
        return EXIT_FAILURE;
    R::ShaderProgram program = r.createShaderProgram(
        r.vsTransform(),
        r.fsConstant()
    );
    float vertices[] = {

		-0.5,  -0.6, -0.5, 1.0,
        -0.5,  -0.7, -0.5, 1.0,
         0.5,  -0.7, -0.5, 1.0,
         0.5,  -0.6, -0.5, 1.0,


        -0.5,  -0.6, 0.5, 1.0,
        -0.5,  -0.7, 0.5, 1.0,
         0.5,  -0.7, 0.5, 1.0,
         0.5,  -0.6, 0.5, 1.0
    };
	int triangles[] = {
        // Front face
        0, 1, 2,
        0, 2, 3,

        // // Back face
        // 4, 5, 6,
        // 4, 6, 7,

        // Left face
        0, 1, 5,
        0, 5, 4,

        // Right face
        3, 2, 6,
        3, 6, 7,

        // Top face
        4, 0, 3,
        4, 3, 7,

        
    };

	R::Object clock = r.createObject();
	r.setVertexAttribs(clock, 0, 8, 4, vertices);
	r.setTriangleIndices(clock, 8, triangles);
    r.enableDepthTest();

   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float vertices1[] = {

		-0.15,  0.7, -0.15, 1.0,
        -0.15,  -0.6, -0.15, 1.0,
         0.15,  -0.6, -0.15, 1.0,
         0.15,  0.7, -0.15, 1.0,


        -0.15,  0.7, 0.15, 1.0,
        -0.15,  -0.6, 0.15, 1.0,
         0.15,  -0.6, 0.15, 1.0,
         0.15,  0.7, 0.15, 1.0
    };
	int triangles1[] = {
        // Front face
        0, 1, 2,
        0, 2, 3,

        // Left face
        0, 1, 5,
        0, 5, 4,

        // Right face
        3, 2, 6,
        3, 6, 7,

        // Top face
        4, 0, 3,
        4, 3, 7,

        
    };

	R::Object clock1 = r.createObject();
	r.setVertexAttribs(clock1, 0, 8, 4, vertices1);
	r.setTriangleIndices(clock1, 8, triangles1);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float vertices2[] = {

		-0.11,  0.11, -0.25, 1.0,
        -0.11,  -0.11, -0.25, 1.0,
         0.11,  -0.11, -0.25, 1.0,
         0.11,  0.11, -0.25, 1.0,

        -0.07, 0.6, -0.25, 1.0,
         0.07, 0.6, -0.25, 1.0,

         0.6, 0.07, -0.25, 1.0,
         0.6, -0.07, -0.25, 1.0,

         0.07, -0.6, -0.25, 1.0,
        -0.07, -0.6, -0.25, 1.0,

        -0.6, -0.07, -0.25, 1.0,
        -0.6, 0.07, -0.25, 1.0

    };

	int triangles2[] = {
        0, 1, 2,
        0, 2, 3,

        0, 3, 4,
        4, 5, 3,

        3, 6, 7,
        3, 7, 2,

        1, 8, 9,
        1, 8, 2,

        1, 10, 11,
        0, 1, 11

        
    };

	R::Object clock2 = r.createObject();
	r.setVertexAttribs(clock2, 0, 12, 4, vertices2);
	r.setTriangleIndices(clock2, 10, triangles2);

    R::ShaderProgram program1 = r.createShaderProgram(
        r.vsColor(),
        r.fsIdentity()
    );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float vertices3[] = {

		-3.0,  3.0, 0.9, 1.0,
        -3.0,  -0.45, 0.9, 1.0,
         3.0,  -0.45, 0.9, 1.0,
         3.0,  3.0, 0.9, 1.0,


    };

    float colors[] = {
        0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 0.45, 1.0,
        0.0, 0.0, 0.45, 1.0,
        0.0, 0.0, 1.0, 1.0
    };

	int triangles3[] = {
        0, 1, 2,
        0, 2, 3,
        
    };

	R::Object clock3 = r.createObject();
	r.setVertexAttribs(clock3, 0, 4, 4, vertices3);
    r.setVertexAttribs(clock3, 1, 4, 4, colors);
	r.setTriangleIndices(clock3, 2, triangles3);



    mat4 rotation_base = glm::rotate(mat4(1.0f),radians(30.0f),vec3(0.0f,1.0f,0.0f));
    mat4 translation = glm::translate(mat4(1.0f),vec3(0.0f,0.35,0.0f));
    mat4 translation_rotation = rotation_base*translation;



    int i = -1;


    while (!r.shouldQuit()) 
    {
        i=i+45;

        r.clear(vec4(0.0, 0.45, 0.0, 1.0));
        r.useShaderProgram(program);
        r.setUniform(program,"color",vec4(0,0,0,1.0));

       

        r.setUniform(program,"transform",rotation_base);
        r.drawObject(clock);


        r.setUniform(program,"color",vec4(0.52,0.52,0.52,1));

        r.drawObject(clock1);


        r.setUniform(program,"color",vec4(0.34,0.34,0.34,1));
        r.setUniform(program,"transform", translation_rotation);

        mat4 rotate_wind = glm::rotate(mat4(1.0f),radians(float(i)),vec3(0.0f,0.0f,-1.0f));

        r.setUniform(program,"transform", translation_rotation*rotate_wind);

        r.drawObject(clock2);




        r.useShaderProgram(program1);

        r.drawObject(clock3);




        r.show();
    }
    r.deleteShaderProgram(program);
    r.deleteShaderProgram(program1);

    return EXIT_SUCCESS;
}

