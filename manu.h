#pragma once
#include <zmqpp/zmqpp.hpp>
template <typename T>
zmqpp::message& operator<<(zmqpp::message& msg, const std::vector<T>& buffer) {
    msg.add_raw(reinterpret_cast<const void*>(buffer.data()),
                sizeof(T) * buffer.size());
    return msg;
}
template <typename T>
zmqpp::message& operator>>(zmqpp::message& msg, std::vector<T>& buffer) {
    size_t part = msg.read_cursor();
    const T* data = static_cast<const T*>(msg.raw_data(part));
    size_t len = msg.size(part);  // Size in bytes
    msg.next();
    if ((len % sizeof(T)) == 0) {
        buffer.assign(data, data + (len / sizeof(T)));
    }
    return msg;
}