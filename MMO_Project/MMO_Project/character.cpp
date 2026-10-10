#pragma once
#include "pch.h"
#include "character.h"

auto character::set_max_hp(int32_t max_hp) -> void
{
	max_hp_ = (std::max)(max_hp, 0);
	hp_ = (std::min)(hp_, max_hp_);
}

auto character::set_max_mp(int32_t max_mp) -> void
{
	max_mp_ = (std::max)(max_mp, 0);
	mp_ = (std::min)(mp_, max_mp_);
}

auto character::set_hp(int32_t hp) -> void
{
	hp_ = std::clamp(hp, 0, max_hp_);
}

auto character::set_mp(int32_t mp) -> void
{
	mp_ = std::clamp(mp, 0, max_mp_);
}

auto character::on_release() -> void
{
	level_ = 1;
	hp_ = 0;
	max_hp_ = 0;
	mp_ = 0;
	max_mp_ = 0;
	move_speed_ = 0.f;
	clear_move();
	last_move_request_ = {};

	gobject::on_release();
}

auto character::start_move(std::vector<vec3> path, time_point_t now) -> void
{
	clear_move();
	if (path.size() < 2 || move_speed_ <= 0.f)
	{
		return;
	}

	move_path_dist_.reserve(path.size());
	move_path_dist_.push_back(0.f);
	for (size_t i = 1; i < path.size(); ++i)
	{
		move_path_dist_.push_back(move_path_dist_.back() + (path[i] - path[i - 1]).length());
	}

	for (size_t i = 1; i < path.size(); ++i)
	{
		const vec3 dir = path[i] - path[0];
		if (dir.length_2d_sq() > 1.f)
		{
			constexpr float rad_to_deg = 57.29577951f;
			set_heading(std::atan2(dir.y, dir.x) * rad_to_deg);
			break;
		}
	}

	set_pos(path.front());
	move_path_ = std::move(path);
	move_start_ = now;
}

auto character::stop_move(time_point_t now) -> void
{
	update_move(now);
	clear_move();
}

auto character::update_move(time_point_t now) -> const vec3&
{
	if (!is_moving())
	{
		return get_pos();
	}

	set_pos(position_at(now));

	const float elapsed = std::chrono::duration<float>(now - move_start_).count();
	if (elapsed * move_speed_ >= move_path_dist_.back())
	{
		clear_move();
	}

	return get_pos();
}

auto character::position_at(time_point_t now) const -> vec3
{
	const float elapsed = (std::max)(std::chrono::duration<float>(now - move_start_).count(), 0.f);
	const float travelled = elapsed * move_speed_;

	if (travelled >= move_path_dist_.back())
	{
		return move_path_.back();
	}

	const auto it = std::upper_bound(move_path_dist_.begin(), move_path_dist_.end(), travelled);
	const size_t i = static_cast<size_t>(std::distance(move_path_dist_.begin(), it));

	const float seg_len = move_path_dist_[i] - move_path_dist_[i - 1];
	const float t = seg_len > 0.f ? (travelled - move_path_dist_[i - 1]) / seg_len : 1.f;

	// 보간 : 시작점 + (끝점 - 시작점) × 비율
	return move_path_[i - 1] + (move_path_[i] - move_path_[i - 1]) * t;
}

auto character::clear_move() -> void
{
	move_path_.clear();
	move_path_dist_.clear();
}
