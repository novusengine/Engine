#pragma once
#include "Base/Types.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Spline
{
    struct InterpolatedStorage
    {
    private:
        static const std::vector<vec3> _emptyVector;

    public:
        explicit InterpolatedStorage(u32 step);

        void SetStep(u32 newStep);
        void StoreInterpolation(const std::vector<vec3>& points, u32 step);
        void AddPortion(const std::vector<vec3>& points, i32 position = -1);

        void Clear() { _points.clear(); };
        void MarkAsDirty() { _dirty = true; };
        bool CheckSize() { return (_points.size() % _stepSize) == 0; };

        [[nodiscard]] bool IsDirty() { return _dirty; };
        [[nodiscard]] const std::vector<vec3>& GetInterpolatedStorage(u32 splinePoints = 0);

    protected:
        bool _dirty = true;
        u32 _stepSize = 0;
        std::vector<vec3> _points = {};
    };

    struct SplinePoint
    {
    public:
        SplinePoint() { };

        /*
         * None:
         * Linear:
         *  - enter/exit: unused
         *
         * Bezier:
         * Hermite:
         * CatmullRom:
         *  - enter OR exit: it depends on the position
         * of the selected point in the current portion
         *
         * BSpline:
         *  - enter AND exit: need both control point everytime
         *
         *
         * EXAMPLE: This is an example of a curve portion to show
         * what is `enterPoint` and `exitPoint`:
         *
         *  - `enterA` and `exitA` are the control points for A
         *  - `enterB` and `exitB` are the control points for B.
         *
         *   The curve is passing through A and B. ( also, the spline
         *   can live before and after this portion, doesn't matter )
         *
         *                                        [exitB]
         *   [enterA]                               /
         *       \                                 /
         *        \                        __----[B]---..
         * ..--___[A]-___           ___---|      /
         *          \    ---____----            /
         *           \                       [enterB]
         *          [exitA]
         *
         */

        union
        {
            struct
            {
                vec3 enter = {};
                vec3 point = {};
                vec3 exit = {};
            } control;

            struct
            {
                vec3 data[3];
            } combined {};
        };
    };

    enum class InterpolationType
    {
        None,
        Linear,
        Bezier,
        Hermite,
        BSpline,
        CatmullRom,
        CatmullRom_Uniform,
        CatmullRom_Centripetal,
        CatmullRom_Chordal,
        MAX_TYPE
    };

    class Spline
    {
    public:
        explicit Spline(InterpolationType type);
        Spline(InterpolationType type, const std::vector<SplinePoint>& points);

        bool IsInterpolatedWithControl();
        [[nodiscard]] vec3 InterpolateControl(f32 t, const SplinePoint& start, const SplinePoint& end);
        [[nodiscard]] vec3 InterpolateCombined(f32 t, const vec3* points, i32 index, f32 alpha = 0.5f);

        bool DoInterpolation();
        std::vector<SplinePoint> GetPoints() { return _points; };
        void GetPointsData(std::vector<vec3>& output);
        void UpdatePoint(i32 position, SplinePoint newPoint);

        void AddPoint(const SplinePoint& newPoint, i32 position = -1);
        void RemovePoint(u32 position);
        void Clear() { _points.clear(); _storage.Clear(); };
        void MarkAsDirty() { _storage.MarkAsDirty(); };

        void SetType(InterpolationType newType);
        void SetStep(u32 newStep);
        void SetAlpha(f32 newAlpha);

        [[nodiscard]] InterpolationType GetType() { return _type; };
        [[nodiscard]] const u32 GetStep() { return _step; };
        [[nodiscard]] const std::vector<vec3>& GetInterpolatedPoints() { return _storage.GetInterpolatedStorage(); };

    protected:
        std::vector<SplinePoint> _points = {};
        InterpolatedStorage _storage;

    private:
        InterpolationType _type = InterpolationType::None;
        u32 _step = 1;
        f32 _alpha = 0.5f;
    };
}
