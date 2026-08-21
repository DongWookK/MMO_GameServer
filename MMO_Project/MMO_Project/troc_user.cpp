#pragma once
#include "pch.h"
#include "troc_user.h"
#include "session.h"

HANDLER_TR_DEFINE(troc_user, TestEcho)
{
    if (!sess) {
        return fw::error{ 111 };
    }

    // 핸들러 로직
    
    return fw::error{ 111 };
}
