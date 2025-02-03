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


            glm::mat4 boundary_scale = scale(mat4(1.0f),vec3(7.5f,7.5f,1.0f));
            r.setUniform<vec4>(program,"color",vec4(0.625,0.625,0.625,1.0f));
            r.setUniform<mat4>(program,"transform",boundary_scale);
            r.drawObject(tickmark);


            r.setUniform<vec4>(program, "color", vec4(0.0, 0.0, 0.0, 1.0));
            glm::mat4 initial_scale = scale(mat4(1.0f),vec3(0.5,0.5,1.0f));
            


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

            auto now = std::chrono::system_clock::now();

            // Convert system time to time_t (seconds since epoch)
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

            // Get the current local time from the time_t
            std::tm* local_time = std::localtime(&now_time_t);

            // Extract hour, minute, and second
            int hour = local_time->tm_hour;  // 24-hour format
            int minute = local_time->tm_min; // Minutes
            int second = local_time->tm_sec; // Seconds


            if (hour >= 12) {
                if (hour >= 12) {
                    hour -= 12; // Convert hour to 12-hour format (subtract 12 if greater than 12)
                }
            }

            // Convert to fractional hours
            float hours_in_float = hour + (minute / 60.0f) + (second / 3600.0f);

            // Convert to fractional minutes
            float minutes_in_float = minute + (second / 60.0f);


            float hour_in_degree = (hours_in_float/12)*360.0f;
            float minute_in_degree = (minutes_in_float/60)*360.0f;
            float seconds_in_degree = (float(second)/60)*360.0f;

            glm::mat4 rotate_hours = glm::rotate(mat4(1.0f),radians(hour_in_degree),vec3(0.0f,0.0f,-1.0f));

            r.setUniform<mat4>(program,"transform", rotate_hours*hour_hand_translate*hour_hand_scale);
            r.drawObject(tickmark);

            glm::mat4 rotate_minutes = glm::rotate(glm::mat4(1.0f),radians(minute_in_degree),vec3(0.0f,0.0f,-1.0f));

            r.setUniform<mat4>(program,"transform",rotate_minutes*minute_hand_translate*minute_hand_scale);
            r.drawObject(tickmark);

            glm::mat4 rotate_seconds = glm::rotate(glm::mat4(1.0f),radians(seconds_in_degree),vec3(0.0f,0.0f,-1.0f));

            r.setUniform<vec4>(program,"color",vec4(0.8,0.0,0.0,1.0));
            r.setUniform<mat4>(program,"transform",rotate_seconds*second_hand_translate*second_hand_scale);
            r.drawObject(tickmark);

            r.setUniform<vec4>(program,"color",vec4(0.0,0.0,0.0,1.0));
            r.setUniform<mat4>(program,"transform",initial_scale);
            r.drawObject(tickmark);

        r.show();
    }
    r.deleteShaderProgram(program);

    return EXIT_SUCCESS;
}

