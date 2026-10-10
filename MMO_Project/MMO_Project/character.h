#pragma once
#include "pch.h"
#include <chrono>
#include "gobject.h"

// todo : fsm (behavior 접목시킬 예정)
class character : public gobject
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;

	static constexpr float default_move_speed = 600.f;

public:
	auto is_movable() const -> bool override { return true; }

public:
	auto is_dead() const -> bool { return hp_ <= 0; }

	auto get_level() const -> int32_t { return level_; }
	auto get_hp() const -> int32_t { return hp_; }
	auto get_max_hp() const -> int32_t { return max_hp_; }
	auto get_mp() const -> int32_t { return mp_; }
	auto get_max_mp() const -> int32_t { return max_mp_; }
	auto get_move_speed() const -> float { return move_speed_; }

	auto set_level(int32_t level) -> void { level_ = level; }
	auto set_max_hp(int32_t max_hp) -> void;
	auto set_max_mp(int32_t max_mp) -> void;
	auto set_hp(int32_t hp) -> void;
	auto set_mp(int32_t mp) -> void;
	auto set_move_speed(float move_speed) -> void { move_speed_ = move_speed; }

	auto on_release() -> void override;

public:
	auto start_move(std::vector<vec3> path, time_point_t now) -> void;	// path[0] = 현재 위치
	auto stop_move(time_point_t now) -> void;
	auto update_move(time_point_t now) -> const vec3&;					// 현재 위치 확정 (도착했으면 이동 종료)

	auto is_moving() const -> bool { return !move_path_.empty(); }
	auto get_move_path() const -> const std::vector<vec3>& { return move_path_; }
	auto get_last_move_request() const -> time_point_t { return last_move_request_; }
	auto set_last_move_request(time_point_t now) -> void { last_move_request_ = now; }

protected:
	explicit character(common::object_type type) : gobject(type) {}

private:
	auto clear_move() -> void;
	auto position_at(time_point_t now) const -> vec3;

private:
	int32_t level_ = 1;
	int32_t hp_ = 0;
	int32_t max_hp_ = 0;
	int32_t mp_ = 0;
	int32_t max_mp_ = 0;
	float   move_speed_ = 0.f;	// cm/s

	std::vector<vec3>  move_path_{};
	std::vector<float> move_path_dist_{};
	time_point_t       move_start_{};
	time_point_t       last_move_request_{};
};
