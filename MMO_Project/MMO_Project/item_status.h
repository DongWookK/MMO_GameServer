#pragma once
#include "pch.h"

class item_status
{
public:
	using serial_no_t = uint64_t;
	using count_t = int64_t;

	static constexpr serial_no_t invalid_serial_no = 0;

public:
	item_status() = default;
	item_status(serial_no_t serial_no, count_t count);

public:
	auto is_valid() const -> bool { return serial_no_ != invalid_serial_no; }

	auto get_serial_no() const -> serial_no_t { return serial_no_; }
	auto get_count() const -> count_t { return count_; }

private:
	serial_no_t serial_no_ = invalid_serial_no;
	count_t     count_ = 0;
};
