#pragma once
#include "pch.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>

class session;

namespace fw
{
template <typename T = void>
using awaitable = boost::asio::awaitable<T>;
class co_executor : public singleton<co_executor>
{
	friend class singleton<co_executor>;

public:
	using session_s_ptr_t = std::shared_ptr<session>;

public:
	auto setup(boost::asio::io_context* db_io_context) -> void { db_io_context_ = db_io_context; }

	auto to_db() const -> awaitable<void>;
	static auto to_session(const session_s_ptr_t& sess) -> awaitable<void>;
	static auto co_start(const session_s_ptr_t& sess, awaitable<void> task, std::string_view name) -> void;

private:
	boost::asio::io_context* db_io_context_ = nullptr;
};
}
