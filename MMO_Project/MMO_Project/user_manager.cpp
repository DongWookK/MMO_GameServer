#pragma once
#include "pch.h"
#include "user_manager.h"
#include "user_sql.h"
#include "pc_sql.h"
#include "map_manager.h"
#include "timer_manager.h"
#include "thread_local.h"
#include <cwctype>

auto user_manager::setup() -> fw::error
{
	fw::error error_code{};
	error_code = user_pool_.AllocateChunk<user>(
		[this]() { return std::make_unique<user>(); },			// 1. Create
		[](user* p, size_t i) { p->set_index(i); return 0; },	// 2. Init
		[](user* p) { p->reset(); },							// 3. UnAcquire 람다
		1000,													// 4. pInitSize
		false													// 5. pIsExpandable
	);
	ASSERT_RETURN_VALUE(!error_code, error_code);

	error_code = pc_pool_.AllocateChunk<pc>(
		[]() { return std::make_unique<pc>(); },
		[](pc* p, size_t i) { p->set_pool_index(static_cast<gobject::pool_index_t>(i)); return 0; },
		[](pc* p) {
			fw::timer_manager::instance()->cancel_all(p->get_object_id().value);	// 이 사용자의 남은 타이머가 다음 사용자에게 울리지 않도록
			p->on_release();
		},
		pc_pool_size,
		false
	);
	ASSERT_RETURN_VALUE(!error_code, error_code);

	return error_code;
}

auto user_manager::start() -> fw::error
{
	return fw::error{};
}

auto user_manager::stop() -> fw::error
{
	return fw::error{};
}

auto user_manager::teardown() -> fw::error
{
	return fw::error{};
}

auto user_manager::user_login(session_s_ptr_t session, std::wstring_view user_name) -> fw::expected<user_s_ptr_t>
{
	auto error = fw::error{};

	ASSERT_RETURN_VALUE(session != nullptr, fw::unexpected(error::code::session_invalid));

	if (!is_valid_name(user_name, max_user_name_len))
	{
		FLOG_WARN("user_login :: invalid user_name length({}) session({})", user_name.size(), session->get_index());
		return fw::unexpected(error::code::user_name_invalid);
	}

	if (find_user(session) != nullptr)
	{
		FLOG_WARN("user_login :: already login session({})", session->get_index());
		return fw::unexpected(error::code::user_already_login);
	}

	auto sql = fw::get_sql<user_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, fw::unexpected(error::code::sql_stmt_invalid));

	user_no_t user_no{};
	error = sql->user_login(user_name, user_no);
	ASSERT_RETURN_VALUE(!error, fw::unexpected(error));
	ASSERT_RETURN_VALUE(user_no != user_no_t{}, fw::unexpected(error::code::user_login_fail));

	auto user = user_pool_.AcquireObject();
	ASSERT_RETURN_VALUE(nullptr != user, fw::unexpected(error::code::object_acquire_fail));

	user->set_session(session);
	user->set_user_no(user_no);

	std::unique_lock lock(lock_);

	auto [it, inserted] = user_list_.insert(user);
	ASSERT_RETURN_VALUE(inserted, fw::unexpected(error::code::user_already_login));

	return *it;
}

auto user_manager::user_logout(session_s_ptr_t session) -> fw::error
{
	ASSERT_RETURN_VALUE(session != nullptr, error::code::session_invalid);

	user_s_ptr_t user{};
	pc_s_ptr_t ingame_pc{};
	{
		std::shared_lock lock(lock_);
		user = find_user_nolock(session->get_index());
		if (user != nullptr)
		{
			ingame_pc = user->get_pc();
		}
	}

	if (user == nullptr)
	{
		FLOG_WARN("user_logout :: not login session({})", session->get_index());
		return error::code::user_not_exist;
	}

	if (ingame_pc != nullptr)
	{
		// 이동 중이면 현재 위치를 확정하고 멈춘 뒤 저장
		ingame_pc->stop_move(character::clock_t::now());

		auto save_error = pc_save(user, ingame_pc);
		if (save_error)
		{
			FLOG_ERROR("user_logout :: pc save fail pc({}) user({}) error({})"
				, ingame_pc->get_pc_no(), user->get_user_no(), save_error.value());
		}

		if (ingame_pc->is_spawned())
		{
			auto leave_error = map_manager::instance()->exit_map(ingame_pc);
			if (leave_error)
			{
				FLOG_ERROR("user_logout :: leave map fail pc({}) user({}) error({})"
					, ingame_pc->get_pc_no(), user->get_user_no(), leave_error.value());
			}
		}

		fw::timer_manager::instance()->cancel_all(ingame_pc->get_object_id().value);
	}

	{
		std::unique_lock lock(lock_);

		if (ingame_pc != nullptr)
		{
			pc_list_.get<tag_object_id>().erase(ingame_pc->get_object_id());
		}

		auto& index = user_list_.get<tag_session>();
		auto it = index.find(session->get_index());
		if (it != index.end() && *it == user)
		{
			index.erase(it);
		}
	}

	return fw::error{};
}

auto user_manager::pc_save(const user_s_ptr_t& user, const pc_s_ptr_t& pc) -> fw::error
{
	ASSERT_RETURN_VALUE(user != nullptr, error::code::user_not_exist);
	ASSERT_RETURN_VALUE(pc != nullptr, error::code::pc_not_exist);

	auto sql = fw::get_sql<pc_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, error::code::sql_stmt_invalid);

	const auto& pos = pc->get_pos();
	pc_sql::save_param param{};
	param.user_no = user->get_user_no();
	param.pc_no = pc->get_pc_no();
	param.level = pc->get_level();
	param.exp = pc->get_exp();
	param.hp = pc->get_hp();
	param.mp = pc->get_mp();
	param.map_no = static_cast<int32_t>(pc->get_map_id());
	param.location_x = pos.x;
	param.location_y = pos.y;
	param.location_z = pos.z;

	pc_sql::save_result result{};
	auto error = sql->pc_save_update(param, result);
	ASSERT_RETURN_VALUE(!error, error);
	ASSERT_RETURN_VALUE(result == pc_sql::save_result::ok, error::code::pc_not_exist);

	return error;
}

auto user_manager::find_user(session_s_ptr_t session) const -> user_s_ptr_t
{
	if (session == nullptr)
	{
		return nullptr;
	}

	std::shared_lock lock(lock_);
	return find_user_nolock(session->get_index());
}

auto user_manager::find_user_nolock(size_t session_index) const -> user_s_ptr_t
{
	auto& index = user_list_.get<tag_session>();
	auto it = index.find(session_index);
	if (it == index.end())
	{
		return nullptr;
	}

	return *it;
}

auto user_manager::find_pc_by_object_id(gobject::object_id_t object_id) const -> pc_s_ptr_t
{
	std::shared_lock lock(lock_);

	auto& index = pc_list_.get<tag_object_id>();
	auto it = index.find(object_id);
	if (it == index.end())
	{
		return nullptr;
	}

	return *it;
}

auto user_manager::find_pc_by_pc_no(pc::pc_no_t pc_no) const -> pc_s_ptr_t
{
	std::shared_lock lock(lock_);

	auto& index = pc_list_.get<tag_pc_no>();
	auto it = index.find(pc_no);
	if (it == index.end())
	{
		return nullptr;
	}

	return *it;
}

auto user_manager::find_pc_by_name(std::wstring_view pc_name) const -> pc_s_ptr_t
{
	const std::wstring key{ pc_name };

	std::shared_lock lock(lock_);

	auto& index = pc_list_.get<tag_pc_name>();
	auto it = index.find(key);
	if (it == index.end())
	{
		return nullptr;
	}

	return *it;
}

auto user_manager::find_pc_by_session(session_s_ptr_t session) const -> pc_s_ptr_t
{
	if (session == nullptr)
	{
		return nullptr;
	}

	std::shared_lock lock(lock_);

	auto user = find_user_nolock(session->get_index());
	return user != nullptr ? user->get_pc() : nullptr;	// user::pc_ 는 lock_ 안에서 복사
}

auto user_manager::pc_list(session_s_ptr_t session) -> fw::expected<std::vector<pc_summary>>
{
	auto error = fw::error{};

	ASSERT_RETURN_VALUE(session != nullptr, fw::unexpected(error::code::session_invalid));

	auto user = find_user(session);
	if (user == nullptr)
	{
		FLOG_WARN("pc_list :: not login session({})", session->get_index());
		return fw::unexpected(error::code::user_not_exist);
	}

	auto sql = fw::get_sql<pc_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, fw::unexpected(error::code::sql_stmt_invalid));

	nanodbc::result result;
	error = sql->pc_list_select(user->get_user_no(), result);
	ASSERT_RETURN_VALUE(!error, fw::unexpected(error));

	std::vector<pc_summary> pc_list{};
	try
	{
		while (result.next())
		{
			pc_summary summary{};
			summary.pc_no = result.get<int64_t>(NANODBC_TEXT("pc_no"));
			summary.pc_name = result.get<std::wstring>(NANODBC_TEXT("pc_name"));
			summary.pc_type = static_cast<common::pc_type>(result.get<uint8_t>(NANODBC_TEXT("pc_type")));
			summary.level = result.get<int32_t>(NANODBC_TEXT("level"));
			pc_list.push_back(std::move(summary));
		}
	}
	catch (const std::exception& e)
	{
		FLOG_ERROR("pc_list :: result read fail user({}) -> {}", user->get_user_no(), e.what());
		return fw::unexpected(error::code::sql_fail);
	}

	return pc_list;
}

auto user_manager::pc_create(session_s_ptr_t session, std::wstring_view pc_name, common::pc_type pc_type) -> fw::expected<pc::pc_no_t>
{
	auto error = fw::error{};

	ASSERT_RETURN_VALUE(session != nullptr, fw::unexpected(error::code::session_invalid));

	user_s_ptr_t user{};
	pc_s_ptr_t selected_pc{};
	{
		std::shared_lock lock(lock_);
		user = find_user_nolock(session->get_index());
		if (user != nullptr)
		{
			selected_pc = user->get_pc();
		}
	}

	if (user == nullptr)
	{
		FLOG_WARN("pc_create :: not login session({})", session->get_index());
		return fw::unexpected(error::code::user_not_exist);
	}

	if (selected_pc != nullptr)
	{
		FLOG_WARN("pc_create :: already selected pc user({})", user->get_user_no());
		return fw::unexpected(error::code::pc_already_selected);
	}

	if (!is_valid_name(pc_name, max_pc_name_len))
	{
		FLOG_WARN("pc_create :: invalid pc_name length({}) user({})", pc_name.size(), user->get_user_no());
		return fw::unexpected(error::code::pc_name_invalid);
	}

	if (pc_type < common::pc_type::MIN || pc_type > common::pc_type::MAX)
	{
		FLOG_WARN("pc_create :: invalid pc_type({}) user({})", fw::to_underlying(pc_type), user->get_user_no());
		return fw::unexpected(error::code::pc_type_invalid);
	}

	auto sql = fw::get_sql<pc_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, fw::unexpected(error::code::sql_stmt_invalid));

	pc::pc_no_t pc_no{ pc::invalid_pc_no };
	pc_sql::create_result result{};
	error = sql->pc_create_insert(user->get_user_no(), pc_name, fw::to_underlying(pc_type), result, pc_no);
	ASSERT_RETURN_VALUE(!error, fw::unexpected(error));

	if (result == pc_sql::create_result::name_duplicate)
	{
		FLOG_WARN("pc_create :: duplicate pc_name length({}) user({})", pc_name.size(), user->get_user_no());
		return fw::unexpected(error::code::pc_name_duplicate);
	}

	ASSERT_RETURN_VALUE(result == pc_sql::create_result::ok, fw::unexpected(error::code::pc_create_fail));
	ASSERT_RETURN_VALUE(pc_no != pc::invalid_pc_no, fw::unexpected(error::code::pc_create_fail));

	return pc_no;
}

auto user_manager::pc_select(session_s_ptr_t session, pc::pc_no_t pc_no) -> fw::expected<pc_s_ptr_t>
{
	auto error = fw::error{};

	ASSERT_RETURN_VALUE(session != nullptr, fw::unexpected(error::code::session_invalid));

	user_s_ptr_t user{};
	pc_s_ptr_t selected_pc{};
	{
		std::shared_lock lock(lock_);
		user = find_user_nolock(session->get_index());
		if (user != nullptr)
		{
			selected_pc = user->get_pc();
		}
	}

	if (user == nullptr)
	{
		FLOG_WARN("pc_select :: not login session({})", session->get_index());
		return fw::unexpected(error::code::user_not_exist);
	}

	if (selected_pc != nullptr)
	{
		FLOG_WARN("pc_select :: already selected pc({}) user({})", selected_pc->get_pc_no(), user->get_user_no());
		return fw::unexpected(error::code::pc_already_selected);
	}

	if (find_pc_by_pc_no(pc_no) != nullptr)
	{
		FLOG_WARN("pc_select :: pc({}) already in game user({})", pc_no, user->get_user_no());
		return fw::unexpected(error::code::pc_already_in_game);
	}

	auto sql = fw::get_sql<pc_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, fw::unexpected(error::code::sql_stmt_invalid));

	nanodbc::result result;
	error = sql->pc_login_select(user->get_user_no(), pc_no, result);
	ASSERT_RETURN_VALUE(!error, fw::unexpected(error));

	pc_s_ptr_t new_pc{};
	gobject::map_no_t map_no = gobject::invalid_map_id;
	try
	{
		if (result.next())
		{
			const auto pc_name = result.get<std::wstring>(NANODBC_TEXT("pc_name"));
			const auto pc_type = static_cast<common::pc_type>(result.get<uint8_t>(NANODBC_TEXT("pc_type")));
			const auto hp = result.get<int32_t>(NANODBC_TEXT("hp"));
			const auto mp = result.get<int32_t>(NANODBC_TEXT("mp"));

			new_pc = pc_pool_.AcquireObject();
			if (new_pc == nullptr)
			{
				FLOG_ERROR("pc_select :: pc pool exhausted pc({}) user({})", pc_no, user->get_user_no());
				return fw::unexpected(error::code::object_acquire_fail);
			}

			new_pc->init(pc_no, pc_name, pc_type);
			new_pc->set_level(result.get<int32_t>(NANODBC_TEXT("level")));
			new_pc->set_exp(result.get<int64_t>(NANODBC_TEXT("exp")));

			new_pc->set_max_hp(hp);
			new_pc->set_hp(hp);
			new_pc->set_max_mp(mp);
			new_pc->set_mp(mp);
			new_pc->set_move_speed(character::default_move_speed);

			map_no = static_cast<gobject::map_no_t>(result.get<int32_t>(NANODBC_TEXT("map_no")));

			// DB FLOAT == double
			new_pc->set_pos(vec3{
				static_cast<float>(result.get<double>(NANODBC_TEXT("location_x"))),
				static_cast<float>(result.get<double>(NANODBC_TEXT("location_y"))),
				static_cast<float>(result.get<double>(NANODBC_TEXT("location_z"))) });
		}
	}
	catch (const std::exception& e)
	{
		FLOG_ERROR("pc_select :: result read fail pc({}) user({}) -> {}", pc_no, user->get_user_no(), e.what());
		return fw::unexpected(error::code::sql_fail);
	}

	if (new_pc == nullptr)
	{
		FLOG_WARN("pc_select :: pc not exist pc({}) user({})", pc_no, user->get_user_no());
		return fw::unexpected(error::code::pc_not_exist);
	}

	new_pc->set_owner(user);

	{
		std::unique_lock lock(lock_);

		if (find_user_nolock(session->get_index()) != user)
		{
			FLOG_WARN("pc_select :: user logged out during select pc({}) user({})", pc_no, user->get_user_no());
			return fw::unexpected(error::code::user_not_exist);
		}

		if (user->get_pc() != nullptr)
		{
			FLOG_WARN("pc_select :: already selected pc({}) user({})", user->get_pc()->get_pc_no(), user->get_user_no());
			return fw::unexpected(error::code::pc_already_selected);
		}

		auto [it, inserted] = pc_list_.insert(new_pc);
		ASSERT_RETURN_VALUE(inserted, fw::unexpected(error::code::pc_already_in_game));

		user->set_pc(new_pc);
	}

	error = map_manager::instance()->enter_map(new_pc, map_no, new_pc->get_pos(), new_pc->get_heading());
	if (error)
	{
		FLOG_ERROR("pc_select :: enter map fail pc({}) user({}) map({}) error({})", pc_no, user->get_user_no(), map_no, error.value());
		rollback_pc_select(user, new_pc);
		return fw::unexpected(error);
	}

	bool still_selected = false;
	{
		std::shared_lock lock(lock_);
		still_selected = (find_user_nolock(session->get_index()) == user && user->get_pc() == new_pc);
	}

	if (!still_selected)
	{
		FLOG_WARN("pc_select :: user logged out during enter map pc({}) user({})", pc_no, user->get_user_no());
		if (new_pc->is_spawned())
		{
			map_manager::instance()->exit_map(new_pc);
		}
		rollback_pc_select(user, new_pc);
		return fw::unexpected(error::code::user_not_exist);
	}

	FLOG_INFO("pc_select :: pc({}) object({}) enter map({}) pos({}, {}, {})", pc_no, new_pc->get_object_id().value, new_pc->get_map_id(),
		new_pc->get_pos().x, new_pc->get_pos().y, new_pc->get_pos().z);
	return new_pc;
}

auto user_manager::rollback_pc_select(const user_s_ptr_t& user, const pc_s_ptr_t& pc) -> void
{
	std::unique_lock lock(lock_);

	pc_list_.get<tag_object_id>().erase(pc->get_object_id());
	if (user->get_pc() == pc)
	{
		user->set_pc(nullptr);
	}
}

auto user_manager::is_valid_name(std::wstring_view name, size_t max_len) -> bool
{
	if (name.empty() || name.size() > max_len)
	{
		return false;
	}

	for (const auto ch : name)
	{
		if (std::iswcntrl(ch))
		{
			return false;
		}
	}

	return true;
}
