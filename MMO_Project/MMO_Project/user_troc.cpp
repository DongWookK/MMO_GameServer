#pragma once
#include "pch.h"
#include "user_troc.h"
#include "packet_dispatcher.h"

auto troc_user::declare_tr_user_login_req(uint16_t tr_type)
{
	packet_dispatcher::instance()->register_handler(tr_type);
}
