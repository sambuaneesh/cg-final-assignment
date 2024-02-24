#pragma once

#include "scene.h"

struct Integrator
{
    Integrator(Scene &scene);

    long long render(int spp, int samplingStrategy);

    Vector3f integrateSample(Interaction &si, int samplingStrategy, Ray cameraRay);

    Scene scene;
    Texture outputImage;
};