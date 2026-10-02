#pragma once
#include "pch.h"
#include "character.h"

class npc : public character
{
public:
	using object_no_t = uint32_t;

public:
	explicit npc(object_no_t template_id);

public:
	virtual auto is_attackable() const -> bool { return false; }

	auto get_template_id() const -> object_no_t { return object_no_; }
	auto get_home_pos() const -> const vec3& { return home_pos_; }

protected:
	npc(common::object_type type, object_no_t object_no);

	auto on_spawn() -> void override;

private:
	const object_no_t object_no_;
	vec3 home_pos_{}; // 스폰위치
};
