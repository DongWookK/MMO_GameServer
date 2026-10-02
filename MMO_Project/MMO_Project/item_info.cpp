#include "pch.h"
#include "item_info.h"

item_info::item_info(no_t item_no
					 , type_t type)
	: item_no_(item_no)
	, type_(type)
{}

auto item_info::get_default() -> const item_info& 
{
	static item_info default_instance{};
	return default_instance;
}

auto item_info::is_default() const -> bool 
{
	return this == &get_default();
}