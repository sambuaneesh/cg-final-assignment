for (int i = 0; i < Area_sample; i++)
{
    light.type_sample = sam_type;
    std::tie(radiance, ls) = light.sample(&si);
    ls.wo = si.toWorld2(ls.wo, si.n, si.p - this->scene.camera.from);

    Ray shadowRay(si.p + 1e-3f * si.n, (ls.wo));
    Interaction siShadow = this->scene.rayIntersect(shadowRay);

    Ray shadowRay2(si.p + 1e-3f * si.n, (ls.wo));
    Interaction area_test = this->scene.rayEmitterIntersect(shadowRay2);

    if (area_test.didIntersect)
    {
        if (!siShadow.didIntersect || siShadow.t > area_test.t)
        {
            if (Dot(ls.p, si.n) <= 0)
            {
                if (sam_type == 0)
                    result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * (AbsDot(si.n, (ls.wo)));
                if (sam_type == 1)
                    result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance;
            }
        }
    }
}

ls.p = this->normal;
float alpha = (next_float()) * 2 * M_PI;
float beeta = (next_float());
beeta = acos(beeta);
float x = sin(beeta) * cos(alpha);
float y = sin(beeta) * sin(alpha);
float z = cos(beeta);
ls.wo = Vector3f(x, y, z);
ls.d = 1000;
radiance = this->radiance * M_PI * 2;

si.samplingStrategy = 1;
std::tie(radiance, ls) = light.sample(&si);
Ray shadowRay(si.p + 1e-5f * si.n, ls.wo);
Interaction siShadow = this->scene.rayIntersect(shadowRay);

Ray lightRay(si.p + 1e-5f * si.n, ls.wo);
Interaction siLight = this->scene.rayEmitterIntersect(lightRay);
// if (si.didIntersect)
// {
//     Vector3f color(0, 0, 0);

//     Vector3f dir_local = uniformSampleHemi(si.n);
//     Vector3f dir_global = si.toWorld(dir_local);
//     Ray shadowRay(si.p + 1e-3f * si.n, dir_global);
//     Interaction siShadow;
//     this->scene.intersectBVH(0, shadowRay, siShadow);
//     if (!siShadow.didIntersect)
//     {
//         color += si.bsdf->eval(&si, si.toLocal(dir_global)) * std::abs(Dot(si.n, dir_local));
//     }
//     result += color / spp;
// }
if (siLight.didIntersect)
{
    if (!siShadow.didIntersect || siShadow.t > siLight.t)
    {
        if (Dot(ls.p, si.n) <= 0)
        {
            color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * (AbsDot(si.n, (ls.wo)));
        }
    }
}
result += color;
}

Render Time : 1897.645020 ms
            Saved PNG : outputs /
                        4.2 / small -
    1.png Render Time : 2745.007080 ms
            Saved PNG : outputs /
                        4.2 / big -
    1.png Render Time : 18329.492188 ms
            Saved PNG : outputs /
                        4.2 / big -
    10.png Render Time : 18919.691406 ms
            Saved PNG : outputs /
                        4.2 / small -
    10.png Render Time : 178884.984375 ms
            Saved PNG : outputs /
                        4.2 / small -
    100.png Render Time : 179717.812500 ms
            Saved PNG : outputs /
                        4.2 / big -
    100.png Render Time : 2796.513916 ms
            Saved PNG : outputs /
                        4.2 / med -
    1.png Render Time : 5702.103027 ms
            Saved PNG : outputs /
                        4.2 / many -
    1.png Render Time : 26288.691406 ms
            Saved PNG : outputs /
                        4.2 / med -
    10.png Render Time : 50855.214844 ms
            Saved PNG : outputs /
                        4.2 / many -
    10.png Render Time : 223708.859375 ms
            Saved PNG : outputs /
                        4.2 / med -
    100.png Render Time : 430536.781250 ms
            Saved PNG : outputs /
                        4.2 / many -
    100.png Render Time : 1884523.375000 ms
            Saved PNG : outputs /
                        4.2 / big -
    1000.png Render Time : 1883273.500000 ms
            Saved PNG : outputs /
                        4.2 / med -
    1000.png