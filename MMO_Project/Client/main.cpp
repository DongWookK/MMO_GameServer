#pragma once
#include "pch.h"
#include <thread>
#include <atomic>
#include <sstream>
#include <windows.h>
#include "enum_error_generated.h"
#include "enum_common_generated.h"

using namespace boost;
using namespace std;

static const std::string raw_ip_address = "127.0.0.1";
static constexpr uint16_t port_no = 221;

/* ------------------------------------------------
    Header Definition
------------------------------------------------*/
#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size; // 패킷 전체 크기 (헤더 + 바디)
    uint16_t type; // 패킷 타입 (tr_type)
};
#pragma pack(pop)

// 수신 스레드 제어용 플래그 및 스레차 객체
std::atomic<bool> is_receiving{ false };
std::thread recv_thread;

std::atomic<int32_t> last_user_no{ 0 };

std::atomic<int64_t> last_pc_no{ 0 };
std::string to_console(const std::string& acp_str)
{
    if (acp_str.empty()) return {};

    int wsize = MultiByteToWideChar(CP_ACP, 0, acp_str.data(), static_cast<int>(acp_str.size()), nullptr, 0);
    std::wstring wstr(wsize, L'\0');
    MultiByteToWideChar(CP_ACP, 0, acp_str.data(), static_cast<int>(acp_str.size()), wstr.data(), wsize);

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wsize, nullptr, 0, nullptr, nullptr);
    std::string out(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wsize, out.data(), size, nullptr, nullptr);
    return out;
}

bool read_line_utf8(std::string& out)
{
    out.clear();

    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    if (!GetConsoleMode(in, &mode))
    {
        // 콘솔이 아닌 경우 (파이프/리다이렉트 입력) : 이미 UTF-8 이라고 가정
        return static_cast<bool>(std::getline(std::cin, out));
    }

    std::wstring wline;
    wchar_t buf[256];
    DWORD read = 0;
    while (ReadConsoleW(in, buf, static_cast<DWORD>(std::size(buf)), &read, nullptr) && read > 0)
    {
        wline.append(buf, read);
        if (wline.back() == L'\n') break;
    }

    if (wline.empty()) return false;

    while (!wline.empty() && (wline.back() == L'\n' || wline.back() == L'\r'))
    {
        wline.pop_back();
    }

    if (wline.empty()) return true;

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wline.data(), static_cast<int>(wline.size()), nullptr, 0, nullptr, nullptr);
    out.resize(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wline.data(), static_cast<int>(wline.size()), out.data(), size_needed, nullptr, nullptr);
    return true;
}

/* ------------------------------------------------
    Generic Write & Read
------------------------------------------------*/
template <typename PacketBuilderFunc>
void send_packet(asio::ip::tcp::socket& sock, game::tr_type packet_type, PacketBuilderFunc builder_func)
{
    boost::system::error_code ec;
    flatbuffers::FlatBufferBuilder builder;

    auto root_offset = builder_func(builder);
    builder.Finish(root_offset);

    const uint8_t* data = builder.GetBufferPointer();
    size_t body_size = builder.GetSize();

    PacketHeader header;
    header.size = static_cast<uint16_t>(sizeof(PacketHeader) + body_size);
    header.type = std::to_underlying(packet_type);

    std::vector<asio::const_buffer> buffers;
    buffers.push_back(asio::buffer(&header, sizeof(header)));
    buffers.push_back(asio::buffer(data, body_size));

    asio::write(sock, buffers, ec);

    if (ec) {
        std::cout << "\n[Send Failed] Type: " << header.type << ", Error: " << to_console(ec.message()) << endl;
    }
    else {
        std::cout << "\n[Send Success] Sent " << header.size << " bytes (Type: " << header.type << ")" << endl;
    }
}
// 1회 수신 함수
bool read_from_socket(asio::ip::tcp::socket& sock)
{
    boost::system::error_code ec;

    PacketHeader header{};
    asio::read(sock, asio::buffer(&header, sizeof(header)), ec);

    if (ec) {
        if (ec == asio::error::eof || ec == asio::error::operation_aborted) {
            std::cout << "\n[Server Closed Connection]" << endl;
        }
        else {
            std::cout << "\n[Read Header Failed] Error: " << to_console(ec.message()) << endl;
        }
        return false;
    }

    // header.size가 PacketHeader보다 작을 경우 발생할 Underflow 방지
    if (header.size < sizeof(PacketHeader)) {
        std::cout << "\n[Invalid Header] Packet size is smaller than header size." << endl;
        return false;
    }

    size_t body_size = header.size - sizeof(PacketHeader);
    std::vector<uint8_t> body_buf(body_size);

    if (body_size > 0) {
        asio::read(sock, asio::buffer(body_buf.data(), body_size), ec);
        if (ec) {
            std::cout << "\n[Read Body Failed] Error: " << to_console(ec.message()) << endl;
            return false;
        }
    }

    std::cout << "\n[Recv Success] Type: " << game::EnumNametr_type(static_cast<game::tr_type>(header.type))
              << "(" << header.type << "), Total Size: " << header.size << " bytes" << endl;

    // FlatBuffers Verifier 생성 (수신된 body_buf의 안전성 검사)
    flatbuffers::Verifier verifier(body_buf.data(), body_buf.size());

    switch (static_cast<game::tr_type>(header.type))
    {
    case game::tr_type::TestEcho:
    {
        if (!verifier.VerifyBuffer<game::TestEcho>(nullptr)) {
            std::cout << " > [Error] Invalid TestEcho FlatBuffer payload!" << endl;
            return false;
        }

        auto echo_pkt = flatbuffers::GetRoot<game::TestEcho>(body_buf.data());
        if (echo_pkt && echo_pkt->data()) {
            std::cout << " > TestEcho Response Data: " << echo_pkt->data()->str() << endl;
        }
    } break;

    case game::tr_type::UserLoginAck:
    {
        if (!verifier.VerifyBuffer<game::UserLoginAck>(nullptr)) {
            std::cout << " > [Error] Invalid LoginAck FlatBuffer payload!" << endl;
            return false;
        }

        auto login_pkt = flatbuffers::GetRoot<game::UserLoginAck>(body_buf.data());
        if (login_pkt) {
            last_user_no = login_pkt->user_no();
            std::cout << " > LoginAck User Name: " << (login_pkt->user_name() ? login_pkt->user_name()->str() : "(null)")
                      << ", User No: " << login_pkt->user_no() << endl;
        }
    } break;

    case game::tr_type::UserLogoutAck:
    {
        if (!verifier.VerifyBuffer<game::UserLogoutAck>(nullptr)) {
            std::cout << " > [Error] Invalid LogoutAck FlatBuffer payload!" << endl;
            return false;
        }

        auto logout_pkt = flatbuffers::GetRoot<game::UserLogoutAck>(body_buf.data());
        if (logout_pkt) {
            last_user_no = 0;
            std::cout << " > LogoutAck User No: " << logout_pkt->user_no() << endl;
        }
    } break;

    case game::tr_type::Nak:
    {
        if (!verifier.VerifyBuffer<game::Nak>(nullptr)) {
            std::cout << " > [Error] Invalid Nak FlatBuffer payload!" << endl;
            return false;
        }

        auto nak_pkt = flatbuffers::GetRoot<game::Nak>(body_buf.data());
        if (nak_pkt) {
            const auto req_type = static_cast<game::tr_type>(nak_pkt->req_type());
            const auto error_code = static_cast<::error::code>(nak_pkt->error_code());

            std::cout << " > Nak Request: " << game::EnumNametr_type(req_type) << "(" << nak_pkt->req_type() << ")"
                      << ", Error: " << ::error::EnumNamecode(error_code) << "(" << nak_pkt->error_code() << ")" << endl;
        }
    } break;

    case game::tr_type::PcCreateAck:
    {
        if (!verifier.VerifyBuffer<game::PcCreateAck>(nullptr)) {
            std::cout << " > [Error] Invalid PcCreateAck FlatBuffer payload!" << endl;
            return false;
        }

        auto create_pkt = flatbuffers::GetRoot<game::PcCreateAck>(body_buf.data());
        if (create_pkt) {
            last_pc_no = create_pkt->pc_no();
            std::cout << " > PcCreateAck Pc No: " << create_pkt->pc_no()
                      << ", Name: " << (create_pkt->pc_name() ? create_pkt->pc_name()->str() : "(null)")
                      << ", Type: " << common::EnumNamepc_type(static_cast<common::pc_type>(create_pkt->pc_type())) << endl;
        }
    } break;

    case game::tr_type::PcListNotify:
    {
        if (!verifier.VerifyBuffer<game::PcListNotify>(nullptr)) {
            std::cout << " > [Error] Invalid PcListNotify FlatBuffer payload!" << endl;
            return false;
        }

        auto list_pkt = flatbuffers::GetRoot<game::PcListNotify>(body_buf.data());
        if (list_pkt) {
            const auto count = list_pkt->pc_list() ? list_pkt->pc_list()->size() : 0;
            std::cout << " > PcListNotify Count: " << count << endl;
            if (list_pkt->pc_list()) {
                for (const auto summary : *list_pkt->pc_list()) {
                    std::cout << "   - Pc No: " << summary->pc_no()
                              << ", Name: " << (summary->pc_name() ? summary->pc_name()->str() : "(null)")
                              << ", Type: " << common::EnumNamepc_type(static_cast<common::pc_type>(summary->pc_type()))
                              << ", Lv: " << summary->level() << endl;
                    last_pc_no = summary->pc_no();
                }
            }
        }
    } break;

    case game::tr_type::PcSelectAck:
    {
        if (!verifier.VerifyBuffer<game::PcSelectAck>(nullptr)) {
            std::cout << " > [Error] Invalid PcSelectAck FlatBuffer payload!" << endl;
            return false;
        }

        auto select_pkt = flatbuffers::GetRoot<game::PcSelectAck>(body_buf.data());
        if (select_pkt) {
            std::cout << " > PcSelectAck Object Id: " << select_pkt->object_id()
                      << ", Pc No: " << select_pkt->pc_no()
                      << ", Name: " << (select_pkt->pc_name() ? select_pkt->pc_name()->str() : "(null)")
                      << ", Type: " << common::EnumNamepc_type(static_cast<common::pc_type>(select_pkt->pc_type()))
                      << ", Lv: " << select_pkt->level() << ", Exp: " << select_pkt->exp()
                      << ", HP: " << select_pkt->hp() << ", MP: " << select_pkt->mp();
            if (auto pos = select_pkt->pos()) {
                std::cout << ", Pos: (" << pos->x() << ", " << pos->y() << ", " << pos->z() << ")";
            }
            std::cout << endl;
        }
    } break;

    default:
    {
        std::cout << " > Unknown Packet Type: " << header.type << endl;
    } break;
    }

    return true;
}

// 백그라운드 수신 루프 스레드 함수
void start_receive_loop(asio::ip::tcp::socket& sock)
{
    is_receiving = true;
    while (is_receiving)
    {
        // 서버로부터 패킷이 오기 전까지 대기(Blocking)하다가 수신 처리
        if (!read_from_socket(sock))
        {
            break;
        }
    }
    is_receiving = false;
}

// 수신 스레드 안전 종료 함수
void stop_receive_thread()
{
    is_receiving = false;
    if (recv_thread.joinable()) {
        recv_thread.join();
    }
}

/* ------------------------------------------------
    Main
------------------------------------------------*/
int main()
{
    SetConsoleOutputCP(CP_UTF8);

#pragma region get_endpoint
    boost::system::error_code ec;
    asio::ip::address ip_address = asio::ip::make_address_v4(raw_ip_address, ec);

    if (ec.value() != 0)
    {
        std::cout << "Failed to parse IP address. Error Code = " << ec.value() << ". Message: " << to_console(ec.message()) << endl;
        return ec.value();
    }

    asio::ip::tcp::endpoint ep(ip_address, port_no);
#pragma endregion

#pragma region create active socket
    asio::io_context ioc;
    asio::ip::tcp::socket sock(ioc);
    sock.open(asio::ip::tcp::v4(), ec);

    if (ec.value() != 0)
    {
        std::cout << "Failed to open socket. Error Code = " << ec.value() << ". Message: " << to_console(ec.message()) << endl;
        return ec.value();
    }
#pragma endregion

    std::string line{};
    bool out = true;
    cout << "client ready (commands: connect, send_echo, send_login [user_name], send_logout, "
            "send_pc_create [pc_name] [pc_type], send_pc_select [pc_no], disconnect)" << endl;

    while (out)
    {
        cout << "> ";
        if (!read_line_utf8(line))
        {
            line = "disconnect";
        }

        std::istringstream iss(line);
        std::string message{};
        std::string arg{};
        std::string arg2{};
        iss >> message >> arg >> arg2;

        if (message.empty())
        {
            continue;
        }

        if ("connect" == message)
        {
            try
            {
                sock.connect(ep);
                std::cout << "Client Connected to " << ep.address().to_string() << ":" << ep.port() << endl;

                // 연결 성공 시 수신 스레드 시작
                stop_receive_thread();
                recv_thread = std::thread(start_receive_loop, std::ref(sock));
            }
            catch (system::system_error& e)
            {
                std::cout << "Error occured! Error Code = " << e.code() << ". Message: " << to_console(e.what()) << endl;
            }
        }
        else if ("disconnect" == message)
        {
            boost::system::error_code shutdown_ec;
            sock.shutdown(asio::ip::tcp::socket::shutdown_both, shutdown_ec);
            sock.close(shutdown_ec);

            stop_receive_thread();
            out = false;
        }
        else if ("send_echo" == message)
        {
            send_packet(sock, game::tr_type::TestEcho, [](flatbuffers::FlatBufferBuilder& builder) {
                auto str_data = builder.CreateString("hello_echo");
                return game::CreateTestEcho(builder, std::to_underlying(game::tr_type::TestEcho), str_data);
                });
        }
        else if ("send_login" == message)
        {
            const std::string user_name = arg.empty() ? "test_user" : arg;

            send_packet(sock, game::tr_type::UserLoginReq, [&user_name](flatbuffers::FlatBufferBuilder& builder) {
                auto name = builder.CreateString(user_name);
                return game::CreateUserLoginReq(builder, std::to_underlying(game::tr_type::UserLoginReq), name);
                });
        }
        else if ("send_logout" == message)
        {
            const int32_t user_no = last_user_no;

            send_packet(sock, game::tr_type::UserLogoutReq, [user_no](flatbuffers::FlatBufferBuilder& builder) {
                return game::CreateUserLogoutReq(builder, std::to_underlying(game::tr_type::UserLogoutReq), user_no);
                });
        }
        else if ("send_pc_create" == message)
        {
            const std::string pc_name = arg.empty() ? "test_pc" : arg;
            const uint8_t pc_type = arg2.empty() ? 0 : static_cast<uint8_t>(std::atoi(arg2.c_str()));

            send_packet(sock, game::tr_type::PcCreateReq, [&pc_name, pc_type](flatbuffers::FlatBufferBuilder& builder) {
                auto name = builder.CreateString(pc_name);
                return game::CreatePcCreateReq(builder, std::to_underlying(game::tr_type::PcCreateReq), name, pc_type);
                });
        }
        else if ("send_pc_select" == message)
        {
            const int64_t pc_no = arg.empty() ? static_cast<int64_t>(last_pc_no) : std::atoll(arg.c_str());

            send_packet(sock, game::tr_type::PcSelectReq, [pc_no](flatbuffers::FlatBufferBuilder& builder) {
                return game::CreatePcSelectReq(builder, std::to_underlying(game::tr_type::PcSelectReq), pc_no);
                });
        }
        else
        {
            std::cout << "invalid command" << endl;
        }
    }

    stop_receive_thread();

    std::cout << "~client terminated" << endl;
    return 0;
}