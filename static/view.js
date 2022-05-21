controller = new WebSocket('ws://localhost:8080/view');

controller.binaryType = 'arraybuffer';

var started = false;

var frame_index = 0;

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
    var message = e.data

    if (typeof message === 'string') {

        var data = JSON.parse(message);

        if ('event' in data) {

            var event = data.event;

            if (event === 'info') {

                if ('nodes_count' in data) {
                    document.getElementById('label-nodes-count').innerHTML = data.nodes_count;
                }

            } else if (event === 'ready' && started) {

                send_calculate();

            }

        }

    } else {

        var canvas = document.getElementById('screen');

        var context = canvas.getContext("2d");

        const imageData = new ImageData(new Uint8ClampedArray(message), 500, 500);

        context.putImageData(imageData, 0, 0);

    }
}

controller.onclose = function() {
    console.log('controller disconnected');
}

update_button_start_stop();
