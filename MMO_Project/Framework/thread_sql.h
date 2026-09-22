#pragma once
#include "pch.h"
#include "db_manager.h"

class thread_sql
{
public:
	thread_sql(SQLRETURN ret) : ret_(ret) {};
	virtual ~thread_sql() = default;

	virtual auto prepare() -> fw::error = 0;
	virtual auto exec() -> fw::error = 0;

protected:
	SQLRETURN ret_;
};