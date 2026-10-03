#pragma once
#include "pch.h"

class item_info
{
public:
	using no_t = uint32_t;
	using type_t = common::item_type;

private:
	item_info() = default;

public:
	explicit item_info(no_t item_no, type_t type);

	static auto get_default() -> const item_info&;
	auto is_default() const -> bool;

public:
	auto get_item_no() const -> no_t { return item_no_; }
	auto get_type() const -> type_t { return type_; }

private:
	no_t   item_no_{};
	type_t type_{};
};
