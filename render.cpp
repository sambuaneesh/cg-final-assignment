#include "render.h"
#include "random.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int spp, int samplingStrategy)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int i = 0; i < spp; i++)
            {
                Ray cameraRay = this->scene.camera.generateRay(x, y, next_float(), next_float());
                Interaction si = this->scene.rayIntersect(cameraRay);

                result += integrateSample(si, samplingStrategy, cameraRay);
            }
            result /= spp;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

Vector3f Integrator::integrateSample(Interaction &si, int samplingStrategy, Ray cameraRay)
{
    Vector3f color(0, 0, 0);
    Vector3f radiance;
    LightSample ls;

    for (Light &light : this->scene.lights)
    {

        si.samplingStrategy = samplingStrategy;

        std::tie(radiance, ls) = light.sample(&si);
        Ray shadowRay(si.p + 1e-5f * si.n, ls.wo);
        Interaction siShadow = this->scene.rayIntersect(shadowRay);

        // Directional Light case
        if ((light.type != AREA_LIGHT) && (!siShadow.didIntersect || siShadow.t > ls.d))
        {
            color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
        }
        // Area Light case
        else if (si.didIntersect)
        {
            Ray lightRay(si.p + 1e-3f * si.n, ls.wo);
            Interaction siLight = this->scene.rayEmitterIntersect(lightRay);

            if (siLight.didIntersect && (!siShadow.didIntersect || siShadow.t > siLight.t) && Dot(lightRay.d, light.normal) <= 0)
            {
                if (samplingStrategy == 0)
                {
                    color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * (AbsDot(si.n, ls.wo)) / this->scene.lights.size();
                }
                else if (samplingStrategy == 1)
                {
                    color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance / this->scene.lights.size();
                }
                else if (samplingStrategy == 2)
                {
                    if ((!siShadow.didIntersect || siShadow.t > ls.d) && (Dot(light.normal, -1 * shadowRay.d) > 0))
                    {

                        color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * AbsDot(si.n, ls.wo);
                    }
                }
            }
            Interaction siEmitter = this->scene.rayEmitterIntersect(cameraRay);
            if (siEmitter.didIntersect)
            {
                color += siEmitter.emissiveColor;
            }
        }
    }
    return color;
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    int samplingStrategy = atoi(argv[4]);
    auto renderTime = rayTracer.render(spp, samplingStrategy);

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}