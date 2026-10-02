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
