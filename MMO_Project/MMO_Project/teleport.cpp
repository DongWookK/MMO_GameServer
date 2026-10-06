#pragma once
#include "pch.h"
#include "teleport.h"

teleport::teleport(map_no_t dest_map_id, const vec3& dest_pos)
	: gobject(common::object_type::teleport)
	, dest_map_id_(dest_map_id)
	, dest_pos_(dest_pos)
{
}
