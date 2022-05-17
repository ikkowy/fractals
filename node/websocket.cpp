#include <iostream>
#include <string.h>
#include <unordered_map>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include "websocket.h"

using client_type = websocketpp::client<websocketpp::config::asio_client>;

struct ws_connection {
    client_type* client;
    websocketpp::connection_hdl hdl;
};

static thread_local ws_message_callback message_callback;

static void message_handler(
    client_type* client,
    websocketpp::connection_hdl hdl,
    websocketpp::config::asio_client::message_type::ptr msg
) {
    ws_connection connection;
    connection.client = client;
    connection.hdl = hdl;
    auto payload = msg->get_payload();
    auto opcode = msg->get_opcode();
    bool binary = (opcode == websocketpp::frame::opcode::text);
    message_callback(&connection, payload.data(), payload.size(), binary);
}

void ws_run_client(
    const char* uri,
    ws_message_callback message_callback
) {
    ::message_callback = message_callback;

    try {
        client_type client;

        client.set_access_channels(websocketpp::log::alevel::all);
        client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        client.init_asio();

        //client.set_open_handler(websocketpp::lib::bind(&on_open, &client, ::_1));

        client.set_message_handler(websocketpp::lib::bind(
            &message_handler,
            &client,
            websocketpp::lib::placeholders::_1,
            websocketpp::lib::placeholders::_2
        ));

        websocketpp::lib::error_code error_code;

        client_type::connection_ptr connection = client.get_connection(uri, error_code);

        if (error_code) {
            std::cerr << "websocket connect failed: " << error_code.message() << std::endl;
            return;
        }

        client.connect(connection);

        client.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}

void ws_send_string(ws_connection* connection, const char* str) {
    connection->client->send(connection->hdl, std::string(str), websocketpp::frame::opcode::text);
}

void ws_send_string_with_size(ws_connection* connection, const char* str, size_t size) {
    connection->client->send(connection->hdl, std::string(str, size), websocketpp::frame::opcode::text);
}

void ws_send_data(ws_connection* connection, const void* data, size_t size) {
    connection->client->send(connection->hdl, std::string((const char*) data, size), websocketpp::frame::opcode::binary);
}
