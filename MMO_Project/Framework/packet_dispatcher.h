#pragma once
#include "pch.h"

class session;
struct packet_header;

using PacketHandler = std::function<void(std::shared_ptr<session>, const uint8_t* body, size_t size)>;

class packet_dispatcher
{
public:
    static packet_dispatcher& instance()
    {
        static packet_dispatcher inst;
        return inst;
    }

    void register_handler(uint16_t packet_id, PacketHandler handler)
    {
        handlers_[packet_id] = std::move(handler);
    }

    void dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size)
    {
        auto it = handlers_.find(packet_id);
        if (it != handlers_.end()) {
            it->second(sess, body_data, body_size);
        }
        else {
            std::cout << "Unknown packet ID: " << packet_id << std::endl;
        }
    }

private:
    std::unordered_map<uint16_t, PacketHandler> handlers_;
};