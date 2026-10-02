#pragma once
#include <cmath>

struct vec3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	constexpr auto operator+(const vec3& rhs) const -> vec3 { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
	constexpr auto operator-(const vec3& rhs) const -> vec3 { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
	constexpr auto operator*(float s) const -> vec3 { return { x * s, y * s, z * s }; }
	constexpr auto operator==(const vec3& rhs) const -> bool = default;

	constexpr auto length_sq() const -> float { return x * x + y * y + z * z; }
	auto length() const -> float { return std::sqrt(length_sq()); }

	// 지면(x, y) 기준 거리
	constexpr auto length_2d_sq() const -> float { return x * x + y * y; }
	auto length_2d() const -> float { return std::sqrt(length_2d_sq()); }

	static constexpr auto distance_sq(const vec3& a, const vec3& b) -> float { return (a - b).length_sq(); }
	static constexpr auto distance_2d_sq(const vec3& a, const vec3& b) -> float { return (a - b).length_2d_sq(); }
};
