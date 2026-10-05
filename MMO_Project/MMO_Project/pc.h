#pragma once
#include "pch.h"
#include "character.h"

class user;

class pc : public character
{
public:
	using pc_no_t = int64_t;
	using pc_type_t = common::pc_type;
	using exp_t = int64_t;
	using user_w_ptr_t = std::weak_ptr<user>;

	static constexpr pc_no_t invalid_pc_no = 0;

public:
	pc(pc_no_t pc_no, std::wstring_view name, pc_type_t pc_type);

public:
	auto get_pc_no() const -> pc_no_t { return pc_no_; }
	auto get_name() const -> const std::wstring& { return name_; }
	auto get_pc_type() const -> pc_type_t { return pc_type_; }
	auto get_exp() const -> exp_t { return exp_; }

	auto set_exp(exp_t exp) -> void { exp_ = exp; }

	auto get_owner() const -> std::shared_ptr<user> { return owner_.lock(); }
	auto set_owner(const std::shared_ptr<user>& owner) -> void { owner_ = owner; }

private:
	const pc_no_t pc_no_;
	const std::wstring name_;
	const pc_type_t pc_type_;
	exp_t exp_ = 0;
	user_w_ptr_t owner_{};
};
