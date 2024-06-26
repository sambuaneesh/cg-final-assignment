#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
    switch (type)
    {
    case LightType::POINT_LIGHT:
        this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
        break;
    case LightType::DIRECTIONAL_LIGHT:
        this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
        break;
    case LightType::AREA_LIGHT:
        this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
        this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
        this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
        this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
        break;
    default:
        std::cout << "WARNING: Invalid light type detected";
        break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction *si)
{
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type)
    {
    case LightType::POINT_LIGHT:
        ls.wo = (position - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    case LightType::DIRECTIONAL_LIGHT:
        ls.wo = Normalize(direction);
        ls.d = 1e10;
        radiance = this->radiance;
        break;
    case LightType::AREA_LIGHT:
        if (si->samplingStrategy == 0)
        {
            float u1 = next_float();
            float u2 = next_float();

            float r = std::sqrt(1 - u1 * u1);
            float phi = 2 * M_PI * u2;

            float x = r * std::cos(phi);
            float y = r * std::sin(phi);
            float z = u1;

            ls.wo = si->toWorld(Vector3f(x, y, z));
            ls.d = 1e10;
            radiance = this->radiance * M_PI * 2;
        }
        else if (si->samplingStrategy == 1)
        {
            float u1 = next_float();
            float u2 = next_float();

            float phi = 2 * M_PI * u1;
            float cos_theta = std::sqrt(1.0f - u2);
            float sin_theta = std::sqrt(u2);

            float x = cos(phi) * sin_theta;
            float y = sin(phi) * sin_theta;
            float z = cos_theta;

            ls.wo = si->toWorld(Vector3f(x, y, z));
            ls.d = 1e10;
            radiance = this->radiance * M_PI;
        }
        else
        {

            Vector3f p = center + (2 * next_float() - 1) * vx + (2 * next_float() - 1) * vy;
            ls.wo = Normalize(p - si->p);
            radiance = this->radiance / (si->p - p).LengthSquared();
            ls.d = (si->p - p).Length();

            auto cos_theta_l = AbsDot(Normalize(normal), ls.wo);
            auto area = 4 * vx.Length() * vy.Length();
            radiance *= cos_theta_l;
            radiance *= area;
            break;
        }
    }
    return {radiance, ls};
}

Interaction Light::intersectLight(Ray *ray)
{
    Interaction si;
    memset(&si, 0, sizeof(si));

    // check if the ray intersects with the area light
    if (type == LightType::AREA_LIGHT) // check if the ray intersects with the area light rectangle
    {
        float t = Dot((center - ray->o), normal) / Dot(ray->d, normal);
        if (t > 0)
        {
            Vector3f p = ray->o + t * ray->d;
            Vector3f d = p - center;

            if (AbsDot(d, vx) / vx.Length() <= vx.Length() && AbsDot(d, vy) / vy.Length() <= vy.Length())
            {
                si.p = p;
                si.n = normal;
                si.t = t;
                si.didIntersect = true;
                si.emissiveColor = this->radiance;
                return si;
            }
        }
    }
    si.t = 1e30f;
    si.didIntersect = false;
    return si;
}