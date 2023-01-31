#include "Geometry.h"
#include <glm/glm.hpp>

namespace Geometry
{
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