#include "Math.h"

#include <cmath>

namespace Math
{
    f32 Sqrt(f32 in)
    {
        return sqrt(in);
    }

    f32 Wrap(f32 value, f32 minimum, f32 maximum)
    {
        const f32 range = maximum - minimum;
        if (!(range > 0.0f))
            return value;
        f32 mapped = std::fmod(value - minimum, range);
        if (mapped < 0.0f)
            mapped += range;
        return minimum + mapped;
    }

    f32 Tan(f32 in)
    {
        return tan(in);
    }
}
