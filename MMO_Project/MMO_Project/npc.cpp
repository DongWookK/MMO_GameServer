#pragma once
#include "pch.h"
#include "npc.h"

npc::npc(object_no_t object_no)
	: npc(common::object_type::npc, object_no)
{
}

npc::npc(common::object_type type, object_no_t object_no)
	: character(type)
	, object_no_(object_no)
{
}

auto npc::on_spawn() -> void
{
	home_pos_ = get_pos();
}
