#pragma once
#include "pch.h"
#include "gobject.h"

class item;
class drop_item : public gobject
{
public:
	using item_s_ptr_t = std::shared_ptr<item>;

public:
	drop_item(item_s_ptr_t item, object_id_t owner_id = invalid_object_id);

public:
	auto get_item() const -> const item_s_ptr_t& { return item_; }
	auto get_owner_id() const -> object_id_t { return owner_id_; }

	auto take_item() -> item_s_ptr_t { return std::move(item_); }

private:
	item_s_ptr_t item_{};
	object_id_t  owner_id_ = invalid_object_id;
};
