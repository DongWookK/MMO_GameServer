#pragma once
#include "pch.h"
#include "item.h"

item::item(const item_info& info, item_status status)
	: info_(info)
	, status_(std::move(status))
{
}
