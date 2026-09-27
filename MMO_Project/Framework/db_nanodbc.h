#pragma once

// nanodbc 는 반드시 이 헤더를 통해서만 include 한다.
// NANODBC_ENABLE_UNICODE 정의 여부에 따라 nanodbc::string 타입이 달라지므로(ODR),
// 모든 번역 단위(nanodbc_impl.cpp 포함)가 같은 설정을 보도록 한 곳에 모아둔다.
// MSVC + UNICODE : nanodbc::string == std::wstring
#ifndef NANODBC_ENABLE_UNICODE
#define NANODBC_ENABLE_UNICODE
#endif

#include <nanodbc/nanodbc.h>
