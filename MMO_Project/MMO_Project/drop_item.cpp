#pragma once
#include "pch.h"
#include "drop_item.h"
#include "item.h"

drop_item::drop_item(item_s_ptr_t item, object_id_t owner_id)
	: gobject(common::object_type::drop_item)
	, item_(std::move(item))
	, owner_id_(owner_id)
{
}
