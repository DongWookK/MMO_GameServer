#pragma once
#include "pch.h"
#include "gobject.h"

class teleport : public gobject
{
public:
	teleport(map_id_t dest_map_id, const vec3& dest_pos);

public:
	auto get_dest_map_id() const -> map_id_t { return dest_map_id_; }
	auto get_dest_pos() const -> const vec3& { return dest_pos_; }

private:
	const map_id_t dest_map_id_;
	const vec3     dest_pos_;
};
