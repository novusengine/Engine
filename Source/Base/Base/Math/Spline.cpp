#include "Spline.h"
#include "Interpolation.h"

namespace Spline
{
    InterpolatedStorage::InterpolatedStorage(u32 step) : _stepSize(step)
    {

    }

    void InterpolatedStorage::SetStep(u32 newStep)
    {
        if (newStep == 0)
            return;

        _stepSize = newStep;
        _points.clear();
        _dirty = true;
    }

    void InterpolatedStorage::StoreInterpolation(const std::vector<vec3>& points, u32 step)
    {
        _stepSize = step;
        _points = points;
        _dirty = false;
    }

    void InterpolatedStorage::AddPortion(const std::vector<vec3>& points, i32 position)
    {
        if (_stepSize == 0 || points.size() != (_stepSize + 1))
            return;

        if (position < 0)
        {
            _points.insert(_points.end(), points.begin(), points.end());
            _dirty = false;
            return;
        }

        i32 newPosition = position * (_stepSize + 1);
        if (position < _points.size())
        {
            _points.insert(_points.begin() + newPosition, points.begin(), points.end());
            _dirty = false;
            return;
        }
    }

    const std::vector<vec3>& InterpolatedStorage::GetInterpolatedStorage(u32 splinePoints)
    {
        bool checkSize = true;

        if (splinePoints != 0)
        {
            checkSize = ((splinePoints * (_stepSize + 1)) == _points.size());
        }

        // TODO: Temporel look at what this should actually be doing
        //if (IsDirty() || !checkSize)
        //    return _emptyVector;

        return _points;
    }

    Spline::Spline(InterpolationType type) : _type(type), _storage(0)
    {
        _storage.Clear();
        _storage.MarkAsDirty();
    }

    Spline::Spline(InterpolationType type, const std::vector<SplinePoint>& points) : _type(type), _storage(0)
    {
        _points = points;

        _storage.Clear();
        _storage.MarkAsDirty();
    }

    void Spline::AddPoint(const SplinePoint& newPoint, i32 position)
    {
        if (position < 0)
        {
            _points.emplace_back(newPoint);
        }
        else
        {
            _points.insert(_points.begin() + position, newPoint);
        }

        _storage.MarkAsDirty();
    }

    void Spline::RemovePoint(u32 position)
    {
        if (position >= _points.size())
            return;

        std::vector<SplinePoint>::iterator it = _points.begin();
        std::advance(it, position);
        _points.erase(it);
        _storage.MarkAsDirty();
    }

    void Spline::SetStep(u32 newStep)
    {
        newStep = Math::Max((i32)newStep, 1);

        if (_step == newStep)
            return;

        _step = newStep;
        _storage.SetStep(newStep);
    }

    void Spline::SetType(InterpolationType newType)
    {
        if (_type == newType)
            return;

        if (newType < InterpolationType::None || newType >= InterpolationType::MAX_TYPE)
            return;

        _type = newType;
        _storage.MarkAsDirty();
    }

    void Spline::SetAlpha(f32 newAlpha)
    {
        _alpha = Math::Clamp(newAlpha, 0.0f, 1.0f);
        _storage.MarkAsDirty();
    }

    bool Spline::DoInterpolation()
    {
        if (!_storage.IsDirty())
            return false;

        // need to have at least 2 points to draw one line
        if (_points.size() < 2 || _step == 0)
            return false;

        _storage.Clear();
        f32 tStep = (1.0f) / static_cast<f32>(_step);
        if (IsInterpolatedWithControl())
        {
            for (i32 n = 0; n < _points.size() - 1; n++)
            {
                SplinePoint currentPoint = _points[n];
                SplinePoint nextPoint = _points[n + 1];

                std::vector<vec3> currentPortion;
                currentPortion.reserve(_step + 1);

                for (u32 i = 0; i < _step + 1; i++)
                {
                    f32 t = tStep * static_cast<f32>(i);
                    currentPortion.emplace_back(InterpolateControl(t, currentPoint, nextPoint));
                }

                _storage.AddPortion(currentPortion);
            }
        }
        else
        {
            std::vector<vec3> inlinePoints;
            GetPointsData(inlinePoints);

            if (inlinePoints.size() != (_points.size() * 3))
                return false;

            for (i32 n = 0; n < inlinePoints.size() - 4; n++)
            {
                std::vector<vec3> currentPortion;
                currentPortion.reserve(_step + 1);

                for (u32 i = 0; i < _step + 1; i++)
                {
                    f32 t = tStep * static_cast<f32>(i);
                    currentPortion.emplace_back(InterpolateCombined(t, inlinePoints.data(), n, _alpha));
                }

                _storage.AddPortion(currentPortion);
            }
        }

        return true;
    }

    void Spline::GetPointsData(std::vector<vec3>& output)
    {
        if (_points.empty())
            return;

        size_t sizeBefore = output.size();
        output.resize(sizeBefore + _points.size() * 3);
        std::memcpy(&output[sizeBefore], _points.data(), _points.size() * 3 * sizeof(vec3));
    }

    bool Spline::IsInterpolatedWithControl()
    {
        switch (_type)
        {
            case InterpolationType::BSpline:
            case InterpolationType::CatmullRom:
            case InterpolationType::CatmullRom_Uniform:
            case InterpolationType::CatmullRom_Centripetal:
            case InterpolationType::CatmullRom_Chordal:
                return false;
            case InterpolationType::Hermite:
            case InterpolationType::Bezier:
            case InterpolationType::Linear:
            default:
                return true;
        }
    }

    vec3 Spline::InterpolateControl(f32 t, const SplinePoint& start, const SplinePoint& end)
    {
        t = Math::Clamp(t, 0.0f, 1.0f);

        switch (_type)
        {
            case InterpolationType::Linear:
                return Interpolation::Linear::Lerp(t, start.control.point, end.control.point);
            case InterpolationType::Bezier:
                return Interpolation::Bezier::Cubic(t, start.control.point, start.control.exit, end.control.enter, end.control.point);
            case InterpolationType::Hermite:
                return Interpolation::Hermite::Cubic(t, start.control.point, end.control.point, start.control.exit, end.control.enter);
            default:
                return start.control.point;
        }
    }

    vec3 Spline::InterpolateCombined(f32 t, const vec3* points, const i32 index, f32 alpha)
    {
        t = Math::Clamp(t, 0.0f, 1.0f);
        alpha = Math::Clamp(alpha, 0.0f, 1.0f);

        switch (_type)
        {
            case InterpolationType::BSpline:
                return Interpolation::BSpline::BasisSpline(t, points, index);
            case InterpolationType::CatmullRom:
                return Interpolation::CatmullRom::Base(t, alpha, points, index);
            case InterpolationType::CatmullRom_Uniform:
                return Interpolation::CatmullRom::Uniform(t, points, index);
            case InterpolationType::CatmullRom_Centripetal:
                return Interpolation::CatmullRom::Centripetal(t, points, index);
            case InterpolationType::CatmullRom_Chordal:
                return Interpolation::CatmullRom::Chordal(t, points, index);
            default:
                break;
        }

        vec3 result = *(points + index);
        return result;
    }

    void Spline::UpdatePoint(i32 position, SplinePoint newPoint)
    {
        if (position < 0 || position >= _points.size())
            return;

        _points[position] = newPoint;
    }
}
