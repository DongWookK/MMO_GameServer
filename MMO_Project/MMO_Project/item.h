#pragma once
#include "pch.h"
#include "item_info.h"
#include "item_status.h"

class item
{
public:
	item(const item_info& info, item_status status);

public:
	auto get_info() const -> const item_info& { return info_.get(); }
	auto get_status() const -> const item_status& { return status_; }
	auto get_status() -> item_status& { return status_; }

	auto get_item_no() const -> item_info::no_t { return info_.get().get_item_no(); }
	auto get_serial_no() const -> item_status::serial_no_t { return status_.get_serial_no(); }

private:
	std::reference_wrapper<const item_info> info_;
	item_status status_{};
};
