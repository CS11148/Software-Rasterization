#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iomanip>

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
	R::Object clock = r.createObject();
	r.setVertexAttribs(clock, 0, 4, 4, vertices);
	r.setTriangleIndices(clock, 2, triangles);

   


    while (!r.shouldQuit()) {
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.useShaderProgram(program);
        r.setUniform(program,"color",vec4(0,0,0,1.0));
        r.setUniform(program,"transform",mat4(1.0f));
        r.drawObject(clock);
        r.show();
    }
    r.deleteShaderProgram(program);

    return EXIT_SUCCESS;
}

