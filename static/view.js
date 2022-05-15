controller = new WebSocket('ws://localhost:8080/view');

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

function send_calculate() {
    controller.send(JSON.stringify(
        {
            event : 'calculate',
            frame_index: frame_index++
        }
    ));
}

function show_frame(frame) {
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

                console.log('received ready');

                send_calculate();

            }

        }

    } else {

        message.text().then(text => { console.log(text); })

    }
}

controller.onclose = function() {
    console.log('controller disconnected');
}

update_button_start_stop();
