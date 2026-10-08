#pragma once
#include "pch.h"
#include "timer_manager.h"
#include "packet_dispatcher.h"
#include "session.h"

auto fw::timer_manager::setup(boost::asio::io_context* io_context) -> void
{
	io_context_ = io_context;
}

auto fw::timer_manager::stop() -> void
{
	std::lock_guard lock(lock_);

	for (auto& [key, entry] : timers_)
	{
		entry->timer.cancel();
	}
	timers_.clear();
}

auto fw::timer_manager::add(std::string_view name, object_id_t object_id, duration_t delay, task_t task) -> void
{
	ASSERT_RETURN(io_context_ != nullptr);
	ASSERT_RETURN(task != nullptr);

	auto entry = std::make_shared<entry_t>(*io_context_);
	entry->task = std::move(task);

	add_impl(name, object_id, delay, std::move(entry));
}

auto fw::timer_manager::add(std::string_view name, object_id_t object_id, duration_t delay, const session_s_ptr_t& sess, task_t task) -> void
{
	ASSERT_RETURN(io_context_ != nullptr);
	ASSERT_RETURN(task != nullptr);
	ASSERT_RETURN(sess != nullptr);

	auto entry = std::make_shared<entry_t>(*io_context_);
	entry->task = std::move(task);
	entry->use_session = true;
	entry->sess = sess;

	add_impl(name, object_id, delay, std::move(entry));
}

auto fw::timer_manager::add_impl(std::string_view name, object_id_t object_id, duration_t delay, entry_s_ptr_t entry) -> void
{
	key_t key{ object_id, std::string{ name } };

	std::lock_guard lock(lock_);

	if (auto it = timers_.find(key); it != timers_.end())
	{
		it->second->timer.cancel();
		timers_.erase(it);
	}

	entry->timer.expires_after(delay);
	entry->timer.async_wait([this, key, entry](const boost::system::error_code& ec) {
		if (ec == boost::asio::error::operation_aborted)
		{
			return;
		}
		on_expired(key, entry);
		});

	timers_.emplace(std::move(key), std::move(entry));
}

auto fw::timer_manager::on_expired(const key_t& key, const entry_s_ptr_t& entry) -> void
{
	{
		std::lock_guard lock(lock_);

		auto it = timers_.find(key);
		if (it == timers_.end() || it->second != entry)
		{
			return;
		}
		timers_.erase(it);
	}

	if (!entry->use_session)
	{
		run_task(key, entry->task);
		return;
	}

	auto sess = entry->sess.lock();
	if (sess == nullptr)
	{
		return;
	}

	packet_dispatcher::instance()->post(sess, [key, task = std::move(entry->task)]() {
		run_task(key, task);
		});
}

auto fw::timer_manager::run_task(const key_t& key, const task_t& task) -> void
{
	try
	{
		task();
	}
	catch (const std::exception& e)
	{
		FLOG_ERROR("timer_manager :: task exception name({}) object({}) -> {}", key.name, key.object_id, e.what());
	}
}

auto fw::timer_manager::cancel(std::string_view name, object_id_t object_id) -> bool
{
	std::lock_guard lock(lock_);

	auto it = timers_.find(key_t{ object_id, std::string{ name } });
	if (it == timers_.end())
	{
		return false;
	}

	it->second->timer.cancel();
	timers_.erase(it);
	return true;
}

auto fw::timer_manager::cancel_all(object_id_t object_id) -> size_t
{
	std::lock_guard lock(lock_);

	size_t count = 0;
	auto it = timers_.lower_bound(key_t{ object_id, std::string{} });
	while (it != timers_.end() && it->first.object_id == object_id)
	{
		it->second->timer.cancel();
		it = timers_.erase(it);
		++count;
	}

	return count;
}

auto fw::timer_manager::exists(std::string_view name, object_id_t object_id) const -> bool
{
	std::lock_guard lock(lock_);
	return timers_.contains(key_t{ object_id, std::string{ name } });
}

auto fw::timer_manager::size() const -> size_t
{
	std::lock_guard lock(lock_);
	return timers_.size();
}
