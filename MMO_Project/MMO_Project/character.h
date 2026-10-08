#pragma once
#include "pch.h"
#include "gobject.h"

// todo : 이동, fsm (behavior 접목시킬 예정)
class character : public gobject
{
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

protected:
	explicit character(common::object_type type) : gobject(type) {}

private:
	int32_t level_ = 1;
	int32_t hp_ = 0;
	int32_t max_hp_ = 0;
	int32_t mp_ = 0;
	int32_t max_mp_ = 0;
	float   move_speed_ = 0.f;	// cm/s
};
