#pragma once
#include "pch.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include "object_pool.hpp"
#include "user.h"

using namespace boost::multi_index;
class user_manager : public feature, public singleton<user_manager>
{
	friend class singleton<user_manager>;

private:
	// feature("user_manager") 매개변수 생성자 호출
	user_manager()
		: feature("user_manager")
	{}
	~user_manager() override = default;

public:
	using pool_t = fw::CObjectPool<user>;
	using object_t = pool_t::Object;
	using user_s_ptr_t = object_t;
	using session_s_ptr_t = std::shared_ptr<session>;

	struct tag_user_no{};
	struct tag_key{};

	typedef multi_index_container<
		user_s_ptr_t,
		indexed_by<
		ordered_unique<tag<tag_key>,const_mem_fun<user, const size_t, &user::get_index>>,
		hashed_non_unique<tag<tag_user_no>, const_mem_fun<user, const int32_t, &user::get_user_no>>
		>
	> login_user_container;

	auto setup() -> fw::error override;
	auto start() -> fw::error override;
	auto stop() -> fw::error override;
	auto teardown() -> fw::error override;

public:
	auto user_login(session_s_ptr_t session) -> user_s_ptr_t;
	auto user_logout(session_s_ptr_t session) -> fw::error;
	auto find_user(session_s_ptr_t session) const -> user_s_ptr_t;

private:

private:
	pool_t user_pool_{};
	login_user_container user_list_{};
};