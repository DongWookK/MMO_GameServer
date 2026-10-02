#pragma once
#include "pch.h"
#include "pc.h"
#include "user.h"

pc::pc(pc_no_t pc_no, std::wstring_view name)
	: character(common::object_type::pc)
	, pc_no_(pc_no)
	, name_(name)
{
}
