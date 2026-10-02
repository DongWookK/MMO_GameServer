#pragma once
#include "pch.h"
#include "monster.h"

monster::monster(object_no_t object_no)
	: npc(common::object_type::monster, object_no)
{
}
