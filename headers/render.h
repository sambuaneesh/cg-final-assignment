#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene);

    long long render(int numSamples);

    Scene scene;
    Texture outputImage;
};