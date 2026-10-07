#pragma once
#include "pch.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <shared_mutex>

#include "object_pool.hpp"
#include "user.h"
#include "pc.h"

using namespace boost::multi_index;
class user_manager : public feature, public singleton<user_manager>
{
	friend class singleton<user_manager>;

public:
	static constexpr size_t max_user_name_len = 20;
	static constexpr size_t max_pc_name_len = 20;

public:
	user_manager()
		: feature("user_manager")
	{}
	~user_manager() override = default;

public:
	using pool_t = fw::CObjectPool<user>;
	using object_t = pool_t::Object;
	using user_s_ptr_t = object_t;
	using session_s_ptr_t = std::shared_ptr<session>;
	using pc_s_ptr_t = user::pc_s_ptr_t;
	using user_no_t = int32_t;

	struct tag_user_no{};
	struct tag_session{};

	typedef multi_index_container<
		user_s_ptr_t,
		indexed_by<
		hashed_unique<tag<tag_session>, const_mem_fun<user, const size_t, &user::get_session_index>>,
		hashed_non_unique<tag<tag_user_no>, const_mem_fun<user, const int32_t, &user::get_user_no>>
		>
	> login_user_container;

	struct tag_object_id{};
	struct tag_pc_no{};
	struct tag_pc_name{};

	typedef multi_index_container<
		pc_s_ptr_t,
		indexed_by<
		hashed_unique<tag<tag_object_id>, const_mem_fun<gobject, gobject::object_id_t, &gobject::get_object_id>>,
		hashed_unique<tag<tag_pc_no>, const_mem_fun<pc, pc::pc_no_t, &pc::get_pc_no>>,
		hashed_unique<tag<tag_pc_name>, const_mem_fun<pc, const std::wstring&, &pc::get_name>>
		>
	> pc_container;

	struct pc_summary
	{
		pc::pc_no_t pc_no{};
		std::wstring pc_name{};
		common::pc_type pc_type{};
		int32_t level{};
	};

	auto setup() -> fw::error override;
	auto start() -> fw::error override;
	auto stop() -> fw::error override;
	auto teardown() -> fw::error override;

public:
	auto user_login(session_s_ptr_t session, std::wstring_view user_name) -> fw::expected<user_s_ptr_t>;
	auto user_logout(session_s_ptr_t session) -> fw::error;
	auto find_user(session_s_ptr_t session) const -> user_s_ptr_t;

	auto find_pc_by_object_id(gobject::object_id_t object_id) const -> pc_s_ptr_t;
	auto find_pc_by_pc_no(pc::pc_no_t pc_no) const -> pc_s_ptr_t;
	auto find_pc_by_name(std::wstring_view pc_name) const -> pc_s_ptr_t;

	template <typename T>
	auto broadcast(flatbuffers::FlatBufferBuilder& builder, flatbuffers::Offset<T> offset) -> void
	{
		builder.Finish(offset);
		const auto packet_type = std::to_underlying(PacketTraits<T>::type);

		std::vector<session_s_ptr_t> targets{};
		{
			std::shared_lock lock(lock_);
			targets.reserve(pc_list_.size());

			for (const auto& ingame_pc : pc_list_)
			{
				auto owner = ingame_pc->get_owner();
				if (owner == nullptr || owner->get_session() == nullptr)
				{
					continue;
				}

				targets.push_back(owner->get_session());
			}
		}

		for (const auto& target : targets)
		{
			target->send(packet_type, builder);
		}
	}

	auto get_ingame_pc_count() const -> size_t
	{
		std::shared_lock lock(lock_);
		return pc_list_.size();
	}

	auto pc_list(session_s_ptr_t session) -> fw::expected<std::vector<pc_summary>>;
	auto pc_create(session_s_ptr_t session, std::wstring_view pc_name, common::pc_type pc_type) -> fw::expected<pc::pc_no_t>;
	auto pc_select(session_s_ptr_t session, pc::pc_no_t pc_no) -> fw::expected<pc_s_ptr_t>;

private:
	auto pc_save(const user_s_ptr_t& user, const pc_s_ptr_t& pc) -> fw::error;
	auto rollback_pc_select(const user_s_ptr_t& user, const pc_s_ptr_t& pc) -> void;

	// lock_ 을 이미 잡은 상태에서만 호출
	auto find_user_nolock(size_t session_index) const -> user_s_ptr_t;

	static auto is_valid_name(std::wstring_view name, size_t max_len) -> bool;

private:
	mutable std::shared_mutex lock_{};
	
	pool_t user_pool_{};
	login_user_container user_list_{};
	pc_container pc_list_{};
};