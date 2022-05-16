#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct ws_connection ws_connection;

typedef void (*ws_message_callback)(
    ws_connection* connection,
    void* data,
    size_t size,
    bool binary
);

void ws_run_client(
    const char* uri,
    ws_message_callback message_callback
);

void ws_send_string(ws_connection* connection, const char* str);

void ws_send_string_with_size(ws_connection* connection, const char* str, size_t size);

void ws_send_data(ws_connection* connection, const void* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* WEBSOCKET_H */
