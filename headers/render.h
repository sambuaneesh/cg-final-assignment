#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene);

    long long render(int spp, int samplingStrategy);

    Scene scene;
    Texture outputImage;
};