#include <../glm/glm.hpp>
#include <string>

const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
glm::vec3 eye = glm::vec3(0.f, 2.5f, 3.0f);

#include "do_not_edit.h"
#include "raytrace.h"

int main()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    LARGE_INTEGER Render_StartingTime, Render_EndingTime;
    QueryPerformanceCounter(&Render_StartingTime);


    obj_parse(MODEL_PATH.c_str(), &tris, 1.f);

    raytrace();

    savebitmap("render.bmp", pixelBuffer, PIXEL_W, PIXEL_H);

    CounterEndAndPrint(Render_StartingTime, &Render_EndingTime, Frequency);
}

