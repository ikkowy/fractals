let x = Math.random() * 60 - 30;
let y = Math.random() * 60 - 30;
let z = Math.random() * 60 - 30;

const a = 15;
const b = 42;
const c = 2;

const canvas = document.getElementById('screen');
const ctx = canvas.getContext("2d");

ctx.fillStyle = "transparent";
ctx.fillRect(0, 0, canvas.width, canvas.height);

function point(x, y) {
    ctx.fillStyle = "red";
    ctx.beginPath();
    ctx.arc(x, y, 3, 0, 2 * Math.PI);
    ctx.fill();
}

function step() {
    let dt = 0.001;
    let dx = (a * (y - x)) * dt;
    let dy = (x * (b - z) - y) * dt;
    let dz = (x * y - c * z) * dt;
    x += dx;
    y += dy;
    z += dz;
}

function next() {
    step();
    point(6 * x + 250, 6 * y + 250);
}

// -----------------------------------------------------------------------------

controller = new WebSocket('ws://localhost:8080/view');

controller.binaryType = 'arraybuffer';

let started = false;

let frame_index = 0;

function send_start() {
    controller.send(JSON.stringify(
        { event : 'start' }
    ));
}

function send_stop() {
    controller.send(JSON.stringify(
        { event : 'stop' }
    ));
}

function send_calculate(c_real, c_imag) {
    controller.send(JSON.stringify(
        {
            event : 'calculate',
            frame_index : frame_index++,
            pixel_width : 500,
            pixel_height : 500,
            c_real : c_real,
            c_imag : c_imag
        }
    ));
}

function start() {
    if (!started) {
        started = true;
        send_start();
    }
}

function stop() {
    if (started) {
        started = false;
        send_stop();
    }
}

function update_button_start_stop() {
    document.getElementById('button-start-stop').innerHTML = started ? 'STOP' : 'START';
}

function toggle_start_stop() {
    if (started) stop();
    else start();
    update_button_start_stop();
}

controller.onopen = function() {
    console.log('controller connected');
}

controller.onmessage = function(e) {
    const message = e.data;

    if (typeof message === 'string') {

        const data = JSON.parse(message);

        if ('event' in data) {

            const event = data.event;

            if (event === 'info') {

                if ('nodes_count' in data) {
                    document.getElementById('label-nodes-count').innerHTML = data.nodes_count;
                }

            } else if (event === 'ready' && started) {

                const c_real = 20 * x / 500;
                const c_imag = 20 * y / 500;
                send_calculate(c_real, c_imag);

            }

        }

    } else {

        const canvas = document.getElementById('screen');

        const context = canvas.getContext("2d");

        const imageData = new ImageData(new Uint8ClampedArray(message), 500, 500);

        context.putImageData(imageData, 0, 0);

        next();

    }
}

controller.onclose = function() {
    console.log('controller disconnected');
}

update_button_start_stop();
