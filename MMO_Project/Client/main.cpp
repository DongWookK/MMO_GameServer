#pragma once
#include "pch.h"
#include <thread>
#include <atomic>

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
        std::cout << "\n[Send Failed] Type: " << header.type << ", Error: " << ec.message() << endl;
    }
    else {
        std::cout << "\n[Send Success] Sent " << header.size << " bytes (Type: " << header.type << ")" << endl;
    }
}

// 1회 수신 함수 (에러 반환형으로 변경하여 연결 끊김 감지)
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
            std::cout << "\n[Read Header Failed] Error: " << ec.message() << endl;
        }
        return false; // 수신 실패 또는 연결 종료
    }

    size_t body_size = header.size - sizeof(PacketHeader);
    std::vector<uint8_t> body_buf(body_size);

    if (body_size > 0) {
        asio::read(sock, asio::buffer(body_buf.data(), body_size), ec);
        if (ec) {
            std::cout << "\n[Read Body Failed] Error: " << ec.message() << endl;
            return false;
        }
    }

    std::cout << "\n[Recv Success] Type: " << header.type << ", Total Size: " << header.size << " bytes" << endl;

    switch (static_cast<game::tr_type>(header.type))
    {
    case game::tr_type::TestEcho:
    {
        auto echo_pkt = flatbuffers::GetRoot<game::TestEcho>(body_buf.data());
        if (echo_pkt && echo_pkt->data()) {
            std::cout << " > TestEcho Response Data: " << echo_pkt->data()->str() << endl;
        }
    } break;
    case game::tr_type::UserLoginAck:
    {
        auto echo_pkt = flatbuffers::GetRoot<game::LoginAck>(body_buf.data());
        if (echo_pkt && echo_pkt->user_no()) {
            std::cout << " > LoginAck User No: " << echo_pkt->user_no() << endl;
        }
    } break;
    default:
    {
        std::cout << " > Unknown Packet Type: " << header.type << endl;
    } break;
    }

    return true; // 정상 수신 성공
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
#pragma region get_endpoint
    boost::system::error_code ec;
    asio::ip::address ip_address = asio::ip::make_address_v4(raw_ip_address, ec);

    if (ec.value() != 0)
    {
        std::cout << "Failed to parse IP address. Error Code = " << ec.value() << ". Message: " << ec.message() << endl;
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
        std::cout << "Failed to open socket. Error Code = " << ec.value() << ". Message: " << ec.message() << endl;
        return ec.value();
    }
#pragma endregion

    std::string message{};
    bool out = true;
    cout << "client ready (commands: connect, send_echo, send_login, disconnect)" << endl;

    while (out)
    {
        cout << "> ";
        cin >> message;

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
                std::cout << "Error occured! Error Code = " << e.code() << ". Message: " << e.what() << endl;
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
            send_packet(sock, game::tr_type::UserLoginReq, [](flatbuffers::FlatBufferBuilder& builder) {
                uint32_t user_no = 12345;
                return game::CreateLoginReq(builder, std::to_underlying(game::tr_type::UserLoginReq), user_no);
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