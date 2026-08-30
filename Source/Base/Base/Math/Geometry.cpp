#include "Geometry.h"
#include <glm/glm.hpp>

#include <cmath>

namespace Geometry
{
    bool TryGetBarycentricCoordinates(const vec2& point, const vec2& a, const vec2& b, const vec2& c, vec3& coordinates, f32 degeneracyEpsilon)
    {
        const f32 denominator = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
        const f32 epsilon = degeneracyEpsilon > 0.0f ? degeneracyEpsilon : 0.0f;
        if (!std::isfinite(denominator) || std::abs(denominator) <= epsilon)
        {
            coordinates = vec3(0.0f);
            return false;
        }
        const f32 first = ((b.y - c.y) * (point.x - c.x) + (c.x - b.x) * (point.y - c.y)) / denominator;
        const f32 second = ((c.y - a.y) * (point.x - c.x) + (a.x - c.x) * (point.y - c.y)) / denominator;
        coordinates = vec3(first, second, 1.0f - first - second);
        return std::isfinite(coordinates.x) && std::isfinite(coordinates.y) && std::isfinite(coordinates.z);
    }

    const vec3& Triangle::GetVert(u32 index) const
    {
        assert(index >= 0 && index < 3);

        if (index == 0)
            return vert1;
        else if (index == 1)
            return vert2;
        else
            return vert3;
    }

    vec3 Triangle::GetNormal() const
    {
        vec3 a = vert2 - vert1;
        vec3 b = vert3 - vert1;
        return glm::cross(a, b);
    }
    f32 Triangle::GetSteepnessAngle() const
    {
        vec3 normal = glm::normalize(GetNormal());
        f32 flatness = glm::abs(glm::dot(normal, vec3(0, 0, 1)));
        return glm::degrees(acos(flatness));
    }

    vec3 Triangle::GetCollisionNormal() const
    {
        // GetCollisionNormal is used because drawing CounterClockWise will invert the normal
        return -GetNormal();
    }
    f32 Triangle::GetCollisionSteepnessAngle() const
    {
        // GetCollisionSteepnessAngle is used because drawing CounterClockWise will invert the normal
        vec3 normal = glm::normalize(GetCollisionNormal());
        f32 flatness = glm::abs(glm::dot(normal, vec3(0, 0, 1)));
        return glm::degrees(acos(flatness));
    }
}
