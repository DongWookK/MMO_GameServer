#pragma once
#include "pch.h"
#include "user_troc.h"
#include "session.h"

HANDLER_TR_DEFINE(troc_user, TestEcho)
{
    // 핸들러 로직 작성
    if (!sess) {
        return fw::error{ 111 };
    }

    // pkt 처리...

    return fw::error{ 111 };
}