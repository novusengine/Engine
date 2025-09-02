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

class Bytebuffer;
struct ObjectGUID
{
public:
    enum class Type : u8
    {
        None,
        Player,
        Creature,
        GameObject,
        Item,
        Count
    };

    ObjectGUID() : _data(0) {}
    ObjectGUID(Type type, u64 counter)
    {
        _data = (static_cast<u64>(type) << 59) | (counter & COUNTER_MASK);
    }
    static ObjectGUID FromU64(u64 rawValue);
    static ObjectGUID CreatePlayer(u64 counter);
    static ObjectGUID CreateCreature(u64 counter);
    static ObjectGUID CreateGameObject(u64 counter);
    static ObjectGUID CreateItem(u64 counter);

    Type GetType() const { return static_cast<Type>(_data >> 59); }
    u64 GetCounter() const { return _data & COUNTER_MASK; }
    u64 GetData() const { return _data; }
    bool IsValid() const { return GetType() != Type::None; }
    bool IsSame(const ObjectGUID& other) const { return GetType() == other.GetType(); }

    /**
    * Calculates the minimum number of bytes needed to represent a counter value.
    * The function determines this by finding the position of the highest set bit,
    * effectively finding the smallest number of bytes that can contain the value.
    *
    * @return Number of bytes (0-7) needed to represent the counter value:
    *         - Returns 0 if counter is 0
    *         - Returns 1-7 based on minimum bytes needed
    */
    u8 GetCounterBytesUsed() const;

public:
    static const ObjectGUID Empty;
    static constexpr u64 TYPE_MASK = 0xF800000000000000;
    static constexpr u64 COUNTER_MASK = 0x07FFFFFFFFFFFFFF;

    std::strong_ordering operator<=>(const ObjectGUID& other) const
    {
        return _data <=> other._data;
    }

    // Then, the other comparison operators can be very simply defined:
    bool operator==(const ObjectGUID& other) const { return (*this <=> other) == std::strong_ordering::equal; }
    bool operator!=(const ObjectGUID& other) const { return !(*this == other); }
    bool operator<(const ObjectGUID& other) const { return (*this <=> other) == std::strong_ordering::less; }
    bool operator>(const ObjectGUID& other) const { return (*this <=> other) == std::strong_ordering::greater; }
    bool operator<=(const ObjectGUID& other) const { return (*this <=> other) != std::strong_ordering::greater; }
    bool operator>=(const ObjectGUID& other) const { return (*this <=> other) != std::strong_ordering::less; }

    std::string ToString() const;
    static std::string TypeToString(Type type);

    bool Deserialize(Bytebuffer* buffer);
    bool Serialize(Bytebuffer* buffer) const;

private:
    ObjectGUID(u64 data);

private:
    u64 _data;

    static_assert(TYPE_MASK == 0xF800000000000000, "TYPE_MASK is incorrect");
    static_assert(COUNTER_MASK == 0x07FFFFFFFFFFFFFF, "COUNTER_MASK is incorrect");
};

namespace std // You can specialize std::hash in the std namespace
{
    template <>
    struct hash<ObjectGUID>
    {
        size_t operator()(const ObjectGUID& og) const
        {
            return std::hash<u64>()(og.GetData());
        }
    };
}