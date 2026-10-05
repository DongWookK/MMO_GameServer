#pragma once
#include "pch.h"
#include "pc.h"
#include "user.h"

pc::pc(pc_no_t pc_no, std::wstring_view name, pc_type_t pc_type)
	: character(common::object_type::pc)
	, pc_no_(pc_no)
	, name_(name)
	, pc_type_(pc_type)
{
}
