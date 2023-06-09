#include "Math.h"

namespace Spline
{
    class Interpolation
    {
    public:
        static void ClampAnim(f32& t)
        {
            t = Math::Clamp(t, 0.0f, 1.0f);
        }

        static void ClampPolynomialDegree(i32& degree)
        {
            degree = Math::Clamp(degree, 2, 32);
        }

        struct Linear
        {
            static f32 Lerp(f32 t, const f32& p0, const f32& p1)
            {
                return (p0 * (1.0f - t)) + (p1 * t);
            }

            static vec2 Lerp(f32 t, const vec2& p0, const vec2& p1)
            {
                return (p0 * (1.0f - t)) + (p1 * t);
            }

            static vec3 Lerp(f32 t, const vec3& p0, const vec3& p1)
            {
                return (p0 * (1.0f - t)) + (p1 * t);
            }

            static vec4 Lerp(f32 t, const vec4& p0, const vec4& p1)
            {
                return (p0 * (1.0f - t)) + (p1 * t);
            }
        };

        struct Sigmoid
        {
            static vec3 Smoothstep(f32 t, const vec3& p0, const vec3& p1)
            {
                t = Math::Clamp((t - p0.x) / (p1.x - p0.x), 0.0f, 1.0f);
                t = t * t * (3.0f - 2.0f * t);
                return p0 + t * (p1 - p0);
            }
        };

        struct Ease
        {
            struct Out
            {
                static f32 Sine(f32 t)
                {
                    ClampAnim(t);
                    return Math::Sin((t * Math::PI) / 2.0f);
                }

                static f32 Circ(f32 t)
                {
                    ClampAnim(t);
                    return Math::Sqrt(1.0f - pow(t - 1.0f, 2.0f));
                }

                static f32 Elastic(f32 t)
                {
                    ClampAnim(t);
                    if (t == 0.0f || t == 1.0f)
                        return t;

                    f32 c4 = (2.0f * Math::PI) / 3.0f;
                    return pow(2.0f, -10.0f * t) * Math::Sin((t * 10.0f - 0.75f) * c4) + 1.0f;
                }

                static f32 Expo(f32 t)
                {
                    ClampAnim(t);
                    if (t == 1.0f)
                        return t;

                    return 1.0f - pow(2.0f, -10.0f * t);
                }

                static f32 Back(f32 t)
                {
                    ClampAnim(t);
                    f32 c1 = 1.70158f;
                    f32 c3 = c1 + 1.0f;

                    return 1.0f + c3 * pow(t - 1.0f, 3.0f) + c1 * pow(t - 1.0f, 2.0f);
                }

                static f32 Bounce(f32 t)
                {
                    f32 n1 = 7.5625f;
                    f32 d1 = 2.75f;

                    if (t < 1.0f / d1)
                    {
                        return n1 * t * t;
                    }
                    else if (t < 2.0f / d1)
                    {
                        t -= 1.5f / d1;
                        return n1 * t * t + 0.75f;
                    }
                    else if (t < 2.5f / d1)
                    {
                        t -= 2.25f / d1;
                        return n1 * t * t + 0.9375f;
                    }
                    else
                    {
                        t -= 2.625f / d1;
                        return n1 * t * t + 0.984375f;
                    }
                }

                static f32 Polynomial(f32 t, i32 degree)
                {
                    ClampAnim(t);
                    ClampPolynomialDegree(degree);
                    return 1.0f - pow(1.0f - t, static_cast<f32>(degree));
                }

                static f32 Quadratic(f32 t) { return Polynomial(t, 2); }
                static f32 Cubic(f32 t)     { return Polynomial(t, 3); }
                static f32 Quartic(f32 t)   { return Polynomial(t, 4); }
                static f32 Quintic(f32 t)   { return Polynomial(t, 5); }
                static f32 Sextic(f32 t)    { return Polynomial(t, 6); }
                static f32 Septic(f32 t)    { return Polynomial(t, 7); }
                static f32 Octic(f32 t)     { return Polynomial(t, 8); }
            };

            struct In
            {
                static f32 Sine(f32 t)
                {
                    ClampAnim(t);
                    return 1.0f - Math::Cos((t * Math::PI) / 2.0f);
                }

                static f32 Circ(f32 t)
                {
                    ClampAnim(t);
                    return 1.0f - Math::Sqrt(1.0f - pow(t, 2.0f));
                }

                static f32 Elastic(f32 t)
                {
                    ClampAnim(t);
                    if (t == 0.0f || t == 1.0f)
                        return t;

                    f32 c4 = (2.0f * Math::PI) / 3.0f;
                    return -pow(2.0f, 10.0f * t - 10.0f) * Math::Sin((t * 10.0f - 10.75f) * c4);
                }

                static f32 Expo(f32 t)
                {
                    ClampAnim(t);
                    if (t == 0.0f)
                        return t;

                    return pow(2.0f, 10.0f * t - 10.0f);
                }

                static f32 Back(f32 t)
                {
                    ClampAnim(t);
                    f32 c1 = 1.70158f;
                    f32 c3 = c1 + 1.0f;

                    return c3 * t * t * t - c1 * t * t;
                }

                static f32 Bounce(f32 t)
                {
                    ClampAnim(t);
                    return 1.0f - Ease::Out::Bounce(1.0f - t);
                }

                static f32 Polynomial(f32 t, i32 degree)
                {
                    ClampAnim(t);
                    ClampPolynomialDegree(degree);
                    return pow(t, static_cast<f32>(degree));
                }

                static f32 Quadratic(f32 t) { return Polynomial(t, 2); }
                static f32 Cubic(f32 t)     { return Polynomial(t, 3); }
                static f32 Quartic(f32 t)   { return Polynomial(t, 4); }
                static f32 Quintic(f32 t)   { return Polynomial(t, 5); }
                static f32 Sextic(f32 t)    { return Polynomial(t, 6); }
                static f32 Septic(f32 t)    { return Polynomial(t, 7); }
                static f32 Octic(f32 t)     { return Polynomial(t, 8); }
            };

            struct InOut
            {
                static f32 Sine(f32 t)
                {
                    ClampAnim(t);
                    return -(Math::Cos(Math::PI * t) - 1.0f) / 2.0f;
                }

                static f32 Circ(f32 t)
                {
                    ClampAnim(t);
                    if (t < 0.5f)
                        return (1.0f - Math::Sqrt(1.0f - pow(2.0f * t, 2.0f))) / 2.0f;

                    return (Math::Sqrt(1.0f - pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
                }

                static f32 Elastic(f32 t)
                {
                    ClampAnim(t);
                    if (t == 0.0f || t == 1.0f)
                        return t;

                    f32 c5 = (2.0f * Math::PI) / 4.5f;
                    if (t < 0.5f)
                        return -(pow(2.0f, 20.0f * t - 10.0f) * Math::Sin((20.0f * t - 11.125f) * c5)) / 2.0f;

                    return (pow(2.0f, -20.0f * t + 10.0f) * Math::Sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
                }

                static f32 Expo(f32 t)
                {
                    ClampAnim(t);
                    if (t == 0.0f || t == 1.0f)
                        return t;

                    if (t < 0.5f)
                        return pow(2.0f, 20.0f * t - 10.0f) / 2.0f;

                    return (2 - pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
                }

                static f32 Back(f32 t)
                {
                    f32 c1 = 1.70158f;
                    f32 c2 = c1 * 1.525f;

                    if (t < 0.5f)
                        return (pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f;

                    return (pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
                }

                static f32 Bounce(f32 t)
                {
                    ClampAnim(t);
                    if (t < 0.5f)
                        return (1.0f - Ease::Out::Bounce(1.0f - 2.0f * t)) / 2.0f;

                    return (1.0f + Ease::Out::Bounce(2.0f * t - 1.0f)) / 2.0f;
                }

                static f32 Polynomial(f32 t, i32 degree)
                {
                    ClampAnim(t);
                    ClampPolynomialDegree(degree);

                    if (t < 0.5f)
                        return pow(2.0f, static_cast<f32>(degree - 1)) * Ease::In::Polynomial(t, degree);

                    return 1.0f - pow(-2.0f * t + 2.0f, static_cast<f32>(degree)) / 2.0f;
                }

                static f32 Quadratic(f32 t) { return Polynomial(t, 2); }
                static f32 Cubic(f32 t)     { return Polynomial(t, 3); }
                static f32 Quartic(f32 t)   { return Polynomial(t, 4); }
                static f32 Quintic(f32 t)   { return Polynomial(t, 5); }
                static f32 Sextic(f32 t)    { return Polynomial(t, 6); }
                static f32 Septic(f32 t)    { return Polynomial(t, 7); }
                static f32 Octic(f32 t)     { return Polynomial(t, 8); }
            };
        };

        struct Bezier
        {
            static vec3 Cubic(f32 t, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3)
            {
                f32 t2 = t * t;
                f32 t3 = t2 * t;

                f32 h0 = -1.0f * t3 + 3.0f * t2 - 3.0f * t + 1.0f;
                f32 h1 = 3.0f * t3 - 6.0f * t2 + 3.0f * t;
                f32 h2 = -3.0f * t3 + 3.0f * t2;
                //f32 h3 = t3;

                return ((p0 * h0) + (p1 * h1) + (p2 * h2) + (p3 * t3));
            }
        };

        struct Hermite
        {
            static vec3 Cubic(f32 t, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3)
            {
                f32 t2 = t * t;
                f32 t3 = t2 * t;

                f32 h0 = 2.0f * t3 - 3.0f * t2 + 1.0f;
                f32 h1 = -2.0f * t3 + 3.0f * t2;
                f32 h2 = t3 - 2 * t2 + t;
                f32 h3 = t3 - t2;

                return ((p0 * h0) + (p1 * h1) + (p2 * h2) + (p3 * h3));
            }
        };

        struct BSpline
        {
            static vec3 BasisSpline(f32 t, const vec3* points, const i32 index)
            {
                f32 t2 = t * t;
                f32 t3 = t2 * t;

                f32 h0 = (1 - t) * (1 - t) * (1 - t);
                f32 h1 = (3.0f * t3 - 6.0f * t2 + 4.0f);
                f32 h2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f);
                f32 h3 = t3;

                vec3 p0 = *(points + index + 0);
                vec3 p1 = *(points + index + 1);
                vec3 p2 = *(points + index + 2);
                vec3 p3 = *(points + index + 3);

                return (p0 * h0 + p1 * h1 + p2 * h2 + p3 * h3) / 6.0f;
            }
        };

        struct CatmullRom
        {
            static f32 GetT(f32 t, f32 alpha, const vec3& p0, const vec3& p1)
            {
                auto d = p1 - p0;
                f32 a = glm::dot(d, d);
                f32 b = glm::pow(a, alpha * 0.5f);
                return (b + t);
            }

            static vec3 Base(f32 t, f32 alpha, const vec3* points, const i32 index)
            {
                vec3 p0 = *(points + index + 0);
                vec3 p1 = *(points + index + 1);
                vec3 p2 = *(points + index + 2);
                vec3 p3 = *(points + index + 3);

                f32 t0 = 0.0f;
                f32 t1 = GetT(t0, alpha, p0, p1);
                f32 t2 = GetT(t1, alpha, p1, p2);
                f32 t3 = GetT(t2, alpha, p2, p3);

                t = Math::Lerp(t1, t2, t);

                f32 h0 = (t1 - t);
                f32 h1 = (t1 - t0);
                f32 h2 = (t - t0);
                f32 h3 = (t2 - t);
                f32 h4 = (t2 - t1);
                f32 h5 = (t - t1);
                f32 h6 = (t3 - t);
                f32 h7 = (t3 - t2);
                f32 h8 = (t - t2);
                f32 h9 = (t2 - t0);
                f32 h10 = (t3 - t1);

                f32 j0 = (h3 / h4);
                f32 j1 = (h5 / h4);

                vec3 a1 = h0 / h1  * p0 + h2 / h1 * p1;
                vec3 a2 = j0  * p1 + j1 * p2;
                vec3 a3 = h6 / h7  * p2 + h8 / h7 * p3;

                vec3 b1 = h3 / h9  * a1 + h2 / h9 * a2;
                vec3 b2 = h6 / h10 * a2 + h5 / h10 * a3;

                vec3 c  = j0 * b1 + j1 * b2;
                return c;
            }

            static vec3 Uniform(f32 t, const vec3* points, const i32 index)
            {
                return CatmullRom::Base(t, 0.0f, points, index);
            }

            static vec3 Centripetal(f32 t, const vec3* points, const i32 index)
            {
                return CatmullRom::Base(t, 0.5f, points, index);
            }

            static vec3 Chordal(f32 t, const vec3* points, const i32 index)
            {
                return CatmullRom::Base(t, 1.0f, points, index);
            }
        };
    };
}
