#include "render.h"
#include "random.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int spp)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            for (int i = 0; i < spp; i++)
            {
                float rand = next_float();

                Ray cameraRay = this->scene.camera.generateRay(x, y, rand);
                Interaction si = this->scene.rayIntersect(cameraRay);

                if (si.didIntersect)
                {
                    Vector3f color = Vector3f(0, 0, 0);
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {

                        for (int i = 0; i < NUM_SAMPLES; i++)
                        {
                            std::tie(radiance, ls) = light.sample(&si);
                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);
                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                if (light.getType() == AREA_LIGHT)
                                {
                                    if (Dot(light.normal, -1 * shadowRay.d) < 0)
                                    {
                                        int x = 0;
                                    }
                                    else
                                    {

                                        auto cos_theta_l = Dot(Normalize(light.normal), Normalize(siShadow.n));
                                        auto area = 4 * light.getVx().Length() * light.getVy().Length();
                                        color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)) * cos_theta_l * area;
                                    }
                                }

                                Interaction siEmitter = this->scene.rayEmitterIntersect(cameraRay);
                                if (siEmitter.didIntersect)
                                {
                                    color += siEmitter.emissiveColor;
                                }
                            }
                        }

                        // std::tie(radiance, ls) = light.sample(&si);

                        // Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        // Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        // if (!siShadow.didIntersect || siShadow.t > ls.d)
                        // {
                        //     color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        //     Interaction siEmitter = this->scene.rayEmitterIntersect(cameraRay);
                        //     if (siEmitter.didIntersect)
                        //     {
                        //         color += siEmitter.emissiveColor;
                        //     }
                        // }
                    }
                    result += color;
                }
            }
            result /= NUM_SAMPLES;
            result /= spp;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
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
    auto renderTime = rayTracer.render(spp);

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
