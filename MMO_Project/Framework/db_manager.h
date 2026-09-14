#pragma once
#pragma comment(lib, "odbc32.lib")
#include "pch.h"

class db_manager {
public:
    db_manager();
    ~db_manager();

    // 데이터베이스 연결 및 해제
    bool connect(const std::wstring& connectionString);
    void disconnect();

    // 저장 프로시저 실행 예시 (예: 유저 로그인 검증 또는 정보 조회)
    bool execute_login_proc(int userId, const std::wstring& userName);

private:
    void print_error(SQLSMALLINT handleType, SQLHANDLE handle);

    SQLHENV  m_env;   // 환경 핸들
    SQLHDBC  m_dbc;   // 연결 핸들
    bool     m_isConnected;
};