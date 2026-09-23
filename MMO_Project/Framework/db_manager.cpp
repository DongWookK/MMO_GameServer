#pragma once
#include "pch.h"
#include "db_manager.h"

db_manager::db_manager()
    : env_handle_(SQL_NULL_HENV), conn_handle_(SQL_NULL_HDBC), is_connected_(false) {}

db_manager::~db_manager() {
    disconnect();
}

bool db_manager::connect(const std::wstring& connectionString) {
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env_handle_);
    if (!SQL_SUCCEEDED(ret)) return false;

    // ODBC 버전 설정 (ODBC 3.x)
    ret = SQLSetEnvAttr(env_handle_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_ENV, env_handle_);
        return false;
    }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, env_handle_, &conn_handle_);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_ENV, env_handle_);
        return false;
    }

    ret = SQLDriverConnectW(
        conn_handle_,
        NULL,
        (SQLWCHAR*)connectionString.c_str(),
        SQL_NTS,
        NULL,
        0,
        NULL,
        SQL_DRIVER_NOPROMPT
    );

    if (!SQL_SUCCEEDED(ret)) {
        FLOG_CRITICAL("[DB Error] Connection Failed:");
        print_error(SQL_HANDLE_DBC, conn_handle_);
        disconnect();
        return false;
    }

    is_connected_ = true;
    FLOG_INFO("[DB Success] Connected to MS-SQL successfully!");
    return true;
}

void db_manager::disconnect() {
    if (conn_handle_ != SQL_NULL_HDBC) {
        if (is_connected_) {
            SQLDisconnect(conn_handle_);
        }
        SQLFreeHandle(SQL_HANDLE_DBC, conn_handle_);
        conn_handle_ = SQL_NULL_HDBC;
    }

    if (env_handle_ != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, env_handle_);
        env_handle_ = SQL_NULL_HENV;
    }

    is_connected_ = false;
}

auto db_manager::prepare() -> fw::error
{
    fw::error error_code{};
    for (auto& thread_sql : sqls_)
    {
        error_code = thread_sql->prepare();
        ASSERT_RETURN_VALUE(!error_code, error_code);
    }

    return error_code;
}

auto db_manager::get_connect_handle() const -> SQLHDBC
{
    return conn_handle_;
}

bool db_manager::execute_login_proc(int userId, const std::wstring& userName) {
    if (!is_connected_) return false;

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    // 문장(Statement) 핸들 할당
    ret = SQLAllocHandle(SQL_HANDLE_STMT, conn_handle_, &hStmt);
    if (!SQL_SUCCEEDED(ret)) return false;

    // 호출할 저장 프로시저 쿼리 구문 작성 (예: EXEC sp_CheckUser ?, ?)
    // ? 는 파라미터 바인딩을 위한 플레이스홀더입니다.
    SQLWCHAR* query = (SQLWCHAR*)L"{CALL sp_CheckUser (?, ?)}";

    ret = SQLPrepareW(hStmt, query, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::wcout << L"[DB Error] SQLPrepare Failed\n";
        print_error(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    // 파라미터 바인딩 변수 준비
    SQLLEN cbParam1 = 0;
    SQLLEN cbParam2 = SQL_NTS;

    // 첫 번째 파라미터 바인딩 (INT형 userId)
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userId, 0, &cbParam1);

    // 두 번째 파라미터 바인딩 (VARCHAR/NVARCHAR형 userName)
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, (SQLPOINTER)userName.c_str(), 0, &cbParam2);

    // 쿼리(저장 프로시저) 실행
    ret = SQLExecute(hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::wcout << L"[DB Error] SQLExecute (Stored Procedure) Failed\n";
        print_error(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    // SP 실행 결과로 SELECT 결과셋이 넘어오는 경우 처리
    SQLWCHAR outName[50] = { 0 };
    SQLLEN outNameInd = 0;
    int outLevel = 0;
    SQLLEN outLevelInd = 0;

    // 결과셋 순회
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        // 컬럼 데이터 가져오기 (첫 번째 컬럼: 이름, 두 번째 컬럼: 레벨 등 가정)
        SQLGetData(hStmt, 1, SQL_C_WCHAR, outName, sizeof(outName), &outNameInd);
        SQLGetData(hStmt, 2, SQL_C_LONG, &outLevel, sizeof(outLevel), &outLevelInd);

        std::wcout << L"[DB Result] User Name: " << outName << L", Level: " << outLevel << std::endl;
    }

    // 리소스 정리
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return true;
}

// 에러 로그 출력 헬퍼 함수
void db_manager::print_error(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLWCHAR sqlState[10] = { 0 };
    SQLINTEGER nativeError = 0;
    SQLWCHAR messageText[256] = { 0 };
    SQLSMALLINT textLength = 0;

    SQLGetDiagRecW(handleType, handle, 1, sqlState, &nativeError, messageText, sizeof(messageText) / sizeof(SQLWCHAR), &textLength);
    FLOG_INFO(L"SQL STATE: {}, Message: {}", std::wstring_view(sqlState), std::wstring_view(messageText));
}