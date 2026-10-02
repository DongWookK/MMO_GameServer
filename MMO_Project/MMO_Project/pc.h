#pragma once
#include "pch.h"
#include "character.h"

class user;

class pc : public character
{
public:
	using pc_no_t = int64_t;
	using user_w_ptr_t = std::weak_ptr<user>;

public:
	pc(pc_no_t pc_no, std::wstring_view name);

public:
	auto get_pc_no() const -> pc_no_t { return pc_no_; }
	auto get_name() const -> const std::wstring& { return name_; }

	auto get_owner() const -> std::shared_ptr<user> { return owner_.lock(); }
	auto set_owner(const std::shared_ptr<user>& owner) -> void { owner_ = owner; }

private:
	const pc_no_t pc_no_;
	const std::wstring name_;
	user_w_ptr_t owner_{};
};
