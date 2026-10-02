#pragma once
#include "pch.h"
#include "npc.h"

class monster : public npc
{
public:
	explicit monster(object_no_t object_no);

public:
	auto is_attackable() const -> bool override { return true; }

	auto get_exp_reward() const -> int64_t { return exp_reward_; }
	auto set_exp_reward(int64_t exp) -> void { exp_reward_ = exp; }

private:
	int64_t exp_reward_ = 0;
};
