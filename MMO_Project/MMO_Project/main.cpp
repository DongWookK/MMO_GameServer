#pragma once
#include "pch.h"
#include "main_server.h"
#include "logger.h"

#include <csignal>
#include <future>
#include <windows.h>
#include <spdlog/spdlog.h>

// ----------------------------------------------------
// 서비스 설정
// ----------------------------------------------------
#define SERVICE_NAME TEXT("MMO_GameServer")

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = NULL;

// 콘솔/디버그 모드 종료 대기 객체
std::promise<void>    g_ConsoleShutdownPromise;

// 서비스 전용 제어 함수
void ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    g_ServiceStatus.dwCurrentState = dwCurrentState;
    g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    g_ServiceStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        g_ServiceStatus.dwControlsAccepted = 0;
    else
        g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        g_ServiceStatus.dwCheckPoint = 0;
    else
        g_ServiceStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

// 서비스 정지 요청(SCM)을 처리하는 콜백
VOID WINAPI ServiceCtrlHandler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        // 1. 서버 서비스 정지
        if (main_server::instance())
        {
            main_server::instance()->stop_service();
        }

        // 2. 서비스 메인 스레드 대기 해제
        if (g_ServiceStopEvent)
        {
            SetEvent(g_ServiceStopEvent);
        }
        break;

    default:
        break;
    }
}

// 서비스 엔트리 함수
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
    if (!g_StatusHandle) return;

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // 1. 로거 초기화
    int32_t error_code = initialize_logger();
    if (error_code != 0)
    {
        ReportSvcStatus(SERVICE_STOPPED, ERROR_FUNCTION_FAILED, 0);
        return;
    }

    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    // 2. 서비스 시작 보고 및 서버 시작
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    FLOG_INFO("main server::start_service");

    main_server::instance()->start_service();

    // 3. SCM에서 정지 명령(Stop Event)을 보낼 때까지 메인 스레드 대기
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);

    // 4. 종료 절차 진행
    FLOG_INFO("main server::graceful shutdown");
    spdlog::shutdown();

    CloseHandle(g_ServiceStopEvent);
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

int main(int argc, char* argv[])
{
    bool is_run_as_console = false;

#if defined(_DEBUG)
    is_run_as_console = true;
#endif

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-c" || std::string(argv[i]) == "--console")
        {
            is_run_as_console = true;
            break;
        }
    }

    if (is_run_as_console)
    {
        std::cout << "[DEBUG MODE] Running as console application..." << std::endl;

        int32_t error_code = initialize_logger();
        ASSERT_RETURN_VALUE(!(error_code), error_code);

        // Ctrl+C 시그널 등록
        std::signal(SIGINT, [](int) {
            std::cout << "\n[DEBUG MODE] Signal received. Stopping service..." << std::endl;
            if (main_server::instance())
            {
                main_server::instance()->stop_service();
            }
            g_ConsoleShutdownPromise.set_value();
            });

        FLOG_INFO("main server::start_service");
        main_server::instance()->start_service();

        std::cout << "Server is running. Press Ctrl+C to stop..." << std::endl;

        // Ctrl+C(stop) 명령까지 메인 스레드 대기
        g_ConsoleShutdownPromise.get_future().wait();

        FLOG_INFO("main server::graceful shutdown");
        spdlog::shutdown();

        return 0;
    }

    // Windows 서비스 모드 실행
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        { (LPTSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // .exe 순수실행한 케이스 예외처리
    if (!StartServiceCtrlDispatcher(ServiceTable))
    {
        std::cout << "This application must be run as a Windows Service." << std::endl;
        std::cout << "For debug/console mode, run with -c option." << std::endl;
    }

    return 0;
}