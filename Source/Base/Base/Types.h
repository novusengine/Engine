#pragma once
#include "Math/HalfFloat.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cstdint>

using i8  = std::int8_t;
using u8  = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using f16 = halffloat::half;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using f32 = float;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using f64 = double;
using StringRef = u32;

using vec2 = glm::vec2;
using ivec2 = glm::ivec2;
using uvec2 = glm::uvec2;
using hvec2 = glm::vec<2, f16>;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using uvec3 = glm::uvec3;
using hvec3 = glm::vec<3, f16>;
using u8vec4 = glm::vec<4, u8>;
using vec4 = glm::vec4;
using ivec4 = glm::ivec4;
using uvec4 = glm::uvec4;
using hvec4 = glm::vec<4, f16>;
using mat3x3 = glm::mat3x3;
using mat4x4 = glm::mat4x4;
using mat4a = glm::mat4x3;
using quat = glm::quat;

#define DECLARE_GENERIC_BITWISE_OPERATORS(T) inline const T operator~ (T a) { return (T)~(i32)a; } \
inline const T operator| (T a, T b) { return (T)((i32)a | (i32)b); } \
inline const T operator& (T a, T b) { return (T)((i32)a & (i32)b); } \
inline const T operator^ (T a, T b) { return (T)((i32)a ^ (i32)b); } \
inline const T& operator|= (T& a, T b) { return (T&)((i32&)a |= (i32)b); } \
inline const T& operator&= (T& a, T b) { return (T&)((i32&)a &= (i32)b); } \
inline const T& operator^= (T& a, T b) { return (T&)((i32&)a ^= (i32)b); }

// Define extra implicit conversions for ImGui vector classes
#define IM_VEC2_CLASS_EXTRA \
    constexpr ImVec2(glm::vec2& f) : x(f.x), y(f.y) {} \
    constexpr ImVec2(const glm::vec2& f) : x(f.x), y(f.y) {} \
    operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA \
    constexpr ImVec4(const glm::vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
    operator glm::vec4() const { return glm::vec4(x,y,z,w); }
