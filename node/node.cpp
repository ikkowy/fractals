#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sstream>
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
    uint32_t color;
    ((uint8_t*) &color)[0] = r;
    ((uint8_t*) &color)[1] = g;
    ((uint8_t*) &color)[2] = b;
    ((uint8_t*) &color)[3] = 0xff;
    return color;
}

static bool waiting = false;

static Frame frame;

static void julia(
    unsigned int pixel_width,
    unsigned int pixel_height,
    std::complex<double> c
) {
    frame = Frame(pixel_width, pixel_height);

    auto f = [](std::complex<double> z, std::complex<double> c) { return z * z + c; };

    for (unsigned int x = 0; x < pixel_width; x++) {
        for (unsigned int y = 0; y < pixel_height; y++) {
            double a = 4.0 * static_cast<double>(x) / static_cast<double>(pixel_width) - 2.0;
            double b = 4.0 * static_cast<double>(y) / static_cast<double>(pixel_height) - 2.0;
            std::complex<double> z(a, b);

            int i;

            for (i = 0; i < 32; i++) {
                z = f(z, c);
                if (std::abs(z) > 16.0) break;
            }

            frame.set_pixel(x, y, rgb_color(i / 32.0 * 255, i / 32.0 * 255, i / 32.0 * 255));
        }
    }
}

static void send_ready(ws_connection* connection) {
    ws_send_string(connection, "{ \"event\" : \"ready\" }");
}

static void send_waiting(ws_connection* connection) {
    ws_send_string(connection, "{ \"event\" : \"waiting\" }");
}

static void calculate(ws_connection* connection, const Json::Value& data) {
    unsigned int frame_index = data["frame_index"].asInt();
    unsigned int pixel_width = data["pixel_width"].asInt();
    unsigned int pixel_height = data["pixel_height"].asInt();

    julia(pixel_width, pixel_height, static_cast<double>(frame_index) / 1000.0);
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* host = argv[1];
    int port = std::atoi(argv[2]);
    std::stringstream uri;
    uri << "ws://" << host << ":" << port << "/node";

    ws_run_client(uri.str().c_str(), on_message);

    return EXIT_SUCCESS;
}
