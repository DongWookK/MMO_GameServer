#pragma once
#include "pch.h"
#include "coroutine.h"
#include "packet_dispatcher.h"
#include "session.h"

auto fw::co_executor::to_db() const -> awaitable<void>
{
	if (nullptr == db_io_context_)
	{
		FLOG_CRITICAL("co_executor :: db io_context not set!");
		co_return;
	}

	co_await boost::asio::post(boost::asio::bind_executor(db_io_context_->get_executor(), boost::asio::use_awaitable));
}

auto fw::co_executor::to_session(const session_s_ptr_t& sess) -> awaitable<void>
{
	co_await boost::asio::post(boost::asio::bind_executor(*packet_dispatcher::instance()->get_strand(sess), boost::asio::use_awaitable));
}

auto fw::co_executor::co_start(const session_s_ptr_t& sess, awaitable<void> task, std::string_view name) -> void
{
	boost::asio::co_spawn(*packet_dispatcher::instance()->get_strand(sess), std::move(task),
		[name = std::string{ name }](std::exception_ptr e) {
			if (nullptr == e)
			{
				return;
			}

			try
			{
				std::rethrow_exception(e);
			}
			catch (const std::exception& ex)
			{
				FLOG_ERROR("co_executor :: name({}) exception -> {}", name, ex.what());
			}
			catch (...)
			{
				FLOG_ERROR("co_executor :: name({}) unknown exception", name);
			}
		});
}
