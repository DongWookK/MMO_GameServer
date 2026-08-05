#pragma once
#include "pch.h"
#include "game_generated.h"
#include "enum_game_generated.h"
#include "packet_dispatcher.h"

class troc_user
{
private:
    REGISTER_TR(TestEcho)

public:
    HANDLER_TR(TestEcho)

};