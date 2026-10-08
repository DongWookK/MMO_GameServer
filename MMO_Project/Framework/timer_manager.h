#pragma once
#include "pch.h"
#include <map>
#include <mutex>
#include "boost/asio.hpp"

class session;

namespace fw
{
class timer_manager : public singleton<timer_manager>
{
	friend class singleton<timer_manager>;

public:
	using object_id_t = int32_t;
	using duration_t = std::chrono::milliseconds;
	using task_t = std::function<void()>;
	using session_s_ptr_t = std::shared_ptr<session>;

private:
	struct key_t
	{
		object_id_t object_id{};
		std::string name{};

		auto operator<=>(const key_t&) const = default;
	};

	struct entry_t
	{
		explicit entry_t(boost::asio::io_context& io_context) : timer(io_context) {}

		boost::asio::steady_timer timer;
		task_t task{};
		bool use_session = false;
		std::weak_ptr<session> sess{};
	};
	using entry_s_ptr_t = std::shared_ptr<entry_t>;

public:
	using object_id_t = int32_t;
	using duration_t = std::chrono::milliseconds;
	using task_t = std::function<void()>;
	using session_s_ptr_t = std::shared_ptr<session>;

public:
	auto setup(boost::asio::io_context* io_context) -> void;
	auto stop() -> void;

	auto add(std::string_view name, object_id_t object_id, duration_t delay, task_t task) -> void;
	auto add(std::string_view name, object_id_t object_id, duration_t delay, const session_s_ptr_t& sess, task_t task) -> void;

	auto cancel(std::string_view name, object_id_t object_id) -> bool;
	auto cancel_all(object_id_t object_id) -> size_t;

	auto exists(std::string_view name, object_id_t object_id) const -> bool;
	auto size() const -> size_t;

private:
	auto add_impl(std::string_view name, object_id_t object_id, duration_t delay, entry_s_ptr_t entry) -> void;
	auto on_expired(const key_t& key, const entry_s_ptr_t& entry) -> void;
	static auto run_task(const key_t& key, const task_t& task) -> void;

private:
	boost::asio::io_context* io_context_ = nullptr;

	mutable std::mutex lock_{};
	std::map<key_t, entry_s_ptr_t> timers_{};
};
}
