#pragma once
#include "pch.h"
#include "pc.h"
#include "user.h"

pc::pc()
	: character(common::object_type::pc)
{
}

auto pc::init(pc_no_t pc_no, std::wstring_view name, pc_type_t pc_type) -> void
{
	pc_no_ = pc_no;
	name_ = name;
	pc_type_ = pc_type;
}

auto pc::on_release() -> void
{
	pc_no_ = invalid_pc_no;
	name_.clear();
	pc_type_ = pc_type_t{};
	exp_ = 0;
	owner_.reset();

	character::on_release();
}
