#pragma once
#include "pch.h"
#include "troc_user.h"

DECLARE_PACKET_TRAITS(game::tr_type::MoveReq, MoveReq);
DECLARE_PACKET_TRAITS(game::tr_type::MoveNotify, MoveNotify);

class troc_move : public feature
{
public:
	static constexpr auto min_move_interval = std::chrono::milliseconds(50);

public:
	troc_move() : feature("troc_move") {}

public:
	auto setup() -> fw::error override
	{
		REGISTER_TR(game::tr_type::MoveReq, MoveReq);

		return fw::error{};
	}

public:
	HANDLER_TR(MoveReq);
};
