#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

int main() {
	R::Rasterizer r;
    if (!r.initialize("Example 1", 640, 640))
        return EXIT_FAILURE;
    R::ShaderProgram program = r.createShaderProgram(
        r.vsTransform(),
        r.fsConstant()
    );
    float vertices[] = {
		-0.1,  -0.1, 0.0, 1.0,
        -0.1,  0.1, 0.0, 1.0,
         0.1,  -0.1, 0.0, 1.0,
         0.1,  0.1, 0.0, 1.0
    };
	int triangles[] = {
		0, 1, 3,
		0, 2, 3
	};
	R::Object tickmark = r.createObject();
	r.setVertexAttribs(tickmark, 0, 4, 4, vertices);
	r.setTriangleIndices(tickmark, 2, triangles);

    glm::mat4 big_mark_scale = glm::scale((mat4(1.0f)),vec3(0.2,0.6,1.0f));
    glm::mat4 translate_big_mark = glm::translate(mat4(1.0f),vec3(0.0f,1,0.0));
   

    glm::mat4 small_mark_scale = glm::scale(mat4(1.0f),vec3(0.05,0.4,1.0f));
    glm::mat4 translate_small_mark = glm::translate(mat4(1.0f),vec3(0.0f,1.53,0.0));
    
    glm::mat4 hour_hand_scale = glm::scale(mat4(1.0f),vec3(0.2,2.5,1.0));
    glm::mat4 hour_hand_translate = glm::translate(mat4(1.0f),vec3(0.0,0.15,1.0));

    glm::mat4 minute_hand_scale = glm::scale(mat4(1.0f),vec3(0.12,3.0,1.0));
    glm::mat4 minute_hand_translate = glm::translate(mat4(1.0f),vec3(0.0,0.12,1.0));

    glm::mat4 second_hand_scale = glm::scale(mat4(1.0f),vec3(0.08,3.5,0.0f));
    glm::mat4 second_hand_translate = glm::translate(mat4(1.0f),vec3(0.0f,0.18,1.0));

    glm::mat4 rotate = glm::rotate(mat4(1.0f),radians(90.0f),vec3(0.0f,0.0f,-1.0f));


    while (!r.shouldQuit()) {
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
            r.useShaderProgram(program);
            r.setUniform<vec4>(program, "color", vec4(0.0, 0.0, 0.0, 1.0));
            glm::mat4 initial_scale = scale(mat4(1.0f),vec3(0.5,0.5,1.0f));
            glm::mat4 initial = initial_scale;
        for(int i=0;i<12;++i){
            glm::mat4 rotate_big_mark = glm::rotate(mat4(1.0f),radians(i*30.0f),vec3(0.0f,0.0f,1.0f));
            r.setUniform<mat4>(program,"transform",rotate_big_mark*big_mark_scale*translate_big_mark);
            r.drawObject(tickmark);
        }
        for(int i=0;i<60;++i){
            glm::mat4 rotate_small_mark = glm::rotate(mat4(1.0f),radians(i*6.0f),vec3(0.0f,0.0f,1.0f));
            r.setUniform<mat4>(program,"transform",rotate_small_mark*small_mark_scale*translate_small_mark);
            r.drawObject(tickmark);
        }
            r.setUniform<mat4>(program,"transform", hour_hand_translate*hour_hand_scale);
            r.drawObject(tickmark);

            r.setUniform<mat4>(program,"transform",rotate*minute_hand_translate*minute_hand_scale);
            r.drawObject(tickmark);

            r.setUniform<vec4>(program,"color",vec4(0.8,0.0,0.0,1.0));
            r.setUniform<mat4>(program,"transform",rotate*rotate*second_hand_translate*second_hand_scale);
            r.drawObject(tickmark);

            r.setUniform<vec4>(program,"color",vec4(0.0,0.0,0.0,1.0));
            r.setUniform<mat4>(program,"transform",initial_scale);
            r.drawObject(tickmark);

        r.show();
    }
    r.deleteShaderProgram(program);

    return EXIT_SUCCESS;
}

