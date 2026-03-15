
#include "do_not_edit.h"
#include "rasterise.h"


int main()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER Render_StartingTime, Render_EndingTime;
    QueryPerformanceCounter(&Render_StartingTime);

    const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";

    std::vector<triangle> tris;
    obj_parse(MODEL_PATH.c_str(), &tris);

    render(tris);

    savebitmap("render.bmp", colour_buffer, PIXEL_W, PIXEL_H);

    CounterEndAndPrint(Render_StartingTime, &Render_EndingTime, Frequency);
}

