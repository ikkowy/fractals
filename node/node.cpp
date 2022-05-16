#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <json/json.h>

#include "websocket.h"

class Frame {

public:

    Frame() = default;

    Frame(unsigned int width, unsigned int height)
        : pixels(width * height), width{width}, height{height} {}

    inline void set_pixel(unsigned int x, unsigned int y, uint32_t value) {
        pixels[x * width + y] = value;
    }

    void* raw_data() {
        return pixels.data();
    }

    unsigned int raw_size() {
        return pixels.size() * sizeof(std::uint32_t);
    }

private:

    unsigned int width = 0;
    unsigned int height = 0;
    std::vector<std::uint32_t> pixels;

};

inline uint32_t rgb_color(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t) r << 24 | (uint32_t) g << 16 | (uint32_t) b << 8 | 0xff;
    //return htonl(color);
}

static bool waiting = false;

static Frame frame;

void send_ready(ws_connection* connection) {
    ws_send_string(connection, "{ \"event\" : \"ready\" }");
}

void send_waiting(ws_connection* connection) {
    ws_send_string(connection, "{ \"event\" : \"waiting\" }");
}

void calculate(ws_connection* connection, const Json::Value& data) {
    unsigned int frame_index = data["frame_index"].asInt();
    unsigned int pixel_width = data["pixel_width"].asInt();
    unsigned int pixel_height = data["pixel_height"].asInt();

    printf("calculating frame %d x %d\n", (int) pixel_width, (int) pixel_height);

    frame = Frame(pixel_width, pixel_height);

    for (unsigned int x = 0; x < pixel_width; x++) {
        for (unsigned int y = 0; y < pixel_height; y++) {
            frame.set_pixel(x, y, rgb_color(frame_index % 256, 0, 0));
        }
    }
}

static void on_message(
    ws_connection* connection,
    void* message,
    size_t size,
    bool binary
) {
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value data;
    JSONCPP_STRING err;
    bool okay = reader->parse((const char*) message, (const char*) message + size, &data, &err);
    if (okay) {
        const std::string event = data["event"].asString();
        if (event == "nudge") {
            send_ready(connection);
        } else if (event == "calculate") {
            calculate(connection, data);
            send_waiting(connection);
            waiting = true;
        } else if (event == "transfer") {
            if (waiting) {
                ws_send_data(connection, frame.raw_data(), frame.raw_size());
                send_ready(connection);
                waiting = false;
            }
        }
    }
}

int main() {
    ws_run_client("ws://localhost:8080/node", on_message);
    return EXIT_SUCCESS;
}
